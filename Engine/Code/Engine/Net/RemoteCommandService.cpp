#include "Engine/Net/RemoteCommandService.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/TextRenderable.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Time/Timer.hpp"
#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Math/Random.hpp"
#include "Engine/Log/Logger.hpp"
#include "Engine/Core/CommandSystem.hpp"
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/DataUtils/EndianUtils.hpp"

RemoteCommandService* RemoteCommandService::GetDefault()
{
    static RemoteCommandService* s_defaultRCS = nullptr;

    if( s_defaultRCS == nullptr )
        s_defaultRCS = new RemoteCommandService();

    return s_defaultRCS;
}

void RemoteCommandService::StartUp()
{
    m_hostAddress = GetLocalHostAddr();
}

void RemoteCommandService::ShutDown()
{
    Disconnect();
}

void RemoteCommandService::Update()
{
    switch( m_rcs_state )
    {
    case RCS_State::INITIAL:
        InitialState_Update();
        break;
    case RCS_State::CLIENT:
        ClientState_Update();
        break;
    case RCS_State::HOST:
        HostState_Update();
        break;
    case RCS_State::HOST_FAILED:
        HostFailedState_Update();
        break;
    }
}

void RemoteCommandService::RenderWidget()
{
    static TextRenderable* titleText = nullptr;
    static TextRenderable* infoText = nullptr;

    Console* console = Console::DefaultConsole();

    if( !console->IsActive() )
        return;

    Renderer* renderer = Renderer::GetDefault();
    Window* window = renderer->GetWindow();
    AABB2 windowBounds = window->GetWindowBounds();
    AABB2 bounds = windowBounds;
    bounds.maxs = bounds.maxs - Vec2( 10, 10 );
    bounds.mins = bounds.maxs - Vec2( 600, 200 );

    if( titleText == nullptr )
    {
        titleText = new TextRenderable( "" );
        titleText->m_fontHeight = bounds.GetHeight() / 13;
        titleText->m_boundingBox = bounds;
        titleText->m_alignment = Vec2( 0, 1 );
        titleText->Finalize();
    }

    if( infoText == nullptr )
    {
        infoText = new TextRenderable( "" );
        infoText->m_fontHeight = bounds.GetHeight() / 15;
        AABB2 infoTextBounds = bounds;
        infoTextBounds.Translate( Vec2( 0, -titleText->m_fontHeight - 5 ) );
        infoText->m_boundingBox = infoTextBounds;
        infoText->m_alignment = Vec2( 0, 1 );
        infoText->Finalize();
    }

    titleText->UpdateText( Stringf( "Remote Command Service [%s]",
                                    m_rcs_state.ToString().c_str() ) );

    if( m_rcs_state == RCS_State::CLIENT || m_rcs_state == RCS_State::HOST )
    {
        String infoString = Stringf(
            "Host Addr: \n  %s\n%d connections\n",
            m_hostAddress.ToStringAll().c_str(),
            m_connectedSockets.size()
        );

        for( int clientIdx = 0; clientIdx < m_connectedSockets.size(); ++clientIdx )
        {
            String clientString = Stringf(
                "  [%d] %s\n",
                clientIdx,
                m_connectedSockets[clientIdx]->m_address.ToStringAll().c_str()
            );

            infoString += clientString;
        }
        infoText->UpdateText( infoString );
    }


    renderer->UseUICamera();

    renderer->DrawAABB( bounds, Rgba( 0, 0, 35, 255 ) );

    renderer->DrawRenderable( titleText );

    renderer->DrawRenderable( infoText );

}

void RemoteCommandService::ShouldHost()
{
    m_shouldHost = true;
    m_shouldDisconnect = true;
}

void RemoteCommandService::ShouldJoin( const NetAddress& addr )
{
    m_shouldHost = false;
    m_hostAddress = addr;
    m_shouldDisconnect = true;
}

void RemoteCommandService::InitialState_Update()
{
    if( m_shouldHost ) // host
    {
        m_shouldHost = false;
        if( TryHost() )
        {
            SetState( RCS_State::HOST );
        }
        else
        {
            StartDelayTimer();
            SetState( RCS_State::HOST_FAILED );
        }
    }
    else // client
    {
        bool joinSuccess = TryJoin( m_hostAddress );

        if( joinSuccess )
        {
            SetState( RCS_State::CLIENT );
        }
        else
        {
            if( IsHostAddrLocal() ) // host if could not join local host
                m_shouldHost = true;
            else // it's a cruel world, come back to mommy
                m_hostAddress = GetLocalHostAddr();
        }
    }
}

void RemoteCommandService::HostState_Update()
{
    // ProcessReceives will NOT be called if m_shouldDisconnect is true
    if( m_shouldDisconnect )
    {
        Disconnect();
        SetState( RCS_State::INITIAL );
        return;
    }
    RemoveClosedConnections();
    ProcessReceives();
    ProcessAccepts();
}

