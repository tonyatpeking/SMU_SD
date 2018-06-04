#include "Engine/UI/Menu.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Intersection.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextMeshBuilder.hpp"
#include "Engine/Renderer/ShaderPass.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/UBO.hpp"
#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Time/Tween.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/IO/ObjLoader.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Renderer/CubeMap.hpp"

#include "Game/Projectile.hpp"
#include "Game/GameState_Playing.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/Asteroid.hpp"


GameState_Playing::GameState_Playing()
    : GameState( GameStateType::PLAYING )
{
    g_encounterState = this;
}

GameState_Playing::~GameState_Playing()
{
    delete g_mainCamera;
    g_encounterState = nullptr;
}

void GameState_Playing::Update()
{
    // switch phase before all updates, this is after process input
    GameState::Update();

    for( auto& go : m_projectiles )
        go->Update();

    for( auto& go : m_asteroids )
        go->Update();

    CheckCollisions();

    for( int goIdx = (int) m_asteroids.size() - 1; goIdx >= 0; --goIdx )
    {
        Asteroid* asteroid = m_asteroids[goIdx];
        if( asteroid->ShouldDie() )
        {
            if( asteroid->m_maxHealth > 2 )
            {
                MakeAsteroid( asteroid->m_maxHealth / 2.f,
                              asteroid->GetTransform().GetWorldPosition() );
                MakeAsteroid( asteroid->m_maxHealth / 2.f,
                              asteroid->GetTransform().GetWorldPosition() );
            }
            g_renderSceneGraph->RemoveGameObject( asteroid );
            delete asteroid;
            ContainerUtils::EraseAtIndexFast( m_asteroids, goIdx );
        }
    }

    for( int goIdx = (int) m_projectiles.size() - 1; goIdx >= 0; --goIdx )
    {
        if( m_projectiles[goIdx]->ShouldDie() )
        {
            g_renderSceneGraph->RemoveGameObject( m_projectiles[goIdx] );
            delete m_projectiles[goIdx];
            ContainerUtils::EraseAtIndexFast( m_projectiles, goIdx );
        }
    }

    UpdateLights();

    TestUpdate();

    for( auto& emitter : m_emitters )
        emitter->Update();

    for( int emitterIdx = (int) m_emitters.size() - 1; emitterIdx >= 0; --emitterIdx )
    {
        if( m_emitters[emitterIdx]->ShouldDie() )
        {
            g_renderSceneGraph->RemoveGameObject( m_emitters[emitterIdx] );
            delete  m_emitters[emitterIdx];
            ContainerUtils::EraseAtIndexFast( m_emitters, emitterIdx );
        }
    }
}

void GameState_Playing::Render() const
{
    GameState::Render();

    TestRender();
}

void GameState_Playing::OnEnter()
{
    GameState::OnEnter();

    CreateCamera();

    DebugRender::Set3DCamera( g_mainCamera );

    g_renderer->SetBackGroundColor( Rgba::BLACK );

    MakeSpaceShip();

    AttatchCameraToShip();

    LoadMiku();

    MakeLights();

    MakeAsteroids();

    MakeSkyBox();

    SetAmbient( 0.1f );

    TestEnter();

    CreateNoiseImage();
}

void GameState_Playing::OnExit()
{
    GameState::OnExit();
}

void GameState_Playing::ProcessInput()
{
    GameState::ProcessInput();
    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_ESCAPE ) )
        g_app->OnQuitRequested();
    ProcessMovementInput();
    TestInput();
}



void GameState_Playing::CreateCamera()
{
    delete g_mainCamera;
    g_mainCamera = new Camera();
    g_renderSceneGraph->AddCamera( g_mainCamera );
    g_mainCamera->SetColorTarget( g_renderer->DefaultColorTarget() );
    g_mainCamera->SetDepthStencilTarget( g_renderer->DefaultDepthTarget() );
    g_mainCamera->SetProjection( m_fov, m_near, m_far ); //fov near far

}

