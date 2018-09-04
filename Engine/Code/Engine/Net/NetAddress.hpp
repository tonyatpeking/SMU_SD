#pragma once

#include "Engine/Core/EngineCommonH.hpp"
#include <vector>

struct sockaddr_storage;
struct sockaddr;

struct NetAddress
{
public:

    static bool ShouldIgnoreAddr( const NetAddress& addr );

    static NetAddress GetLocal( const String& service );

    static std::vector<NetAddress> GetAllLocal(
        const String& service,
        int maxCount = INT_MAX );

    static NetAddress GetAddrForHost( const String& addr,
                                      const String& service );
    static std::vector<NetAddress> GetAllForHost(
        const String& addr,
        const String& service,
        int maxCount = INT_MAX );

    NetAddress();
    ~NetAddress() {};

    NetAddress( const sockaddr* addr );
    NetAddress( const String& addrWithService );
    NetAddress( const String& addr, const String& service );


    bool ToSockAddr( sockaddr *out, size_t *outAddrLen ) const;
    bool FromSockAddr( const sockaddr* addr );
    bool IsEmpty() const;
    static bool AreIPsSame( const NetAddress& addrA, const NetAddress& addrB );

    String ToStringAll() const;
    String ToStringIP() const;
    String ToStringPort() const;

public:

    uint m_ip4Address = 0;
    uint m_port = 0;




};
