#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Profiler.hpp"

#include "Game/GameState_Loading.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameMap.hpp"

GameState_Loading::GameState_Loading()
    : GameState( GameStateType::LOADING )
{

}

GameState_Loading::~GameState_Loading()
{

}

void GameState_Loading::Update()
{
    GameState::Update();
    LoadResourcesOnSecondFrame();
}

void GameState_Loading::Render() const
{
    GameState::Render();

    g_renderer->UseUICamera();
    g_renderer->DrawAABB( AABB2::MakeBoundsFromDimensions( g_window->GetDimensions() ), g_config->overlayColor );
    Vec2 screenCenter = (Vec2) g_window->GetDimensions() * 0.5f;
    float textHeight = 70.f;
    g_renderer->DrawText2D( "LOADING...", screenCenter, textHeight );

}

void GameState_Loading::OnEnter()
{
    GameState::OnEnter();
    g_game->ChangeState( GameStateType::MAIN_MENU, 1.f );
}

void GameState_Loading::OnExit()
{
    GameState::OnExit();
}

void GameState_Loading::ProcessInput()
{
    GameState::ProcessInput();
}

void GameState_Loading::LoadResourcesOnSecondFrame()
{
    static int frame = 0;
    ++frame;
    if( frame != 2 )
    {
        return;
    }

    PROFILE_LOG_SCOPE_FUNCTION();

    {
        PROFILE_LOG_SCOPE( MakeSpaceShip );
        MakeSpaceShip();
    }
    {
        PROFILE_LOG_SCOPE( MakeSkyBox );
        MakeSkyBox();
    }
    {
        PROFILE_LOG_SCOPE( MakeNoiseImage );
        MakeNoiseImage();
    }
    {
        PROFILE_LOG_SCOPE( MakeMap );
        MakeMap();
    }
    {
        PROFILE_LOG_SCOPE( LoadAudio );
        LoadAudio();
    }
}

void GameState_Loading::MakeSpaceShip()
{
    g_game->m_shipHull = new GameObject();
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
    g_game->m_shipHull->SetRenderable( spaceShipRenderable );
    g_game->m_shipHull->GetRenderable()->SetMaterial( 0, mat );

}


void GameState_Loading::MakeSkyBox()
{
    GameObject* skybox = new GameObject();
    Renderable* renderable = Renderable::MakeCube();
    Texture* cubemap = new CubeMap( "Data/Images/Skybox/SkyAndSea.jpg" );
    Material* mat = new Material();
    mat->SetDiffuse( cubemap );
    mat->SetShaderPass( 0, ShaderPass::GetSkyboxShader() );
    renderable->SetMaterial( 0, mat );
    skybox->SetRenderable( renderable );
}

void GameState_Loading::MakeNoiseImage()
{
    float scale = 20.f;
    uint numOctaves = 2;

    g_game->m_noiseImage = new Image( g_game->m_noiseImageSize, g_game->m_noiseImageSize );

    for( int posX = 0; posX < g_game->m_noiseImageSize; ++posX )
    {
        for( int posY = 0; posY < g_game->m_noiseImageSize; ++posY )
        {
            float redVal = Noise::Compute2dPerlin( (float) posX, (float) posY, scale, numOctaves ) + 1;
            Rgba color = Rgba::BLACK;
            color.r = (uchar) ( redVal / 2 * 255 );
            g_game->m_noiseImage->SetTexel( posX, posY, color );
        }
    }

    g_game->m_noiseTexture = new Texture( g_game->m_noiseImage );

    //DebugRender::SetOptions( 100, Rgba::WHITE, Rgba::WHITE );
    //DebugRender::DrawQuad( AABB2::NEG_ONES_ONES * 2, Vec3::ZEROS, Vec3::ZEROS, g_game->m_noiseTexture );

}

void GameState_Loading::LoadAudio()
{
    g_audio->LoadAudioGroups( "Data/Audio/Audio.xml" );
}

void GameState_Loading::MakeMap()
{
    g_game->m_map = new GameMap();;
    g_game->m_map->LoadFromImage( g_game->m_noiseImage, AABB2( -128, -128, 128, 128 ),
                                  0, 20, IVec2( 16, 16 ) );
}