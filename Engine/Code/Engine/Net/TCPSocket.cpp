#include "Engine/Core/WindowsUtils.hpp"
#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Log/Logger.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/DataUtils/EndianUtils.hpp"
#include "Engine/DataUtils/BytePacker.hpp"

namespace
{
const size_t MAX_STR_LEN = 0xFFFF;

char* GetStrBuffer()
{
    static char* s_strBuffer = nullptr;

    if( s_strBuffer == nullptr )
        s_strBuffer = (char*) malloc( MAX_STR_LEN );

    return s_strBuffer;
}
}


TCPSocket::TCPSocket()
{
    m_sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( INVALID_SOCKET == m_sock )
    {
        LOG_WARNING_TAG( "Net", "Could not create socket" );
    }
    else
    {
        m_isClosed = false;
    }
}

TCPSocket::~TCPSocket()
{
    free( m_receiveBuffer );
}

bool TCPSocket::Listen( const NetAddress& localAddr, uint maxQueued )
{
    if( localAddr.IsInvalid() )
    {
        LOG_WARNING_TAG( "Net", "Listen() failed. Could not find local address" );
        Close();
        return false;
    }

    sockaddr_storage saddr;
    size_t addrlen;
    localAddr.ToSockAddr( (sockaddr*) &saddr, &addrlen );

    size_t result = ::bind( (SOCKET) m_sock, (sockaddr*) &saddr, (int) addrlen );
    if( IsFatalError( result ) )
    {
        LOG_WARNING_TAG( "Net", "failed to bind socket" );
        Close();
        return false;
    }

    result = ::listen( (SOCKET) m_sock, maxQueued );
    if( IsFatalError( result ) )
    {
        LOG_WARNING_TAG( "Net", "failed to listen" );
        Close();
        return false;
    }
    LOG_INFO_TAG( "Net", "Listening at, %s", localAddr.ToStringAll().c_str() );
    m_address = localAddr;
    m_isClosed = false;
    return true;
}

TCPSocket* TCPSocket::Accept()
{
    sockaddr_storage theirAddr;
    socklen_t theirAddrlen = sizeof( sockaddr_storage );
    SOCKET theirSock = ::accept( (SOCKET) m_sock, (sockaddr*) &theirAddr, &theirAddrlen );
    if( theirSock == INVALID_SOCKET )
    {
        if( HasFatalError() )
        {
            LOG_INFO_TAG( "Net", "Could not accept" );
        }
        return nullptr;
    }

    TCPSocket* theirTCPSock = new TCPSocket( theirSock );
    theirTCPSock->m_address = NetAddress( (sockaddr*) &theirAddr );
    theirTCPSock->m_isClosed = false;
    return theirTCPSock;
}

bool TCPSocket::Connect( const NetAddress& addr )
{
    sockaddr_storage saddr;
    size_t addrlen;
    addr.ToSockAddr( (sockaddr*) &saddr, &addrlen );

    int result = ::connect( (SOCKET) m_sock, (sockaddr*) &saddr, (int) addrlen );
    if( IsFatalError( result ) )
    {
        LOG_INFO_TAG( "Net", "Could not connect" );
        Close();
        return false;
    }
    m_address = addr;
    LOG_INFO_TAG( "Net", "Connected" );
    return true;
}

size_t TCPSocket::Send( void const *data, int dataByteSize )
{
    size_t sent = ::send( (SOCKET) m_sock, (const char*) data, dataByteSize, 0 );
    if( IsFatalError( sent ) )
    {
        LOG_INFO_TAG( "Net", "Could not Send" );
        Close();
    }
    return sent;
}

size_t TCPSocket::Receive( void *buffer, int maxByteSize )
{
    if( IsClosed() )
        return Socket::Error;
    size_t recvd = ::recv( (SOCKET) m_sock, (char*) buffer, maxByteSize, 0 );
    if( IsFatalError( recvd ) )
    {
        LOG_INFO_TAG( "Net", "Could not Receive" );
        Close();
    }
    if( recvd == 0 )
    {
        Close();
    }
    return recvd;
}

void TCPSocket::SetBufferSize( size_t bufferSize )
{
    if( m_bufferSize == bufferSize )
        return;
    m_bufferSize = bufferSize;
    MakeBuffer();
}

void TCPSocket::MakeBuffer()
{
    free( m_receiveBuffer );
    m_receiveBuffer = (Byte*) malloc( m_bufferSize );
}

size_t TCPSocket::BufferIncomming( size_t bufferUpTo )
{
    if( m_receiveBuffer == nullptr )
        MakeBuffer();

    if( m_bufferWriteHead >= bufferUpTo )
        return m_bufferWriteHead;

    if( bufferUpTo > m_bufferSize )
    {
        LOG_WARNING_TAG( "Net", "m_receiveBuffer too small" );
        return 0;
    }

    size_t bytesNeeded = bufferUpTo - m_bufferWriteHead;

    size_t byteCountRead = Receive( m_receiveBuffer + m_bufferWriteHead, (int) bytesNeeded );

    // return if no data or disconnect
    if( byteCountRead == SOCKET_ERROR || byteCountRead == 0 )
    {
        return byteCountRead;
    }

    m_bufferWriteHead += byteCountRead;
    return m_bufferWriteHead;
}

size_t TCPSocket::ReceiveExact( void *buffer, size_t exactByteSize )
{
    size_t bufferedByteCount = BufferIncomming( exactByteSize );
    if( bufferedByteCount == SOCKET_ERROR || bufferedByteCount == 0 )
        return bufferedByteCount;

    if( bufferedByteCount > exactByteSize )
    {
        LOG_ERROR_TAG( "Net", "bufferedByteCount > exactByteSize something went wrong" );
        return 0;
    }

    if( bufferedByteCount == exactByteSize )
    {
        memcpy( buffer, m_receiveBuffer, exactByteSize );
        m_bufferWriteHead = 0;
        return exactByteSize;
    }

    return (size_t) SOCKET_ERROR;
}


size_t TCPSocket::ReceiveMessage( bool& out_isEcho, string& out_msg )
{
    if( m_msgSize == 0 )
    {
        // Get msgSize
        unsigned short tempMsgSize = 0;
        size_t bytesCount = ReceiveExact( &tempMsgSize, 2 );
        if( bytesCount == 2 )
        {
            m_msgSize = tempMsgSize;
            EndianUtils::FromEndianness( &m_msgSize, Endianness::BIG );
        }
        return (size_t) SOCKET_ERROR; // keep on waiting
    }

    else
    {
        // Get message
        size_t bytesCount = BufferIncomming( m_msgSize );
        if( bytesCount == m_msgSize )
        {
            BytePacker packer( m_msgSize, m_receiveBuffer );
            packer.SetWriteHead( m_msgSize );

            packer.Read( &out_isEcho );
            char* strBuffer = GetStrBuffer();
            packer.ReadString( strBuffer, MAX_STR_LEN );
            out_msg = string( strBuffer );

            m_msgSize = 0;
            m_bufferWriteHead = 0;
            return bytesCount;
        }
        return (size_t) SOCKET_ERROR; // keep on waiting
    }
}

