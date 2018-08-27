#pragma once
#include "Engine/Net/NetAddress.hpp"

class TCPSocket
{
public:
	TCPSocket(){};
	~TCPSocket();

    bool Listen( uint port, uint maxQueued = 5 );

    TCPSocket* Accept();

    // for joining
    bool Connect( const NetAddress& addr );

    // when finished
    void Close();

    // - - - - - -
    // TRAFFIC
    // - - - - - -
    // returns how much sent
    size_t Send( void const *data, int dataByteSize );
    // returns how much received
    size_t Receive( void *buffer, int maxByteSize );

    // - - - - - -
    // HELPERS
    // - - - - - -
    bool IsClosed() const;

public:
    unsigned __int64 m_sock;

    // if you're a listening, the address is YOUR address
    // if you are connecting (or socket is from an accept)
    // this address is THEIR address;  (do not listen AND connect on the same socket)
    NetAddress m_address;
    bool m_isClosed = true;


};
