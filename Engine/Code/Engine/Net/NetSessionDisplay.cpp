#include "Engine/Net/NetSession.hpp"
#include "Engine/Net/NetSessionDisplay.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/TextRenderable.hpp"
#include "Engine/Net/PacketChannel.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/NetConnection.hpp"

NetSessionDisplay* NetSessionDisplay::GetDefault()
{
    static NetSessionDisplay* s_default = nullptr;;
    if( !s_default )
        s_default = new NetSessionDisplay();
    return s_default;
}

NetSessionDisplay::NetSessionDisplay()
{
    Renderer* renderer = Renderer::GetDefault();
    Window* window = renderer->GetWindow();
    AABB2 windowBounds = window->GetWindowBounds();
    m_bounds = windowBounds;
    m_bounds.maxs = m_bounds.GetMinXMaxY() + Vec2( 600, -10 );
    m_bounds.mins = m_bounds.GetMinXMaxY() + Vec2( 10, -200 );

    m_titleText = new TextRenderable( "Session Info" );
    m_titleText->m_fontHeight = m_bounds.GetHeight() / 13;
    m_titleText->m_boundingBox = m_bounds;
    m_titleText->m_alignment = Vec2( 0, 1 );
    m_titleText->Finalize();

    m_infoText = new TextRenderable( "" );
    m_infoText->m_fontHeight = m_bounds.GetHeight() / 15;
    AABB2 infoTextBounds = m_bounds;
    infoTextBounds.Translate( Vec2( 0, -m_titleText->m_fontHeight - 5 ) );
    m_infoText->m_boundingBox = infoTextBounds;
    m_infoText->m_alignment = Vec2( 0, 1 );
    m_infoText->Finalize();
}

NetSessionDisplay::~NetSessionDisplay()
{

}

void NetSessionDisplay::Render()
{
    Console* console = Console::GetDefault();
    if( !console->IsActive() )
        return;

    NetSession* session = NetSession::GetDefault();
    string infoStr = Stringf(
        "My Addr:\n  %s \nConnections:\n",
        session->m_packetChannel->m_socket->m_address.ToStringAll().c_str()
    );

    for ( auto& pair : session->m_connections )
    {
        char indicator = ' ';
        if( pair.first == session->m_myConnectionIdx )
            indicator = '*';

        string connectionStr = Stringf(
        " %c[%d]  [%s]\n",
            indicator,
            pair.first,
            pair.second->m_address.ToStringAll().c_str()
        );
        infoStr += connectionStr;
    }
    m_infoText->UpdateText( infoStr );

    Renderer* renderer = Renderer::GetDefault();

    renderer->UseUICamera();
    renderer->DrawAABB( m_bounds, Rgba( 0, 0, 35, 255 ) );
    renderer->DrawRenderable( m_titleText );
    renderer->DrawRenderable( m_infoText );
}