void GameState_Playing::ProcessMovementInput()
{
    float ds = g_gameClock->GetDeltaSecondsF();
    Transform& shipTransform = m_shipHull->GetTransform();

    bool isMoving = false;

    if( g_input->IsKeyPressed( 'W' ) )
    {
        shipTransform.TranslateLocal( Vec3::FORWARD * ds * m_moveSpeed );
        isMoving = true;
    }
    if( g_input->IsKeyPressed( 'A' ) )
    {
        shipTransform.TranslateLocal( Vec3::LEFT * ds * m_moveSpeed );
        isMoving = true;
    }
    if( g_input->IsKeyPressed( 'S' ) )
    {
        shipTransform.TranslateLocal( Vec3::BACKWARD * ds * m_moveSpeed );
        isMoving = true;
    }
    if( g_input->IsKeyPressed( 'D' ) )
    {
        shipTransform.TranslateLocal( Vec3::RIGHT * ds * m_moveSpeed );
        isMoving = true;
    }

    if( isMoving )
        SetExhaustSpawnRate( 60 );
    else
        SetExhaustSpawnRate( 0.001f );

    float deltaYaw = 0;
    float deltaPitch = 0;
    float deltaRoll = 0;

    if( g_input->IsKeyPressed( 'Q' ) )
        deltaRoll = ds * m_rollSpeed;
    if( g_input->IsKeyPressed( 'E' ) )
        deltaRoll = -ds * m_rollSpeed;

    Vec2 cursorDelta = g_input->GetCursorDelta();
    if( cursorDelta.GetLengthSquared() > 0.01f )
    {
        deltaYaw = cursorDelta.x * m_turnSpeed;
        deltaPitch = cursorDelta.y * m_turnSpeed;
    }
    shipTransform.RotateLocalEuler( Vec3( deltaPitch, deltaYaw, deltaRoll ) );


    if( g_input->IsKeyPressed( InputSystem::MOUSE_LEFT ) )
    {
        static float timeSinceLastSpawn = 0;
        static bool alternator = false;
        timeSinceLastSpawn += ds;
        if( timeSinceLastSpawn > 0.15f )
        {
            timeSinceLastSpawn = 0;
            if( alternator )
                MakeProjectile( Vec3( 4.5f, 1, 8 ) );
            else
                MakeProjectile( Vec3( -4.5f, 1, 8 ) );
            alternator = !alternator;
        }

    }
}

