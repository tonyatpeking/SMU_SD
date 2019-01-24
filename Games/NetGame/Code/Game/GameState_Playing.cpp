#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Math/Distance.hpp"
#include "Engine/UI/Menu.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IVec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/SurfacePatch.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Intersection.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Core/ContainerUtils.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Image/Image.hpp"
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
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Time/Tween.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/FileIO/ObjLoader.hpp"
#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Math/Raycast.hpp"
#include "Engine/Math/Ray2.hpp"
#include "Engine/Thread/Thread.hpp"
#include "Engine/ShapeGrammar/ShapeRulesetLoader.hpp"
#include "Engine/Thread/ThreadSafeQueue.hpp"
#include "Engine/Log/Logger.hpp"

#include "Game/NetCube.hpp"
#include "Engine/Net/NetSession.hpp"

#include "Game/GameState_Playing.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameNetMessages.hpp"
#include "Game/Player.hpp"


namespace
{
GameState_Playing* s_default = nullptr;

}


GameState_Playing* GameState_Playing::GetDefault()
{
    return  s_default;
}

GameState_Playing::GameState_Playing()
    : GameState( GameStateType::PLAYING )
{
    s_default = this;
}

GameState_Playing::~GameState_Playing()
{
    //delete g_mainCamera;
    s_default = nullptr;
}

void GameState_Playing::Update()
{
    PROFILER_SCOPED();
    g_input->ShowCursor( false );

    // switch phase before all updates, this is after process input
    GameState::Update();

    // Host is also a client
    ClientUpdate();

    if( IsHost() )
        HostUpdate();

    g_gameObjectManager->Update();

    g_gameObjectManager->DeleteDeadGameObjects();
}

void GameState_Playing::HostUpdate()
{
    RemoveDisconnectedPlayers();
    CheckForVictoryReset();
    UpdatePlayerInputs();
    SendCubeUpdatesForAllClients();
    UpdateBullets();
}

void GameState_Playing::ClientUpdate()
{
    SendInputsToHost();
}

void GameState_Playing::Render() const
{
    GameState::Render();
}

void GameState_Playing::OnEnter()
{
    GameState::OnEnter();

    m_session = NetSession::GetDefault();

    g_input->LockCursor( true );
    g_input->ClipCursor( true );
    g_input->ShowCursor( false );

    MakeCamera();

    DebugRender::Set3DCamera( g_renderer->GetMainCamera() );

    g_renderer->SetBackGroundColor( Rgba::BLACK );

    MakeSun();

    SetAmbient( 0.3f );

    // Tell the host we have started playing
    SendEnterGame();
}

void GameState_Playing::OnExit()
{
    GameState::OnExit();
}

void GameState_Playing::ProcessInput()
{
    PROFILER_SCOPED();
    GameState::ProcessInput();

    if( g_input->WasKeyJustPressed( InputSystem::KEYBOARD_ESCAPE ) )
        g_app->OnQuitRequested();

    ProcessMovementInput();
}


void GameState_Playing::Process_EnterGame( uint8 playerID )
{
    // tell new player to create all cubes
    map<uint16, NetCube*> allCubes = NetCube::GetAllCubes();
    for( auto& pair : allCubes )
    {
        SendCreateCube( playerID, pair.second );
    }
    CreatePlayerCube( playerID );
}

void GameState_Playing::SendCreateCube( uint8 playerID, NetCube* cube )
{
    if( playerID == 0 )
        return;
    m_session->SendToConnection(
        playerID, GameNetMessages::Compose_CreateCube( cube ) );
}

void GameState_Playing::SendDestroyCube( uint16 netID )
{
    m_session->SendToAllButMe( GameNetMessages::Compose_DestroyCube( netID ) );
}

void GameState_Playing::SendCreateCubeToAll( NetCube* cube )
{
    m_session->SendToAllButMe(
        GameNetMessages::Compose_CreateCube( cube ) );
}

void GameState_Playing::CreatePlayerCube( uint8 playerID )
{
    Rgba color = Random::Default()->ColorWheel();
    uint16 netID = NetCube::GetNextFreeNetID();
    NetCube* cube = new NetCube(
        Vec3::ZEROS, Vec3::ZEROS, Vec3::ONES, color, netID );
    cube->m_factionID = playerID;
    Player* player = new Player();
    player->m_id = playerID;
    player->m_cube = cube;
    m_players[playerID] = player;

    SendCreateCubeToAll( cube );
}

void GameState_Playing::SendCubeUpdatesForAllClients()
{
    for( auto& pair : m_players )
    {
        NetCube* cube = pair.second->m_cube;
        if( cube )
            m_session->SendToAllButMe(
                GameNetMessages::Compose_UpdateCube( cube ) );
    }
}


