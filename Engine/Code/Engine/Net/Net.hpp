#pragma once

// winsock libraries
#pragma comment(lib, "ws2_32.lib")

#include "Engine/Core/EngineCommonH.hpp"

struct sockaddr;
struct NetAddress;
class TCPSocket;

class Net
{
public:
    static bool Startup();
    static bool Shutdown();

    static void ConnectionTest( const NetAddress& addr, const String& msg );

    static void HostTest(int port);
    static void HostTestClose();
    static void HostTestServerThread( const NetAddress& addr );
    static void HostTestServiceThread( TCPSocket* client );

    static bool GetAddressForHost( sockaddr *out, int *out_addrlen, char const *hostname,
                                   char const *service = "12345" );


};