void GameState_Playing::MakePrimitiveShapes()
{
    Material* mat = new Material();
    mat->m_specularAmount = 0.7f;
    mat->m_specularPower = 30.f;
    mat->m_shaderPass = new ShaderPass();
    mat->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles(
        "Data/Shaders/lit" );

    mat->m_diffuse = g_renderer->CreateOrGetTexture( "Data/Images/Couch/example_colour.png" );
    mat->m_normal = g_renderer->CreateOrGetTexture( "Data/Images/Couch/example_normal.png" );

    Material* matDouble = new Material();
    matDouble->m_specularAmount = 0.7f;
    matDouble->m_specularPower = 30.f;
    matDouble->m_shaderPass = new ShaderPass();
    matDouble->m_shaderPass->SetCullMode( CullMode::NONE );
    matDouble->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles(
        "Data/Shaders/lit" );
    matDouble->m_diffuse = g_renderer->CreateOrGetTexture( "Data/Images/Couch/example_colour.png" );
    matDouble->m_normal = g_renderer->CreateOrGetTexture( "Data/Images/Couch/example_normal.png" );


    GameObject* quad = new GameObject();
    quad->SetRenderable( Renderable::MakeQuad() );
    quad->GetRenderable()->SetMaterial( 0, mat );
    quad->GetTransform().TranslateLocal( Vec3::FORWARD * 10 );
    g_renderSceneGraph->AddGameObject( quad );

    GameObject* cube = new GameObject();
    cube->SetRenderable( Renderable::MakeCube() );
    cube->GetRenderable()->SetMaterial( 0, mat );
    cube->GetTransform().TranslateLocal( Vec3( 4, 0, 10 ) );
    g_renderSceneGraph->AddGameObject( cube );


    GameObject* sphere = new GameObject();
    sphere->SetRenderable( Renderable::MakeUVSphere() );
    sphere->GetRenderable()->SetMaterial( 0, mat );
    sphere->GetTransform().TranslateLocal( Vec3( -4, 0, 10 ) );
    g_renderSceneGraph->AddGameObject( sphere );

    GameObject* cylinder = new GameObject();
    Renderable* renderable = new Renderable();
    SurfacePatch_Cylinder spCylinder{};
    renderable->m_mesh = MeshBuilder::FromSurfacePatch( &spCylinder ).MakeMesh();
    cylinder->SetRenderable( renderable );
    cylinder->GetRenderable()->SetMaterial( 0, mat );
    cylinder->GetTransform().TranslateLocal( Vec3( 0, 2, 10 ) );
    g_renderSceneGraph->AddGameObject( cylinder );


    GameObject* cone = new GameObject();
    renderable = new Renderable();
    SurfacePatch_Cone spCone{};
    renderable->m_mesh = MeshBuilder::FromSurfacePatch( &spCone ).MakeMesh();
    cone->SetRenderable( renderable );
    cone->GetRenderable()->SetMaterial( 0, mat );
    cone->GetTransform().TranslateLocal( Vec3( -4, 2, 10 ) );
    g_renderSceneGraph->AddGameObject( cone );

    GameObject* torus = new GameObject();
    renderable = new Renderable();
    SurfacePatch_Torus spTorus{};
    renderable->m_mesh = MeshBuilder::FromSurfacePatch( &spTorus ).MakeMesh();
    torus->SetRenderable( renderable );
    torus->GetRenderable()->SetMaterial( 0, mat );
    torus->GetTransform().TranslateLocal( Vec3( -8, 2, 10 ) );
    g_renderSceneGraph->AddGameObject( torus );

    GameObject* helicoid = new GameObject();
    renderable = new Renderable();
    SurfacePatch_Helicoid spHelicoid( 1 );
    renderable->m_mesh = MeshBuilder::FromSurfacePatch( &spHelicoid ).MakeMesh();
    helicoid->SetRenderable( renderable );
    helicoid->GetRenderable()->SetMaterial( 0, matDouble );
    helicoid->GetTransform().TranslateLocal( Vec3( 4, 2, 10 ) );
    g_renderSceneGraph->AddGameObject( helicoid );
}


void GameState_Playing::TestEnter()
{
    MakePrimitiveShapes();
    g_console->Print( "Current Dir:" );
    g_console->Print( IOUtils::GetCurrentDir() );






}

void GameState_Playing::TestRender() const
{
}

void GameState_Playing::TestUpdate()
{


}