void GameState_Playing::CreateBulletForPlayer( uint8 playerID )
{
    Player* player = GetPlayer( playerID );
    NetCube* playerCube = GetPlayerCube( playerID );
    if( playerCube == nullptr )
        return;
    if( !player->PopShootTimer() )
        return;
    Rgba color = playerCube->GetColor();
    color = Random::Default()->ColorInRange( color, 30 );
    uint16 netID = NetCube::GetNextFreeNetID();
    NetCube* bullet = new NetCube(
        playerCube->GetTransform().GetLocalPosition(),
        Vec3::ZEROS, Vec3( 0.3f, 0.3f, 0.3f ), color, netID );
    bullet->m_velocity = playerCube->m_direction;
    bullet->m_factionID = playerCube->m_factionID;
    m_bullets.push_back( bullet );

    SendCreateCubeToAll( bullet );
}

void GameState_Playing::RemoveDisconnectedPlayers()
{
    for( auto it = m_players.cbegin(); it != m_players.cend(); )
    {
        uint8 playerID  = it->first;
        if( m_session->GetConnection( playerID ) == nullptr )
        {
            SendDestroyCube( playerID );
            delete it->second;
            it = m_players.erase( it );
        }
        else
        {
            ++it;
        }
    }
}

void GameState_Playing::CheckForVictoryReset()
{
    if( m_players.size() == 1 )
        return;

    float r = 0;
    float g = 0;
    float b = 0;

    for( auto& pair : m_players )
    {
        NetCube* playerCube = pair.second->m_cube;
        Rgba color = playerCube->GetColor();
        r += (float) color.r;
        g += (float) color.g;
        b += (float) color.b;
    }
    r = r / (float) m_players.size();
    g = g / (float) m_players.size();
    b = b / (float) m_players.size();

    for( auto& pair : m_players )
    {
        NetCube* playerCube = pair.second->m_cube;
        Rgba color = playerCube->GetColor();
        if( fabsf( (float) color.r - r ) > VICTORY_COLOR_DEVIATION
            || fabsf( (float) color.g - g ) > VICTORY_COLOR_DEVIATION
            || fabsf( (float) color.b - b ) > VICTORY_COLOR_DEVIATION )
        {
            return;
        }
    }

    // Victory
    for( auto& pair : m_players )
    {
        NetCube* playerCube = pair.second->m_cube;
        playerCube->SetTargetColor( Random::Default()->ColorWheel() );
    }
}

NetCube* GameState_Playing::GetPlayerCube( uint8 playerID )
{
    Player* player = GetPlayer( playerID );
    if( player )
        return player->m_cube;
    return nullptr;
}

Player* GameState_Playing::GetPlayer( uint8 playerID )
{
    if( ContainerUtils::ContainsKey( m_players, playerID ) )
        return m_players[playerID];
    return nullptr;
}

void GameState_Playing::Process_SendInputs(
    uint8 playerID, const ClientInputs& inputs )
{
    Player* player = GetPlayer( playerID );
    if( player )
        *player->m_inputs = inputs;
}

void GameState_Playing::UpdatePlayerInputs()
{
    for( auto& pair : m_players )
    {
        uint8 playerID = pair.first;
        Player* player = pair.second;
        // movement
        NetCube* playerCube = player->m_cube;
        ClientInputs& input = *player->m_inputs;
        Transform& t = playerCube->GetTransform();
        Vec3 translate =
            input.up * Vec3::UP
            + input.left * Vec3::LEFT
            + input.down * Vec3::DOWN
            + input.right * Vec3::RIGHT;
        float length = translate.NormalizeAndGetLength();
        if( length > 0.00001 )
            playerCube->m_direction = translate;
        translate = translate
            * g_gameClock->GetDeltaSecondsF()
            * PLAYER_MOVE_SPEED;
        t.TranslateLocal( translate );
        playerCube->SetTargetPosition( t.GetLocalPosition() );

        // shooting
        if( input.fire )
        {
            CreateBulletForPlayer( playerID );
        }

    }
}

void GameState_Playing::UpdateBullets()
{
    for( int i = (int)m_bullets.size() - 1; i >= 0; --i )
    {
        NetCube* bullet = m_bullets[i];
        bullet->m_timeToLive -= g_gameClock->GetDeltaSecondsF();
        if( bullet->m_timeToLive <= 0.f )
        {
            SendDestroyCube( bullet->m_netID );
            bullet->SetShouldDie( true );
            ContainerUtils::EraseAtIndexFast( m_bullets, i );
            continue;
        }

        for( auto& pair : m_players )
        {
            Player* player = pair.second;
            if( player->m_cube->m_factionID == bullet->m_factionID )
                continue;
            Vec3 playerPos = player->m_cube->GetTransform().GetLocalPosition();
            Vec3 bulletPos = bullet->GetTransform().GetLocalPosition();
            if( ( playerPos - bulletPos ).GetLengthSquared()
                < PLAYER_BULLET_COLLISION_DIST_SQUARED )
            {
                Rgba playerColor = player->m_cube->GetColor();
                Rgba bulletColor = bullet->GetColor();
                Rgba blend = Lerp( playerColor, bulletColor,
                                   BULLET_COLOR_BLEND_WEIGHT );
                player->m_cube->SetTargetColor( blend );
                SendDestroyCube( bullet->m_netID );
                bullet->SetShouldDie( true );
                ContainerUtils::EraseAtIndexFast( m_bullets, i );
                break;
            }
        }

    }
}

