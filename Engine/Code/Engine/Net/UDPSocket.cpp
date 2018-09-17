#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Core/WindowsCommon.hpp"


bool UDPSocket::Bind( const NetAddress &addr, uint16_t port_range /*= 0U*/ )
{
    // create the socket
    SOCKET my_socket = socket( AF_INET,	// IPv4 to send...
                               SOCK_DGRAM,							// ...Datagrams...
                               IPPROTO_UDP );						// ...using UDP.

    ASSERT_OR_DIE( my_socket != INVALID_SOCKET, "Invalid socket" );

    // TODO, try to bind all ports within the range.
    // Shown - just trying one;
    sockaddr_storage sock_addr;
    size_t sock_addr_len;
    addr.ToSockAddr( (sockaddr*) &sock_addr, &sock_addr_len );

    // try to bind - if it succeeds - great.  If not, try the next port in the range.
    int result = ::bind( my_socket, (sockaddr*) &sock_addr, (int) sock_addr_len );
    if( 0 == result )
    {
        m_sock = my_socket;
        m_address = addr;
        m_isClosed = false;
        return true;
    }

    return false;
}

size_t UDPSocket::SendTo( const NetAddress &addr, const void *data,
                          const size_t byte_count )
{
    //     if( IsClosed() )
    //         return false;

    sockaddr_storage saddr;
    size_t addr_len;
    addr.ToSockAddr( (sockaddr*) &saddr, &addr_len );

    SOCKET sock = (SOCKET) m_sock;

    int sent = sendto(
        sock,
        (const char*) data,
        (int) byte_count,
        0,
        (sockaddr*) &saddr,
        (int)addr_len );

    if( sent > 0 )
    {
        if( sent != byte_count )
            LOG_ERROR_TAG( "Net", "sent != byte_count" );
        return (size_t) sent;
    }
    // ASSERT( sent == byte_count );
    else
    {
        if( HasFatalError() )
            Close();
        return 0U;
    }


}

size_t UDPSocket::ReceiveFrom( NetAddress& out_addr,
                               void *buffer,
                               const size_t max_read_size )
{
    //     if( IsClosed() )
    //         return 0U;

    sockaddr_storage fromaddr;
    int addr_len = sizeof( sockaddr_storage );

    SOCKET sock = (SOCKET) m_sock;

    int recvd = recvfrom(
        sock,
        (char*) buffer,
        (int) max_read_size,
        0,
        (sockaddr*) &fromaddr,
        &addr_len
    );

    if( recvd > 0 )
    {
        out_addr.FromSockAddr( (sockaddr*) &fromaddr );
        return recvd;
    }

    else
    {
        if( HasFatalError() )
        {
            Close();
        }
        return 0;
    }
}
