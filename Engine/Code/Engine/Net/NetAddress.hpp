#pragma once

#include "Engine/Net/NetCommonH.hpp"
#include <vector>

struct sockaddr_storage;
struct sockaddr;

struct NetAddress
{
public:

    static bool ShouldIgnoreAddr( const NetAddress& addr );

    static NetAddress GetLocal( const string& service );

    static vector<NetAddress> GetAllLocal(
        const string& service,
        int maxCount = INT_MAX );

    static NetAddress GetAddrForHost( const string& addr,
                                      const string& service );
    static vector<NetAddress> GetAllForHost(
        const string& addr,
        const string& service,
        int maxCount = INT_MAX );

    NetAddress() {};
    ~NetAddress() {};
    NetAddress( const sockaddr* addr );
    NetAddress( const string& addrWithService );
    NetAddress( const string& addr, const string& service );

    bool operator==( const NetAddress& compare ) const;
    bool operator!=( const NetAddress& compare ) const;

    bool ToSockAddr( sockaddr *out, size_t *outAddrLen ) const;
    bool FromSockAddr( const sockaddr* addr );
    bool IsInvalid() const;
    static bool AreIPsSame( const NetAddress& addrA, const NetAddress& addrB );

    string ToStringAll() const;
    string ToStringIP() const;
    string ToStringPort() const;

public:

    uint m_ip4Address = INVALID_IPV4_ADDR;
    uint m_port = INVALID_PORT;

};