void GameState_Playing::TestInput()
{
    static std::vector<uint> handles;

    // Quad2D
    if( g_input->WasKeyJustPressed( 'Z' ) )
    {
        for( int Index = 0; Index < 10; ++Index )
        {
            DebugRender::SetOptions( 1, Rgba::RED, Rgba::BLUE );
            Vec2 dim = g_window->GetDimensions();
            Vec2 randPoint = Random::Vec2InRange( Vec2::ZEROS, dim );
            AABB2 bounds = AABB2::NEG_ONES_ONES * 100;
            bounds.Translate( randPoint );
            uint handle = DebugRender::DrawQuad2D(
                bounds, g_renderer->GetTexture( TEST_TEXTURE ) );
            handles.push_back( handle );
        }
    }

    // Clear
    if( g_input->WasKeyJustPressed( 'X' ) )
    {
        for( auto& handle : handles )
        {
            DebugRender::KillTask( handle );
        }
        handles.clear();
    }

    // Line2D
    if( g_input->WasKeyJustPressed( 'Z' ) )
    {
        for( int Index = 0; Index < 100; ++Index )
        {
            DebugRender::SetOptions( 1, Rgba::WHITE, Rgba::RED );
            Vec2 dim = g_window->GetDimensions();
            Vec2 randPoint0 = Random::Vec2InRange( Vec2::ZEROS, dim );
            Vec2 randPoint1 = Random::Vec2InRange( Vec2::ZEROS, dim );

            uint handle = DebugRender::DrawLine2D( randPoint0, randPoint1,
                                                   Rgba::YELLOW, Rgba::GREEN );
            handles.push_back( handle );
        }
    }

    // Text2D
    if( g_input->WasKeyJustPressed( 'Z' ) )
    {
        for( int Index = 0; Index < 100; ++Index )
        {
            DebugRender::SetOptions( 1, Rgba::WHITE, Rgba::RED );
            Vec2 dim = g_window->GetDimensions();
            Vec2 randPoint = Random::Vec2InRange( Vec2::ZEROS, dim );
            AABB2 bounds = AABB2::NEG_ONES_ONES * 100;
            bounds.Translate( randPoint );

            uint handle = DebugRender::DrawText2D( bounds, 15, Vec2::POINT_FIVES,
                                                   "hello %d", 100 );
            handles.push_back( handle );
        }
    }

    // Logf
    if( g_input->WasKeyJustPressed( 'C' ) )
    {
        for( int Index = 0; Index < 10; ++Index )
        {
            DebugRender::SetOptions( 10, Rgba::GREEN, Rgba::RED );
            uint handle = DebugRender::Logf( "hi %d", Random::IntInRange( 0, 100 ) );
            handles.push_back( handle );
        }
    }

    // Random lines 3D
    if( g_input->WasKeyJustPressed( 'V' ) )
    {
        for( int Index = 0; Index < 100; ++Index )
        {
            DebugRender::SetOptions( 2, Rgba::GREEN, Rgba::RED );
            Vec3 randPoint0 = Random::Vec3InRange( Vec3::NEG_ONES, Vec3::ONES );
            Vec3 randPoint1 = Random::Vec3InRange( Vec3::NEG_ONES, Vec3::ONES );

            uint handle = DebugRender::DrawLine( randPoint0, randPoint1,
                                                 Rgba::YELLOW, Rgba::GREEN );
            handles.push_back( handle );
        }
    }

    const Vec3 camPos =  g_mainCamera->m_transform.GetWorldPosition();
    // Sphere
    if( g_input->WasKeyJustPressed( 'V' ) )
    {
        DebugRender::SetOptions( -1, Rgba::GREEN, Rgba::RED, DebugRender::Mode::USE_DEPTH,
                                 FillMode::WIRE );
        uint handle = DebugRender::DrawSphere( camPos + Vec3( 2, 0, 4 ) );
        handles.push_back( handle );

        DebugRender::SetOptions( -1, Rgba::RED );
        handle = DebugRender::DrawTag( AABB2::NEG_ONES_ONES, camPos + Vec3( 2, 2, 4 ),
                                       10, Vec2::POINT_FIVES, "Sphere Wireframe" );
        handles.push_back( handle );

        DebugRender::SetOptions( -1, Rgba::GREEN, Rgba::RED, DebugRender::Mode::USE_DEPTH,
                                 FillMode::SOLID );
        handle = DebugRender::DrawSphere( camPos + Vec3( 4, 0, 4 ) );
        handles.push_back( handle );

        DebugRender::SetOptions( -1, Rgba::RED );
        handle = DebugRender::DrawTag( AABB2::NEG_ONES_ONES, camPos + Vec3( 4, 2, 4 ),
                                       10, Vec2::POINT_FIVES, "Sphere" );
        handles.push_back( handle );
    }

    // AABB3
    if( g_input->WasKeyJustPressed( 'V' ) )
    {
        DebugRender::SetOptions( -1, Rgba::GREEN, Rgba::RED, DebugRender::Mode::USE_DEPTH,
                                 FillMode::WIRE );
        uint handle = DebugRender::DrawAABB3(
            AABB3( camPos + Vec3::FORWARD * 4, 1, 2, 3 ) );
        handles.push_back( handle );

        DebugRender::SetOptions( -1, Rgba::RED );
        handle = DebugRender::DrawGlyph(
            AABB2::NEG_ONES_ONES * 20, camPos + Vec3( 0, 2, 4 ),
            g_renderer->GetTexture( TEST_TEXTURE ) );
        handles.push_back( handle );
    }

    // Quad 3D
    if( g_input->WasKeyJustPressed( 'V' ) )
    {
        DebugRender::SetOptions( 2, Rgba::GREEN, Rgba::RED, DebugRender::Mode::USE_DEPTH,
                                 FillMode::WIRE );
        uint handle = DebugRender::DrawQuad(
            AABB2::NEG_ONES_ONES, camPos + Vec3( -2, 0, 4 ), Vec3( 20, 20, 20 ) );
        handles.push_back( handle );
    }

    // Text 3D
    if( g_input->WasKeyJustPressed( 'V' ) )
    {
        DebugRender::SetOptions( -1, Rgba::GREEN, Rgba::RED );
        Transform trans = g_mainCamera->m_transform;
        trans.TranslateLocal( Vec3( -5, 0, 4 ) );


        uint handle = DebugRender::DrawText(
            AABB2::NEG_ONES_ONES, trans.GetLocalToParent(), 1, Vec2::POINT_FIVES,
            "Hi there" );
        handles.push_back( handle );
    }

    // Grid
    if( g_input->WasKeyJustPressed( 'B' ) )
    {
        DebugRender::SetOptions( 10 );
        uint handle = DebugRender::DrawGrid( camPos, 10, 70 );
        handles.push_back( handle );
    }

    // Spotlight at camera
    if( g_input->WasKeyJustPressed( 'F' ) )
    {
        MoveLightToCamera( true );
    }

    // Directional light at camera
    if( g_input->WasKeyJustPressed( 'R' ) )
    {
        MoveLightToCamera( false );
    }

}

