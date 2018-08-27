// winsock libraries
#pragma comment(lib, "ws2_32.lib")

#include "Engine/Core/EngineCommon.hpp"

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
    static void HostTestServerThread( int port );
    static void HostTestServiceThread( TCPSocket* client );

    static void ConnectDirectWithWinSock();


    static bool GetAddressForHost( sockaddr *out, int *out_addrlen, char const *hostname,
                                   char const *service = "12345" );


};