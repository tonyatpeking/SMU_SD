#include <math.h>
#include <cassert>
#include <crtdbg.h>

#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/FileIO/Blackboard.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/RuntimeVars.hpp"

#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameplayDefines.hpp"

#include "Engine/Core/WindowsCommon.hpp"

bool WindowCallback( unsigned int wmMessageCode, size_t wParam, size_t lParam )
{
    unsigned char keyCode = (unsigned char) wParam;

    switch( wmMessageCode )
    {
    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
        g_app->OnQuitRequested();
        return true;
    case WM_KEYDOWN:
    {
        g_input->OnKeyPressedWithRepeat( keyCode );
        bool keyWasDown = lParam & ( 1 << 30 );
        if( !keyWasDown )
        {
            g_input->OnKeyPressed( keyCode );
        }
        return true;
    }
    case WM_KEYUP:
        g_input->OnKeyReleased( keyCode );
        return true;
    case WM_CHAR:
    {
        unsigned char asciiCode = (unsigned char) wParam;
        g_input->OnCharInput( asciiCode );
        return true;
    }
    case WM_ACTIVATE: //Called when a window gains or loses focus
    {
        bool isActive = ( WA_INACTIVE != LOWORD( wParam ) );
        g_input->SetWindowHasFocus( isActive );
        return true;
    }
    case WM_MOVE:
    case WM_SIZE:
        g_input->UpdateClipCursorAfterResize();
    case WM_LBUTTONDOWN:
        g_input->OnKeyPressed( InputSystem::MOUSE_LEFT );
        return true;
    case WM_RBUTTONDOWN:
        g_input->OnKeyPressed( InputSystem::MOUSE_RIGHT );
        return true;
    case WM_MBUTTONDOWN:
        g_input->OnKeyPressed( InputSystem::MOUSE_MIDDLE );
        return true;
    case WM_LBUTTONUP:
        g_input->OnKeyReleased( InputSystem::MOUSE_LEFT );
        return true;
    case WM_RBUTTONUP:
        g_input->OnKeyReleased( InputSystem::MOUSE_RIGHT );
        return true;
    case WM_MBUTTONUP:
        g_input->OnKeyReleased( InputSystem::MOUSE_MIDDLE );
        return true;

    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM( wParam );
        g_input->AccumMouseWheelDelta( zDelta );
        return true;
    }

    }
    return false;
}


//-----------------------------------------------------------------------------------------------
void Initialize()
{
    g_blackboard = new Blackboard();
    g_blackboard->PopulateFromFile( CONFIG_PATH );
    g_config = new Config();
    g_config->LoadConfigFromBlackboard();
    g_window = new Window( g_config->clientAspect );
    g_window->RegisterHandler( WindowCallback );
    g_app = new App();
    if( RuntimeVars::HasVar( TILE_WINDOW ) )
    {
        int tile_pos = 0;
        RuntimeVars::GetVar( TILE_WINDOW, tile_pos );
        g_window->Tile4by4( tile_pos );
    }
}

//-----------------------------------------------------------------------------------------------
void Shutdown()
{
    delete g_app;
    g_app = nullptr;
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
    UNUSED( applicationInstanceHandle );
    RuntimeVars::PopulateFromCommandLine( ADDITIONAL_COMMAND_LINE_ARGS );
    // these can overwrite the above
    RuntimeVars::PopulateFromCommandLine( commandLineString );
    Initialize();
    RuntimeVars::LogAll();

    while( !g_app->IsQuitting() )
    {
        Sleep( 2 ); // give at least 1 millisecond (out of 16 per frame) back so that other processes can run
        g_app->RunFrame();
    }
    Shutdown();
    return 0;
}