void GameState_Playing::MakeSpaceShip()
{
    m_shipHull = new GameObject();
    Renderable* spaceShipRenderable = new Renderable();
    //     MeshBuilder mb = ObjLoader::LoadFromFile(
    //         "Data/Model/scifi_fighter_mk6/scifi_fighter_mk6.obj" );
    spaceShipRenderable->m_mesh = Mesh::CreateOrGetMesh(
        "Data/Model/scifi_fighter_mk6/scifi_fighter_mk6.obj", false, true );
    Material* mat = new Material();
    mat->m_specularAmount = 0.7f;
    mat->m_specularPower = 30.f;
    mat->m_diffuse = g_renderer->CreateOrGetTexture(
        "Data/Model/scifi_fighter_mk6/SciFi_Fighter-MK6-diffuse.jpg" );
    mat->m_normal = g_renderer->CreateOrGetTexture(
        "Data/Model/scifi_fighter_mk6/ship_normal.png" );
    mat->m_shaderPass = new ShaderPass();
    mat->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/lit" );
    m_shipHull->SetRenderable( spaceShipRenderable );
    m_shipHull->GetRenderable()->SetMaterial( 0, mat );
    //m_gameObjects.push_back( m_shipHull );
    g_renderSceneGraph->AddGameObject( m_shipHull );

    // Add spotlight to front of ship
    Light* light = new Light();
    light->m_sourceRadius = 10;
    light->m_coneInnerDot = CosDeg( 0 );
    light->m_coneOuterDot = CosDeg( 10 );
    light->m_intensity = 4;
    light->GetTransform().SetWorldPosition( Vec3( 0, 3, -4 ) );
    light->SetParent( m_shipHull );
    g_renderSceneGraph->AddLight( light );

    // Add exhaust trail
    m_rightThrustEmitter = MakeExhaustParticles( Vec3( 3, 0, -3 ) );
    m_leftThrustEmitter = MakeExhaustParticles( Vec3( -3, 0, -3 ) );

}

