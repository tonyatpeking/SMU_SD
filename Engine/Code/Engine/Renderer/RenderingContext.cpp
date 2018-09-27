
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/RenderingContext.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/Window.hpp"


RenderingContext::RenderingContext( Window* window )
    : m_window(window)
{
    Init();
}

RenderingContext::~RenderingContext()
{
    Cleanup();
}

bool RenderingContext::Init()
{
    GLFunctions::LoadGLLibrary();
    // Get the Device Context (DC) - how windows handles the interface to rendering devices
    // This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call.
    HDC hdc = ::GetDC( (HWND) m_window->GetHandle() );

    // use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
    // This should be very similar to SD1
    HGLRC tempContext = CreateOldRenderContext( hdc );

    ::wglMakeCurrent( hdc, tempContext );
    // find the functions we'll need to create the real context;
    GLFunctions::BindNewWGLFunctions();

    // create the real context, using opengl version 4.2
    HGLRC realContext = CreateRealRenderContext( hdc, 4, 2 );

    // Set and cleanup
    ::wglMakeCurrent( hdc, realContext );
    ::wglDeleteContext( tempContext );

    // Finalize all our OpenGL functions we'll be using.
    GLFunctions::BindGLFunctions();

    // set the globals
    m_GLContext = realContext;

    // Disable / Enable vsync
    wglSwapIntervalEXT( 0 );

    return true;
}


HGLRC RenderingContext::CreateOldRenderContext( HDC hdc )
{
    // Setup the output to be able to render how we want
    // (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
    // and is double buffered
    PIXELFORMATDESCRIPTOR pfd;
    memset( &pfd, 0, sizeof( pfd ) );
    pfd.nSize = sizeof( pfd );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
    pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
    pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN
    // Find a pixel format that matches our search criteria above.
    int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
    if( pixel_format == NULL )
    {
        ERROR_RECOVERABLE( "pixel_format == NULL" );
        return NULL;
    }
    // Set our HDC to have this output.
    if( !::SetPixelFormat( hdc, pixel_format, &pfd ) )
    {
        ERROR_RECOVERABLE( "!::SetPixelFormat( hdc, pixel_format, &pfd )" );
        return NULL;
    }
    // Create the context for the HDC
    HGLRC context = wglCreateContext( hdc );
    if( context == NULL )
    {
        ERROR_RECOVERABLE( "context == NULL" );
        return NULL;
    }
    // return the context;
    return context;
}


HGLRC RenderingContext::CreateRealRenderContext( HDC hdc, int major, int minor )
{
    // So similar to creating the temp one - we want to define
    // the style of surface we want to draw to.  But now, to support
    // extensions, it takes key_value pairs
    int const format_attribs[] ={
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
        WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
                                            // WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
                                            // WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
                                            NULL, NULL,                         // Tell it we're done.
    };

    // Given the above criteria, we're going to search for formats
    // our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
    size_t const MAX_PIXEL_FORMATS = 128;
    int formats[MAX_PIXEL_FORMATS];
    int pixel_format = 0;
    UINT format_count = 0;

    BOOL succeeded = wglChoosePixelFormatARB( hdc,
                                              format_attribs,
                                              nullptr,
                                              MAX_PIXEL_FORMATS,
                                              formats,
                                              (UINT*) &format_count );

    if( !succeeded )
    {
        return NULL;
    }

    // Loop through returned formats, till we find one that works
    for( unsigned int i = 0; i < format_count; ++i )
    {
        pixel_format = formats[i];
        succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context;
        if( succeeded )
        {
            break;
        }
        else
        {
            DWORD error = GetLastError();
            UNUSED( error );
            //LOG( ( string( "Failed to set the format: " ) + StringUtils::ToString( (int) error ) ) );
        }
    }

    if( !succeeded )
    {
        return NULL;
    }
    // Okay, HDC is setup to the right format, now create our GL context

    // First, options for creating a debug context (potentially slower, but
    // driver may report more useful errors).
    int context_flags = 0;
#if defined(_DEBUG)
    context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

    // describe the context
    int const attribs[] ={
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
        WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
        0, 0
    };

    // Try to create context
    HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
    if( context == NULL )
    {
        return NULL;
    }

    return context;
}

void RenderingContext::Cleanup()
{
    HWND hwnd = (HWND) m_window->GetHandle();
    HDC hdc = ::GetDC( hwnd );
    wglMakeCurrent( hdc, NULL );

    ::wglDeleteContext( m_GLContext );
    ::ReleaseDC( hwnd, hdc );

    m_GLContext = NULL;
}
