#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Core/Thread.hpp"

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

void Net::ConnectionTest( const NetAddress& addr, const String& msg )
{
    TCPSocket socket;
    if( socket.Connect( addr ) )
    {
        socket.Send( msg.c_str(), (int) msg.size() + 1 );

        char payload[256];
        size_t recvd = socket.Receive( payload, 256 );
        payload[recvd] = NULL;
        Logger::GetDefault()->LogPrintf( "Received: %s", payload );
        socket.Close();
    }
    else
    {
        Logger::GetDefault()->LogPrintf( "Could not connect" );
    }

}

// HostTest internal
namespace
{
std::atomic_bool s_shouldShutServer = false;
}

void Net::HostTest( int port )
{
    Thread::CreateAndDetach( HostTestServerThread, port );
}

void Net::HostTestClose()
{
    s_shouldShutServer = true;
}

void Net::HostTestServerThread( int port )
{
    TCPSocket host;
    host.Listen( port );
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

    int recvd = (int) client->Receive( buffer, bufferSize );


    if( recvd != 0 )
    {
        buffer[recvd] = NULL; // just cause I'm printing it

        Logger::GetDefault()->LogPrintf( "Received: %s", buffer );

        String msg = "Tony's Shitty Server";
        client->Send( msg.c_str(), (int) msg.size() + 1 );
    }

    client->Close();
    delete client;

}

void Net::ConnectDirectWithWinSock()
{
    /*        String addr_str = args.get_next_string();*/
    String ip = "10.8.151.155";
    String port = "12345";

    //char const *msg = args.get_next_string();

    char const *msg = "123";

    //     if( StringIsNullOrEmpty( addr_str ) || StringIsNullOrEmpty( msg ) )
    //     {
    //         return;
    //     }

    /*    addr_str.split( &ip, &port, ':' );*/

    sockaddr_storage saddr;
    int addrlen;
    if( !GetAddressForHost( (sockaddr*) &saddr, &addrlen, ip.c_str(), port.c_str() ) )
    {
        return;
    }

    // first, create the socket - this allows us to setup options;
    SOCKET sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( sock == INVALID_SOCKET )
    {
        //ConsoleErrorf( "Could not create socket." );
        return;
    }

    // we then try to connect.  This will block until it either succeeds
    // or fails (which possibly requires a timeout).  We will talk about making
    // this non-blocking later so it doesn't hitch the game (more important
    // when connecting to remote hosts)
    int result = ::connect( sock, (sockaddr*) &saddr, (int) addrlen );
    if( result == SOCKET_ERROR )
    {
        //ConsoleErrorf( "Could not connect" );
        ::closesocket( sock );  // frees the socket resource;
        return;
    }

    //ConsoleInfof( "Connected." );

    // you send raw bytes over the network in whatever format you want;
    ::send( sock,

            R"(
Let's play a game.
)"

, 440000, 0 );

    // with TCP/IP, data sent together is not guaranteed to arrive together.
    // so make sure you check the return value.  This will return SOCKET_ERROR
    // if the host disconnected, or if we're non-blocking and no data is there.
    char payload[256];
    size_t recvd = ::recv( sock, payload, 256 - 1U, 0 );

    // it may not be null terminated and I'm printing it, so just for safety.
    payload[recvd] = NULL;
    //ConsoleInfof( "Recieved: %s", payload );

    // cleanup
    ::closesocket( sock );
}

bool Net::GetAddressForHost( sockaddr *out, int *out_addrlen, char const *hostname,
                             char const *service )
{


    //     if( SOCKET_ERROR == ::gethostname( hostname, 256 ) )
    //         return;


    if( &hostname == nullptr || hostname[0] == '\0' )
        return false;

    addrinfo hints;
    memset( &hints, 0, sizeof( hints ) );

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE;

    addrinfo *result = nullptr;
    int status = ::getaddrinfo( hostname, service, &hints, &result );
    if( status != 0 )
    {
        Logger::GetDefault()->LogPrintf(
            "net, Failed to find addresses for \\[%s:%s]. Error\\[%s]",
            hostname, service, ::gai_strerror( status ) );
    }

    bool found_one = false;
    addrinfo *iter = result;
    while( iter != nullptr )
    {
        if( iter->ai_family == AF_INET )
        {
            sockaddr_in* ipv4 = (sockaddr_in*) ( iter->ai_addr );
            memcpy( out, ipv4, sizeof( sockaddr_in ) );
            *out_addrlen = sizeof( sockaddr_in );
            found_one = true;
            break;
        }
        iter = iter->ai_next;

    }

    ::freeaddrinfo( result );

    return found_one;
}