void GameState_Playing::LoadMiku()
{
    m_miku = new GameObject();
    Renderable* mikuRenderable = new Renderable();
    mikuRenderable->m_mesh = Mesh::CreateOrGetMesh(
        "Data/Model/snow_miku/snow_miku.obj", true, true );

    Material* floor = new Material();
    floor->m_diffuse = g_renderer->CreateOrGetTexture(
        "Data/Model/snow_miku/floor.png" );
    floor->m_shaderPass = ShaderPass::GetAdditiveShader();
    //floor->m_shaderPass = new ShaderPass();
    //floor->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/lit" );
    mikuRenderable->SetMaterial( 0, floor );

    Material* matBody = new Material();
    matBody->m_diffuse = g_renderer->CreateOrGetTexture(
        "Data/Model/snow_miku/body.png" );
    matBody->m_shaderPass = new ShaderPass();
    matBody->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/lit" );
    mikuRenderable->SetMaterial( 1, matBody );
    mikuRenderable->SetMaterial( 3, matBody );

    Material* snowFlake = new Material();
    snowFlake->m_diffuse = g_renderer->CreateOrGetTexture(
        "Data/Model/snow_miku/snow_flake.png" );
    snowFlake->m_shaderPass = new ShaderPass();
    snowFlake->m_shaderPass->m_program = ShaderProgram::CreateOrGetFromFiles( "Data/Shaders/lit" );
    mikuRenderable->SetMaterial( 2, snowFlake );


    m_miku->SetRenderable( mikuRenderable );
    m_miku->GetTransform().TranslateLocal( Vec3::DOWN * 5 + Vec3::LEFT * 15 );
    g_renderSceneGraph->AddGameObject( m_miku );
}

void GameState_Playing::MakeLights()
{
    for( uint lightIdx = 0; lightIdx < m_lightsToSpawn; ++lightIdx )
    {
        Light* light = new Light();
        Rgba colors[8] ={
            Rgba::RED,
            Rgba::ORANGE,
            Rgba::YELLOW,
            Rgba::GREEN,
            Rgba::CYAN,
            Rgba::BLUE,
            Rgba::PURPLE,
            Rgba::WHITE
        };

        light->m_color = colors[lightIdx % MAX_LIGHTS];
        light->m_intensity = m_lightIntensity;
        light->m_sourceRadius = m_lightSourceRadius;

        Renderable* lightRenderable = new Renderable();
        lightRenderable->SetMesh(
            MeshPrimitive::MakeUVSphere( 0.5, 5, 10, light->m_color ).MakeMesh() );
        Material* mat = lightRenderable->GetMaterial( 0 );
        mat->m_shaderPass = new ShaderPass();
        mat->m_diffuse = Texture::GetWhiteTexture();
        mat->m_shaderPass->SetFillMode( FillMode::WIRE );
        light->SetRenderable( lightRenderable );
        light->GetTransform().SetLocalScale( m_lightSourceRadius * 2 );
        light->GetTransform().TranslateLocal( Vec3::UP * 20 );

        m_lights.push_back( light );
        g_renderSceneGraph->AddLight( light );
        g_renderSceneGraph->AddGameObject( light );
    }
}

void GameState_Playing::UpdateLights()
{
    for( uint lightIdx = 0; lightIdx < m_lightsToSpawn - 1; ++lightIdx )
    {
        float angleOffset = (float) lightIdx * 360 / ( (float) m_lightsToSpawn - 1 );
        float elevation = 3 * SinDeg( g_gameClock->GetTimeSinceStartupF() * 180 );
        float azimuth = 20 * g_gameClock->GetTimeSinceStartupF() + angleOffset;
        Vec3 pos = SphericalToCartesian( 20, elevation, azimuth );
        pos += Vec3::DOWN * 5;
        pos += Vec3::RIGHT * 10;
        m_lights[lightIdx]->GetTransform().SetWorldPosition( pos );
    }
}


void GameState_Playing::RenderLights() const
{
    for( auto& light : m_lights )
        g_renderer->DrawRenderable( light->GetRenderable() );
}