void RemoteCommandService::ClientState_Update()
{
    // ProcessReceives will NOT be called if m_shouldDisconnect is true
    if( m_shouldDisconnect || m_connectedSockets.size() == 0 )
    {
        Disconnect();
        SetState( RCS_State::INITIAL );
        return;
    }
    RemoveClosedConnections();
    ProcessReceives();
}

void RemoteCommandService::HostFailedState_Update()
{
    if( IsDelayOver() )
        SetState( RCS_State::INITIAL );
}

void RemoteCommandService::SetState( RCS_State state )
{
    m_rcs_state = state;
}

bool RemoteCommandService::TryJoin( const NetAddress& addr )
{
    TCPSocket* socket = new TCPSocket();
    socket->SetBlocking( true );
    if( socket->Connect( addr ) )
    {
        socket->SetBlocking( false );
        m_connectedSockets.push_back( socket );
        return true;
    }
    delete socket;
    return false;
}

bool RemoteCommandService::TryHost()
{
    m_hostAddress = GetLocalHostAddr();
    m_hostListenSocket = new TCPSocket();
    if( !m_hostListenSocket->Listen( m_hostAddress ) )
    {
        delete m_hostListenSocket;
        return false;
    }
    m_hostListenSocket->SetBlocking( false );
    return true;
}

void RemoteCommandService::Disconnect()
{
    m_shouldDisconnect = false;
    delete m_hostListenSocket;
    m_hostListenSocket = nullptr;

    for( auto& connection : m_connectedSockets )
    {
        delete connection;
    }
    m_connectedSockets.clear();
}

void RemoteCommandService::StartDelayTimer()
{
    delete m_delayTimer;
    m_delayTimer = new Timer( Clock::GetRealTimeClock(), m_failDelay );
}

bool RemoteCommandService::IsDelayOver()
{
    return m_delayTimer->PeekOneLap();
}

bool RemoteCommandService::IsHostAddrLocal()
{
    NetAddress local = GetLocalHostAddr();
    return NetAddress::AreIPsSame( m_hostAddress, local );
}

NetAddress RemoteCommandService::GetLocalHostAddr()
{
    return NetAddress::GetLocal( m_service );
}

TCPSocket* RemoteCommandService::GetSocketByIndex( int idx )
{
    if( idx >= (int) m_connectedSockets.size() )
    {
        return nullptr;
    }
    else
    {
        return m_connectedSockets[idx];
    }
}

bool RemoteCommandService::SendMsg( int idx, bool isEcho, const char* str )
{
    TCPSocket* sockToSend = GetSocketByIndex( idx );
    if( sockToSend == nullptr || sockToSend->IsClosed() )
        return false;
    BytePacker packer( (Endianness)Endianness::BIG );
    packer.Write( isEcho );
    packer.WriteString( str );

    size_t len = packer.GetWrittenByteCount();

    GUARANTEE_OR_DIE( len <= 0xffff, "does not support msg size" );

    uint16_t uslen = (uint16_t) len;
    EndianUtils::ToEndianness( &uslen, Endianness::BIG );
    sockToSend->Send( &uslen, 2 );

    sockToSend->Send( packer.GetBuffer(), (int) len );
    return true;
}

void RemoteCommandService::ProcessReceives()
{
    for( auto& connection : m_connectedSockets )
    {
        ProcessReceive( connection );
    }
}

bool RemoteCommandService::ProcessReceive( TCPSocket* sock )
{
    bool out_isEcho;
    String out_msg;
    size_t byteCount = sock->ReceiveMessage( out_isEcho, out_msg );
    if( byteCount != 0 && byteCount != (size_t) -1 )
    {
        Logger::GetDefault()->LogPrintf( out_msg.c_str() );
        if( out_isEcho )
        {
            ;
        }
        else
        {
            CommandSystem::DefaultCommandSystem()->RunCommand( out_msg );
        }
        return true;
    }
    return false;
}

void RemoteCommandService::ProcessAccepts()
{
    bool acceptQueueEmpty = false;
    while( !acceptQueueEmpty )
    {
        TCPSocket* newClient = m_hostListenSocket->Accept();
        if( nullptr != newClient )
        {
            m_connectedSockets.push_back( newClient );
        }
        else
        {
            acceptQueueEmpty = true;
        }
    }
}

void RemoteCommandService::RemoveClosedConnections()
{
    for( int idx = (int) m_connectedSockets.size() - 1; idx >= 0; --idx )
    {
        if( m_connectedSockets[idx]->IsClosed() )
            ContainerUtils::EraseAtIndexFast( m_connectedSockets, idx );
    }
}
