#pragma once

class NetAddress;
class UDPSocket;
class NetPacket;
class NetSession;

// collection of UDP sockets to communicate on
// can allocate and free packets
class PacketChannel
{
public:
    // tries to bind to an additional UDP Socket
    // returns the index assigned to this socket.
    // returns -1 if it failed to bind;
    bool Bind( const NetAddress& addr );

    void Send( const NetPacket& packet );

    bool Receive( NetPacket& out_packet );

    bool IsClosed();

public:

    UDPSocket* m_socket;
    NetSession* m_owningSession;

};