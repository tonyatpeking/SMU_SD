#pragma once
#include "Engine/Net/NetAddress.hpp"

// net/socket.hpp
// this is just so it is a typed pointer - but
// internally we cast socket_t to SOCKET
class socket_type;
typedef socket_type* socket_t;



enum eSocketOptionBit : uint
{
    SOCKET_OPTION_BLOCKING = BIT_FLAG( 0 ),

    // some other options that may be useful to support
    // SOCKET_OPTION_BROADCAST - socket can broadcast messages (if supported by network)
    // SOCKET_OPTION_LINGER - wait on close to make sure all data is sent
    // SOCKET_OPTION_NO_DELAY - disable nagle's algorithm
};
typedef uint eSocketOptions;

class Socket
{
public:
    static size_t Error;

public:
    Socket() {}
    // used for accept
    Socket( unsigned __int64 socket );

    virtual ~Socket();

    void SetBlocking( bool block );

    void Close();

    // - - - - - -
    // HELPERS
    // - - - - - -
    bool IsClosed() const;

    bool IsFatalError( size_t result ) const;

    bool HasFatalError() const;

    int GetLastErrorCode() const;

    string GetLastErrorString() const;



public:
    unsigned __int64 m_sock = (unsigned __int64) -1;

    // if you're a listening, the address is YOUR address
    // if you are connecting (or socket is from an accept)
    // this address is THEIR address;  (do not listen AND connect on the same socket)
    NetAddress m_address;
    bool m_isClosed = true;
    bool m_blocking = true;


    // used if you want to set options while closed,
    // used to keep track so you can apply_options() after the socket
    // gets created;
    eSocketOptions m_options;
};
