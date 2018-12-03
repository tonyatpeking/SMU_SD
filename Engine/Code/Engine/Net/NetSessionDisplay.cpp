
#include "Engine/Net/NetCommonC.hpp"
#include "Engine/Net/NetSessionDisplay.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/TextRenderable.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Core/RuntimeVars.hpp"
#include "Engine/Time/Clock.hpp"

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
    float boundsWidth = 1100;
    float boundsHeight = 300;
    float boundsBorder = 10;
    m_bounds.maxs = m_bounds.GetMinXMaxY() + Vec2( boundsWidth + boundsBorder, -boundsBorder );
    m_bounds.mins = m_bounds.GetMinXMaxY() + Vec2( boundsBorder, -boundsHeight - boundsBorder );

    m_titleText = new TextRenderable( "Session Info" );
    m_titleText->m_fontHeight = m_bounds.GetHeight() / 18;
    m_titleText->m_boundingBox = m_bounds;
    m_titleText->m_alignment = Vec2( 0, 1 );
    m_titleText->Finalize();

    m_infoText = new TextRenderable( "" );
    m_infoText->m_fontHeight = m_bounds.GetHeight() / 24;
    AABB2 infoTextBounds = m_bounds;
    infoTextBounds.Translate( Vec2( 0, -m_titleText->m_fontHeight - 5 ) );
    m_infoText->m_boundingBox = infoTextBounds;
    m_infoText->m_alignment = Vec2( 0, 1 );
    m_infoText->Finalize();

    RuntimeVars::SetVar( DISPLAY_NET_INFO, true );
}

NetSessionDisplay::~NetSessionDisplay()
{

}

void NetSessionDisplay::Render()
{
    Console* console = Console::GetDefault();
    if( !console->IsActive() )
        return;

    if( !RuntimeVars::IsBoolSet( DISPLAY_NET_INFO ) )
        return;

    NetSession* session = NetSession::GetDefault();

    if( session->m_state == eSessionState::DISCONNECTED )
        return;

    string infoStr = Stringf(
        "sim lag: %0.2fs-%0.2fs  sim loss: %0.2f%%  net clock: %0.2fs \n  My Addr:\n    %s \n  Connections:\n"
        "%-1s %-3s %-22s %-5s %-5s %-4s %-4s %-4s %-4s %-16s\n",
        session->m_minSimLatency, session->m_maxSimLatency,
        session->m_simLossAmount * 100,
        session->GetNetClock()->GetTimeSinceStartupF(),
        session->m_packetChannel->m_socket->m_address.ToStringAll().c_str(),
        "-", "idx", "addr", "rtt/s", "loss%", "lrcv", "lsnt", "oAck", "iAck", "rcvBits"
    );

    for ( auto& pair : session->m_connections )
    {
        char indicator = ' ';
        if( pair.first == session->m_myConnectionIdx )
            indicator = 'L';
        NetConnection* connection = pair.second;
        string connectionStr = Stringf(
            "%-1c %-3d %-22s %-5.2f %-5.1f %-4.2f %-4.2f %-4d %-4d %-16s\n",
            indicator,
            pair.first,
            connection->m_address.ToStringAll().c_str(),
            connection->m_roundTripTime,
            connection->CalculateLossRate() * 100.f,
            TimeUtils::GetCurrentTimeSecondsF() - connection->m_timeOfLastReceive,
            TimeUtils::GetCurrentTimeSecondsF() - connection->m_timeOfLastSend,
            connection->m_nextAckToSend,
            connection->m_receivedAcksCount,
            StringUtils::ToBitfield( connection->m_receivedAckBitfield ).c_str()
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
