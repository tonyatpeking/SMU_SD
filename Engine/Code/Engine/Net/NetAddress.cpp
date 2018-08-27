#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Logger.hpp"



NetAddress NetAddress::GetLocal( const String& service )
{
    std::vector<NetAddress> addresses = GetAllLocal( service, 1 );
    if( addresses.size() > 0 )
        return addresses[0];

    return NetAddress{};
}


std::vector<NetAddress> NetAddress::GetAllLocal( const String& service, int maxCount /*= INT_MAX */ )
{
    return GetAllForHost( "", service, maxCount );
}

NetAddress NetAddress::GetAddrForHost( const String& hostName, const String& service )
{
    std::vector<NetAddress> addrs = GetAllForHost( hostName, service, 1 );
    if( addrs.size() == 0 )
    {
        LOG_WARNING( "Could not find host: " + hostName + ":" + service );
        return NetAddress{};
    }
    return addrs[0];
}

std::vector<NetAddress> NetAddress::GetAllForHost(
    const String& addr,
    const String& service, int maxCount )
{

    std::vector<NetAddress> addresses;
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;


    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if( addr.empty() )
        hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    status = ::getaddrinfo( addr.c_str(), service.c_str(), &hints, &servinfo );
    if( status != 0 )
    {
        Logger::GetDefault()->LogPrintf(
            "net, Failed to find addresses for \\[%s:%s]. Error\\[%s]",
            addr.c_str(), service.c_str(), ::gai_strerror( status ) );
        return addresses;
    }


    addrinfo *iter = servinfo;
    int count = 0;
    while( iter != nullptr && count < maxCount )
    {
        if( iter->ai_family == AF_INET )
        {
            addresses.push_back( NetAddress( (sockaddr*) iter->ai_addr ) );
            ++count;
        }
        iter = iter->ai_next;
    }

    freeaddrinfo( servinfo ); // free the linked-list
    return addresses;
}

NetAddress::NetAddress()
{

}

NetAddress::NetAddress( const sockaddr* addr )
{
    this->FromSockAddr( addr );
}

NetAddress::NetAddress( const String& addrWithService )
{
    size_t colonPos = addrWithService.find_last_of( ":" );
    if( colonPos == String::npos )
    {
        LOG_WARNING( "missing :service in " + addrWithService );
        return;
    }
    String hostStr = addrWithService.substr( 0, colonPos );
    String serviceStr = addrWithService.substr( colonPos + 1 );

    *this = GetAddrForHost( hostStr, serviceStr );
}

NetAddress::NetAddress( const String& addr, const String& service )
{
    *this = GetAddrForHost( addr, service );
}

bool NetAddress::ToSockAddr( sockaddr *out, size_t *outAddrLen ) const
{
    *outAddrLen = sizeof( sockaddr_in );
    memset( out, 0, *outAddrLen );
    out->sa_family = AF_INET;
    sockaddr_in *ipv4 = (sockaddr_in *) out;
    ipv4->sin_addr.S_un.S_addr = m_ip4Address;
    ipv4->sin_port = ::htons( (USHORT) m_port );
    return true;
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

bool NetAddress::IsEmpty()
{
    return m_ip4Address == 0 && m_port == 0;
}

String NetAddress::ToStringAll() const
{
    return ToStringIP() + ":" + ToStringPort();
}

String NetAddress::ToStringIP() const
{
    char cstr[INET_ADDRSTRLEN];
    sockaddr_in sa;
    size_t len;
    ToSockAddr( (sockaddr*) &sa, &len );
    inet_ntop( sa.sin_family, &( sa.sin_addr ), cstr, INET_ADDRSTRLEN );
    String str( cstr );
    return str;
}

String NetAddress::ToStringPort() const
{
    return ::ToString( m_port );
}

