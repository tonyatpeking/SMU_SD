// winsock libraries
#pragma comment(lib, "ws2_32.lib")

struct sockaddr;

class Net
{
public:
    static bool Startup();
    static bool Shutdown();
    static void ConnectDirectWithWinSock();


    static bool GetAddressForHost( sockaddr *out, int *out_addrlen, char const *hostname,
                                   char const *service = "12345" );

//     bool NetAddress::from_sockaddr( sockaddr const* addr )
//     {
//         if( addr->sa_family != AF_INET )
//         {
//             return;
//         }
//         // if ipv6 also check AF_INET6
//
//         sockaddr_in const *ipv4 = (sockaddr_in const*) addr;
//         uint ip = ipv4->sin_addr.S_un.S_addr;
//         uint port = ::ntohs( ipv4->sin_port );
//
//         m_address = ip;
//         m_port = port;
//
//         return ture;
//
//     }



};