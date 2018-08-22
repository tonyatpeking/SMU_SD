

// winsock libraries
#pragma comment(lib, "ws2_32.lib")

struct sockaddr;

class Net
{
public:
    static bool Startup();
    static bool Shutdown();
    static void ConnectToForseth();
    static bool GetAddressForHost( sockaddr *out, int *out_addrlen, char const *hostname,
                                   char const *service = "12345" );
};