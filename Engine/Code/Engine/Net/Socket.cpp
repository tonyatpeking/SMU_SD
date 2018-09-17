#include "Engine/Core/WindowsUtils.hpp"
#include "Engine/Net/Socket.hpp"
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


size_t Socket::Error = (size_t) -1;


Socket::Socket( unsigned __int64 socket )
{
    m_sock = socket;
    m_isClosed = false; // assume it is open until proven otherwise
}

Socket::~Socket()
{
    if( !IsClosed() )
        Close();
}

void Socket::SetBlocking( bool block )
{
    m_blocking = block;
    u_long nonBlocking = m_blocking ? 0 : 1;
    int result = ioctlsocket( m_sock, FIONBIO, &nonBlocking );
    if( result != NO_ERROR )
    {
        String errStr = WindowsUtils::ErrorCodeToString( result );
        LOG_WARNING_TAG(
            "Net",
            "ioctlsocket failed with error: %d, %s\n",
            result,
            errStr.c_str() );
    }
}

void Socket::Close()
{
    LOG_INFO_TAG( "Net", "Connection closed %s", m_address.ToStringAll().c_str() );
    ::closesocket( (SOCKET) m_sock );
    m_isClosed = true;
}

bool Socket::IsClosed() const
{
    return m_isClosed;
}



bool Socket::IsFatalError( size_t result ) const
{
    if( result == (size_t) -1 )
        return HasFatalError();
    return false;
}

bool Socket::HasFatalError() const
{
    int err = WSAGetLastError();
    AssertBreakpoint( err != 10038 );
    if( err == WSAEWOULDBLOCK || err == WSAEMSGSIZE || err == WSAECONNRESET )
        return false;
    String errStr = WindowsUtils::ErrorCodeToString( err );
    LOG_WARNING_TAG( "Net", "winsock error: %d, %s", err, errStr.c_str() );
    return true;
}

int Socket::GetLastErrorCode() const
{
    return WSAGetLastError();
}

String Socket::GetLastErrorString() const
{
    int errCode = GetLastErrorCode();
    return WindowsUtils::ErrorCodeToString( errCode );
}