void GameState_Playing::MoveLightToCamera( bool isSpotLight )
{
    Light* light = m_lights[m_lightsToSpawn - 1];
    light->GetTransform().SetLocalToParent( g_mainCamera->GetCamToWorldMatrix() );
    if( isSpotLight )
    {
        light->m_sourceRadius = 5.f;
        light->m_isPointLight = 1;
        light->m_coneInnerDot = CosDeg( 0 );
        light->m_coneOuterDot = CosDeg( 20 );
        light->m_intensity = 17;
    }
    else
    {
        light->m_sourceRadius = 99999.f;
        light->m_isPointLight = 0;
        light->m_coneInnerDot = -2;
        light->m_coneOuterDot = -2;
        light->m_intensity = 1;
    }

}


void GameState_Playing::SetLightPos( const Vec3& pos )
{
    m_lightPos = pos;
    DebugRender::Task* task = DebugRender::GetTask( m_lightDebugHandle );
    if( !task )
        return;
    task->m_renderable->m_modelMatrix = Mat4::MakeTranslation( m_lightPos );
    g_renderer->SetLight( 0, Vec4( 1, 1, 1, m_lightIntensity ), m_lightPos, m_lightSourceRadius );
}

void GameState_Playing::SetAmbient( float ambient )
{
    g_renderer->SetAmbient( Vec4( 1, 1, 1, ambient ) );
}

void GameState_Playing::MakeProjectile( const Vec3& offset )
{
    Projectile* proj = new Projectile();
    Transform& trans = proj->GetTransform();
    Transform& shipTrans = m_shipHull->GetTransform();

    float yaw = Random::FloatInRange( -1, 1 );
    float pitch = Random::FloatInRange( -1, 1 );

    Mat4 rot = Mat4::MakeRotationEuler( yaw, pitch, 0 );

    proj->m_velocity = rot * ( shipTrans.GetForward() * m_projectileVelocity );
    proj->m_drag = 0;
    trans.SetLocalToParent( shipTrans.GetLocalToParent() );
    trans.SetWorldPosition( shipTrans.GetWorldPosition() );
    trans.TranslateLocal( offset );
    //trans.RotateLocalEuler( Vec3( pitch, yaw, 0 ));
    //trans.SetLocalToParent( rot * trans.GetLocalToParent() );
    g_renderSceneGraph->AddGameObject( proj );
    m_projectiles.push_back( proj );
}

void GameState_Playing::MakeAsteroids()
{
    MakeAsteroid( 5, Vec3( 10, -20, 40 ) );
    MakeAsteroid( 10, Vec3( 10, 20, 40 ) );
    MakeAsteroid( 8, Vec3( 20, 50, 40 ) );
}



void GameState_Playing::MakeAsteroid( float maxHealth, const Vec3& position )
{
    Asteroid* asteroid = new Asteroid( maxHealth );
    asteroid->GetTransform().SetWorldPosition( position );
    asteroid->m_velocity = Random::Vec3InRange( Vec3( -10, -10, -10 ), Vec3( 10, 10, 10 ) );
    asteroid->m_drag = 1;
    m_asteroids.push_back( asteroid );
    g_renderSceneGraph->AddGameObject( asteroid );

}

void GameState_Playing::CheckCollisions()
{
    for( auto& projectile : m_projectiles )
    {
        Vec3 projectilePos = projectile->GetTransform().GetWorldPosition();
        for( auto& asteroid : m_asteroids )
        {
            Vec3 asteroidPos = asteroid->GetTransform().GetWorldPosition();
            float dist = ( projectilePos - asteroidPos ).GetLength();
            if( dist < asteroid->m_maxHealth )
            {
                projectile->SetShouldDie( true );
                asteroid->HitWithProjectile( projectile->m_velocity );
                MakeCollisionParticles( projectilePos );
            }
        }
    }
}

// collision particles spawn callback
namespace
{
void CollisionParticleSpawnCB( Particle& particle )
{
    particle.m_velocity = Random::Vec3InRange( Vec3( -10, -10, -10 ), Vec3( 10, 10, 10 ) );
    particle.m_startColor = Random::ColorInRange( Rgba::YELLOW, Rgba( 200, 100, 20, 255 ) );
    particle.m_endColor = Random::ColorInRange( Rgba::RED, Rgba( 255, 100, 20, 255 ) );
    particle.m_drag = Random::FloatInRange( 2, 5 );
    particle.m_startSize = 0.4f;
    particle.m_endSize = 0;
    particle.m_maxAge = Random::FloatInRange( 1, 3 );
}
}

