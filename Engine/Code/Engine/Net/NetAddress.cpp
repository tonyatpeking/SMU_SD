#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"

NetAddress NetAddress::GetLocal()
{
    return NetAddress{};
}

NetAddress::NetAddress()
{

}

NetAddress::NetAddress( const sockaddr* addr )
{

}

NetAddress::NetAddress( const String& string )
{

}

bool NetAddress::ToSockAddr( sockaddr_storage *out, size_t *outAddrLen )
{
    return false;;
}

bool NetAddress::FromSockAddr( const sockaddr* addr )
{
    if( addr->sa_family != AF_INET )
    {
        LOG_WARNING( "Only Supports IPV4" );
        return false;
    }

    sockaddr_in const *ipv4 = (sockaddr_in const*) addr;
    uint ip = ipv4->sin_addr.S_un.S_addr;
    uint port = ::ntohs( ipv4->sin_port );

    m_ip4Address = ip;
    m_port = port;

    return true;
}

String NetAddress::ToString() const
{
    return "";
}
