#pragma once

#include "Engine/Core/EngineCommon.hpp"

struct sockaddr_storage;
struct sockaddr;

struct NetAddress
{
public:

    static NetAddress GetLocal();

    NetAddress();
    ~NetAddress(){};

    NetAddress( const sockaddr* addr);
    NetAddress( const String& string );

    bool ToSockAddr( sockaddr_storage *out, size_t *outAddrLen );
    bool FromSockAddr( const sockaddr* addr );

    String ToString() const;

public:

    uint m_ip4Address;
    uint16_t m_port;




};
