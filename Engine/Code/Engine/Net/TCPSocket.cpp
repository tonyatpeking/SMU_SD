#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/StringUtils.hpp"

TCPSocket::~TCPSocket()
{
    if( !IsClosed() )
        Close();
}

bool TCPSocket::Listen( uint port, uint maxQueued )
{
    NetAddress localAddr = NetAddress::GetLocal( ToString( port ) );
    if( localAddr.IsEmpty() )
    {
        Logger::GetDefault()->LogPrintf( "Could not find local address" );
        return false;
    }
    m_sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    sockaddr_storage saddr;
    size_t addrlen;
    localAddr.ToSockAddr( (sockaddr*) &saddr, &addrlen );

    size_t result = ::bind( (SOCKET) m_sock, (sockaddr*) &saddr, (int)addrlen );
    if( result == SOCKET_ERROR )
    {
        // failed to bind - if you want to know why, call WSAGetLastError()
        Close();
        return false;
    }

    result = ::listen( (SOCKET) m_sock, maxQueued );
    if( result == SOCKET_ERROR )
    {
        Close();
        return false;
    }
    Logger::GetDefault()->LogPrintf( "Listening at, %s", localAddr.ToStringAll().c_str() );

    m_isClosed = false;
    return true;
}

TCPSocket* TCPSocket::Accept()
{
    sockaddr_storage theirAddr;
    socklen_t theirAddrlen = sizeof(sockaddr_storage);
    SOCKET theirSock = ::accept( (SOCKET)m_sock, (sockaddr*) &theirAddr, &theirAddrlen );
    if( theirSock == INVALID_SOCKET )
    {
        int err = WSAGetLastError();
        return nullptr;
    }

    TCPSocket* theirTCPSock = new TCPSocket();
    theirTCPSock->m_sock = theirSock;
    theirTCPSock->m_address = NetAddress( (sockaddr*) &theirAddr );
    theirTCPSock->m_isClosed = false;
    return theirTCPSock;
}

bool TCPSocket::Connect( const NetAddress& addr )
{
    m_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if( INVALID_SOCKET == m_sock )
    {
        Logger::GetDefault()->LogPrintf( "Could not create socket" );
    }

    sockaddr_storage saddr;
    size_t addrlen;
    addr.ToSockAddr( (sockaddr*) &saddr, &addrlen );

    int result = ::connect( (SOCKET) m_sock, (sockaddr*) &saddr, (int) addrlen );
    if( result == SOCKET_ERROR )
    {
        Logger::GetDefault()->LogPrintf( "Could not connect" );
        Close();
        return false;
    }

    Logger::GetDefault()->LogPrintf( "Connected" );
    m_isClosed = false;
    return true;
}

void TCPSocket::Close()
{
    ::closesocket( (SOCKET) m_sock );
    m_isClosed = true;
}

size_t TCPSocket::Send( void const *data, int dataByteSize )
{
    size_t sent = ::send( (SOCKET) m_sock, (const char*) data, dataByteSize, 0 );
    return sent;
}

size_t TCPSocket::Receive( void *buffer, int maxByteSize )
{
    size_t recvd = ::recv( (SOCKET) m_sock, (char*) buffer, maxByteSize, 0 );
    return recvd;
}

bool TCPSocket::IsClosed() const
{
    return m_isClosed;
}
