#pragma once
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/Socket.hpp"

class TCPSocket : public Socket
{
public:

    TCPSocket();

    TCPSocket( unsigned __int64 socket ) : Socket( socket ) {}

    ~TCPSocket();

    bool Listen( const NetAddress& localAddr, uint maxQueued = 5 );

    TCPSocket* Accept();

    bool Connect( const NetAddress& addr );

    // TRAFFIC
    // returns how much sent
    size_t Send( void const *data, int dataByteSize );
    // returns how much received
    size_t Receive( void *buffer, int maxByteSize );

    // will destroy buffered data
    void SetBufferSize( size_t bufferSize );

    // will destroy buffered data
    void MakeBuffer();

    size_t BufferIncomming( size_t bufferUpTo );

    // returns 0 for disconnect -1 for error/non-fatal
    // resets buffer upon success
    size_t ReceiveExact( void *buffer, size_t exactByteSize );

    // return 0 for disconnect -1 for error/non-fatal
    size_t ReceiveMessage( bool& out_isEcho, String& out_msg );


public:

    size_t m_bufferSize = 0xFFFF;
    Byte* m_receiveBuffer = nullptr;
    size_t m_bufferWriteHead = 0;

    unsigned short m_msgSize = 0;
};