void GameState_Playing::MakeCollisionParticles( const Vec3& position )
{
    ParticleEmitter* emitter = new ParticleEmitter( g_gameClock );
    emitter->GetTransform().SetWorldPosition( position );
    emitter->SetSpawnRate( 0 );
    emitter->SetParticleSpawnCB( CollisionParticleSpawnCB );
    emitter->Burst( 40, 70 );
    m_emitters.push_back( emitter );
    g_renderSceneGraph->AddGameObject( emitter );
}

// exhaust particles spawn callback
namespace
{
void ExhaustParticleSpawnCB( Particle& particle )
{
    particle.m_startColor = Random::ColorInRange( Rgba::BLUE, Rgba( 100, 100, 200, 200 ) );
    particle.m_endColor = Random::ColorInRange( Rgba::RED, Rgba::ORANGE );
    particle.m_startSize = 1.f;
    particle.m_endSize = 0;
    particle.m_maxAge = Random::FloatInRange( 1, 3 );
}
}
ParticleEmitter* GameState_Playing::MakeExhaustParticles( const Vec3& offset )
{
    ParticleEmitter* emitter = new ParticleEmitter( g_gameClock );
    emitter->SetSpawnRate( 0.001f );
    emitter->SetParticleSpawnCB( ExhaustParticleSpawnCB );
    emitter->SetParentWorldSpace( m_shipHull );
    emitter->m_spawnPointTransform.SetLocalPosition( offset );
    m_emitters.push_back( emitter );
    g_renderSceneGraph->AddGameObject( emitter );
    return emitter;
}

void GameState_Playing::SetExhaustSpawnRate( float rate )
{
    m_leftThrustEmitter->SetSpawnRate( rate );
    m_rightThrustEmitter->SetSpawnRate( rate );
}

void GameState_Playing::MakeSkyBox()
{
    GameObject* skybox = new GameObject();
    Renderable* renderable = Renderable::MakeCube();
    Texture* cubemap = new CubeMap( "Data/Images/Skybox/SkyAndSea.jpg" );
    Material* mat = new Material();
    mat->SetDiffuse( cubemap );
    mat->SetShaderPass( 0, ShaderPass::GetSkyboxShader() );
    renderable->SetMaterial( 0, mat );
    skybox->SetRenderable( renderable );
    g_renderSceneGraph->AddGameObject( skybox );
}

void GameState_Playing::CreateNoiseImage()
{
    float scale = 50;
    uint numOctaves = 2;

    m_noiseImage = new Image( m_noiseImageSize, m_noiseImageSize );

    for( int posX = 0; posX < m_noiseImageSize; ++posX )
    {
        for( int posY = 0; posY < m_noiseImageSize; ++posY )
        {
            float redVal = Noise::Compute2dPerlin( (float) posX, (float) posY, scale, numOctaves ) + 1;
            Rgba color = Rgba::BLACK;
            color.r = (uchar) ( redVal / 2 * 255 );
            m_noiseImage->SetTexel( posX, posY, color );
        }
    }

    m_noiseTexture = new Texture( m_noiseImage );

    DebugRender::SetOptions( 100, Rgba::WHITE, Rgba::WHITE );
    DebugRender::DrawQuad( AABB2::NEG_ONES_ONES * 2, Vec3::ZEROS, Vec3::ZEROS, m_noiseTexture );

}

void GameState_Playing::AttatchCameraToShip()
{
    Transform& camTrans = g_mainCamera->GetTransform();
    camTrans.SetParent( &m_shipHull->GetTransform() );
    camTrans.SetLocalPosition( Vec3( 0, 5, -15 ) );
    camTrans.LookAt( Vec3( 0, 0, 1000 ) );
}
