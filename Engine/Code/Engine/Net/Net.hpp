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

    static void ConnectionTest( const NetAddress& addr, const string& msg );

    static void HostTest(int port);
    static void HostTestClose();
    static void HostTestServerThread( const NetAddress& addr );
    static void HostTestServiceThread( TCPSocket* client );



};