#pragma once

#include "Engine/Net/NetCommonH.hpp"

// collection of UDP sockets to communicate on
// can allocate and free packets
class PacketChannel
{
public:
    // tries to bind to an additional UDP Socket
    // returns the index assigned to this socket.
    // returns -1 if it failed to bind;
    bool Bind( const NetAddress& addr );
    void Close();

    void SendImmediate( const NetPacket& packet );

    bool Receive( NetPacket& out_packet );

    bool IsClosed();

public:

    UDPSocket* m_socket;
    NetSession* m_owningSession;

};