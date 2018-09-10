#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Image/Image.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshPrimitive.hpp"

#include "Game/GameState_Loading.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

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
    g_renderer->DrawText2D( "LOADING...", screenCenter + Vec2(-400, textHeight / 2), textHeight );

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
        PROFILE_LOG_SCOPE( MakeSkyBox );
        MakeSkyBox();
    }

}


void GameState_Loading::MakeSkyBox()
{
    GameObject* skybox = new GameObject();
    Renderable* renderable = Renderable::MakeCube();
    Texture* cubemap = new CubeMap( "Data/Images/Skybox/Galaxy.png" );
    Material* mat = new Material();
    mat->SetDiffuse( cubemap );
    mat->SetShaderPass( 0, ShaderPass::GetSkyboxShader() );
    renderable->SetMaterial( 0, mat );
    skybox->SetRenderable( renderable );
}
