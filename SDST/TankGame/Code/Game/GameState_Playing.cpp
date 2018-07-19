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
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Time/TweenSystem.hpp"
#include "Engine/Time/Tween.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/IO/ObjLoader.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Math/Raycast.hpp"
#include "Engine/Math/Ray2.hpp"
#include "Engine/Core/Thread.hpp"
#include "Engine/Core/ShapeRulesetLoader.hpp"

#include "Game/GameState_Playing.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"

#include "ThirdParty/pybind11/embed.h"
#include "ThirdParty/pybind11/operators.h"
#include "ThirdParty/pybind11/stl.h"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_EMBEDDED_MODULE( zzzGame, m ) {
    m.def( "SetRootGameObject", &GameState_Playing::SetRootGameObject );
}

GameObject* GameState_Playing::s_rootGameObject = nullptr;


GameState_Playing::GameState_Playing()
    : GameState( GameStateType::PLAYING )
{
}

GameState_Playing::~GameState_Playing()
{
    //delete g_mainCamera;
}

void GameState_Playing::Update()
{
    PROFILER_SCOPED();

    // switch phase before all updates, this is after process input
    GameState::Update();

    CheckForShipRuleChange();

    g_gameObjectManager->Update();

    g_gameObjectManager->DeleteDeadGameObjects();

}

void GameState_Playing::Render() const
{
    GameState::Render();
}



void GameState_Playing::OnEnter()
{
    GameState::OnEnter();

    g_input->LockCursor( true );
    g_input->ClipCursor( true );
    g_input->ShowCursor( false );

    MakeCamera();

    DebugRender::Set3DCamera( g_renderer->GetMainCamera() );

    g_renderer->SetBackGroundColor( Rgba::BLACK );

    MakeSun();

    SetAmbient( 0.3f );

    BuildShipFromTree();

    ShapeRulesetLoader::Init();

    ShapeRulesetLoader::Load( "example_ruleset" );
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



void GameState_Playing::SetRootGameObject( GameObject* go )
{
    s_rootGameObject = go;
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

    if( s_rootGameObject )
    {
        Transform& shipTransform = s_rootGameObject->GetTransform();
        float deltaShipRoll = 0;

        if( g_input->IsKeyPressed( 'Q' ) )
            deltaShipRoll = ds * m_rollSpeed;
        if( g_input->IsKeyPressed( 'E' ) )
            deltaShipRoll = -ds * m_rollSpeed;
        shipTransform.RotateLocalEuler( Vec3( 0, 0, deltaShipRoll ) );
    }

    float deltaCamYaw = 0;
    float deltaCamPitch = 0;



    Vec2 cursorDelta = g_input->GetCursorDelta();
    if( cursorDelta.GetLengthSquared() > 0.01f )
    {
        deltaCamYaw = -cursorDelta.x * m_cameraRotateSpeed;
        deltaCamPitch = cursorDelta.y * m_cameraRotateSpeed;
    }
    m_camYaw += deltaCamYaw;
    m_camPitch += deltaCamPitch;
    m_camPitch = Clampf( m_camPitch, -89, 89 );


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


void GameState_Playing::CheckForShipRuleChange()
{
    if( ShapeRulesetLoader::DidCurrentRuleChange() )
        ShapeRulesetLoader::Load( "example_ruleset" );
}

void GameState_Playing::BuildShipFromTree()
{
//     if( s_rootGameObject )
//     {
//         s_rootGameObject->GetRenderable()->DeleteMesh();
//         delete s_rootGameObject;
//         s_rootGameObject = nullptr;
//     }

    //     s_rootGameObject = new GameObject();
    //     Renderable* r = new Renderable();
    //     s_rootGameObject->SetRenderable( r );
    //     r->GetMaterial( 0 )->SetShaderPass( 0, ShaderPass::GetLitShader() );
    //     r->SetMesh( MeshPrimitive::MakeCube().MakeMesh() );
    //s_rootGameObject = GameObject::MakeCube();

}

void GameState_Playing::UpdateCameraToFollow()
{
    float camHeight = 3;
    float camRadius = 20;
    Vec3 camOrbitPoint;
    if( s_rootGameObject )
    {
        Transform& shipTrans = s_rootGameObject->GetTransform();

        Vec3 shipPos = shipTrans.GetWorldPosition();
        camOrbitPoint = shipPos + Vec3::UP * camHeight;
    }
    else
    {
        camOrbitPoint = Vec3::ZEROS;
    }


    ( (OrbitCamera*) g_renderer->GetMainCamera() )->SetTarget( camOrbitPoint );
    ( (OrbitCamera*) g_renderer->GetMainCamera() )->SetSphericalCoord( camRadius, m_camPitch, m_camYaw );

}
