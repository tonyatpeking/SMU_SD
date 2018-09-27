#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Log/Logger.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Thread/Thread.hpp"

#include <atomic>

bool Net::Startup()
{
    WORD version = MAKEWORD( 2, 2 );
    WSADATA data;

    int error = WSAStartup( version, &data );

    GUARANTEE_OR_DIE( error == 0, "WSAStartup failed" );
    return error == 0;

}

bool Net::Shutdown()
{
    ::WSACleanup();
    return true;
}

void Net::ConnectionTest( const NetAddress& addr, const string& msg )
{
    TCPSocket socket;
    if( socket.Connect( addr ) )
    {
        socket.Send( msg.c_str(), (int) msg.size() + 1 );

        char payload[256];
        size_t recvd = socket.Receive( payload, 256 );
        payload[recvd] = NULL;
        LOG_INFO( "Received: %s", payload );
        socket.Close();
    }
    else
    {
        LOG_INFO( "Could not connect" );
    }

}

// HostTest internal
namespace
{
std::atomic_bool s_shouldShutServer = false;
}

void Net::HostTest( int port )
{
    vector<NetAddress> addrs = NetAddress::GetAllLocal( ::ToString( port ) );

    for( auto& addr : addrs )
    {
        Thread::CreateAndDetach( HostTestServerThread, addr );
    }
}

void Net::HostTestClose()
{
    s_shouldShutServer = true;
}

void Net::HostTestServerThread( const NetAddress& addr )
{
    TCPSocket host;
    if( !host.Listen( addr ) )
        LOG_INFO( "Can not listen on %s", addr.ToStringAll().c_str() );

    while( !s_shouldShutServer )
    {
        TCPSocket *client = host.Accept();
        if( client != nullptr )
        {
            // this thread does all the work and destroys the socket;
            Thread::CreateAndDetach( HostTestServiceThread, client );
        }
    }
    s_shouldShutServer = false;
    host.Close();
}

void Net::HostTestServiceThread( TCPSocket* client )
{
    const int bufferSize = 256;
    char buffer[bufferSize];
    memset( buffer, 0, bufferSize );

    while( true )
    {
        int recvd = (int) client->Receive( buffer, bufferSize );
        if( recvd == 0 || recvd == SOCKET_ERROR )
            break;

        buffer[recvd] = NULL; // just cause I'm printing it

        LOG_INFO( "Received from %s: %s",
                  client->m_address.ToStringAll().c_str(),
                  buffer );

        string msg = "Tony's Shitty Server";
        client->Send( msg.c_str(), (int) msg.size() + 1 );
    }

    LOG_INFO( "%s closed connection",
                                     client->m_address.ToStringAll().c_str() );

    client->Close();
    delete client;

}