void GameState_Playing::SendInputsToHost()
{
    ClientInputs inputs;
    inputs.up = g_input->IsKeyPressed( 'W' );
    inputs.left = g_input->IsKeyPressed( 'A' );
    inputs.down = g_input->IsKeyPressed( 'S' );
    inputs.right = g_input->IsKeyPressed( 'D' );
    inputs.fire = g_input->IsKeyPressed( InputSystem::KEYBOARD_SPACE );
    m_session->SendToHost( GameNetMessages::Compose_SendInputs( inputs ) );
}

void GameState_Playing::Process_CreateCube( const Vec3& position, const Vec3& velocity, const Vec3& scale, const Rgba& color, uint16 netID )
{
    NetCube* cube = new NetCube( position, Vec3::ZEROS, scale, color, netID );
    cube->m_velocity = velocity;
}

void GameState_Playing::SendEnterGame()
{
    m_session->SendToHost( GameNetMessages::Compose_EnterGame() );
}

void GameState_Playing::Process_DestroyCube( uint16 netID )
{
    NetCube::MarkForDestroy( netID );
}

void GameState_Playing::Process_UpdateCube(
    const Vec3& position,
    const Rgba& color,
    uint16 netID )
{
    NetCube* cube = NetCube::GetNetCube( netID );
    if( !cube )
        return;
    cube->SetTargetPosition( position );
    cube->SetTargetColor( color );
}

bool GameState_Playing::IsHost()
{
    return m_session->IsHost();
}

void GameState_Playing::MakeCamera()
{
    Camera* cam = new OrbitCamera();
    cam->SetColorTarget( g_renderer->DefaultColorTarget() );
    cam->SetDepthStencilTarget( g_renderer->DefaultDepthTarget() );
    cam->SetProjection( m_fov, m_near, m_far ); //fov near far
    g_renderer->SetMainCamera( cam );
    g_renderSceneGraph->AddCamera( cam );
}

void GameState_Playing::ProcessMovementInput()
{
    PROFILER_SCOPED();
    if( !g_input->WindowHasFocus() )
        return;

    float ds = g_gameClock->GetDeltaSecondsF();

    SendInputsToHost();

    float deltaCamYaw = 0;
    float deltaCamPitch = 0;


    //     Vec2 cursorDelta = g_input->GetCursorDelta();
    //     if( cursorDelta.GetLengthSquared() > 0.01f )
    //     {
    //         deltaCamYaw = -cursorDelta.x * m_cameraRotateSpeed;
    //         deltaCamPitch = cursorDelta.y * m_cameraRotateSpeed;
    //     }
    //     m_camYaw += deltaCamYaw;
    //     m_camPitch += deltaCamPitch;
    //     m_camPitch = Clampf( m_camPitch, -89, 89 );


    UpdateCameraToFollow();
}

void GameState_Playing::MakeSun()
{
    m_sun = new Light();

    m_sun->SetCastShadow( true );

    m_sun->m_color = Rgba::WHITE;
    m_sun->GetTransform().SetWorldEuler( Vec3( 45, 90, 0 ) );
    m_sun->GetTransform().SetWorldPosition( Vec3( 0, 30, 0 ) );

    m_sun->m_sourceRadius = 99999.f;
    m_sun->m_isPointLight = 0;
    m_sun->m_coneInnerDot = -2;
    m_sun->m_coneOuterDot = -2;
    m_sun->m_intensity = 1;

    DebugRender::SetOptions( 100 );
    DebugRender::DrawBasis( m_sun->GetTransform().GetLocalToWorld() );
}


void GameState_Playing::SetAmbient( float ambient )
{
    g_renderer->SetAmbient( Vec4( 1, 1, 1, ambient ) );
}


void GameState_Playing::UpdateCameraToFollow()
{
    float camHeight = 3;
    float camRadius = 20;
    Vec3 camOrbitPoint;
    if( true )
    {
        Vec3 shipPos = Vec3::ZEROS;
        camOrbitPoint = shipPos + Vec3::UP * camHeight;
    }
    else
    {
        camOrbitPoint = Vec3::ZEROS;
    }


    ( (OrbitCamera*) g_renderer->GetMainCamera() )->SetTarget( camOrbitPoint );
    ( (OrbitCamera*) g_renderer->GetMainCamera() )->SetSphericalCoord( camRadius, m_camPitch, m_camYaw );

}
