
// add this to WindowsCommon.h
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>


#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Net/Net.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Logger.hpp"


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

void Net::ConnectToForseth()
{

}
