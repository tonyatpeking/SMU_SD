#pragma once

class NetAddress;
class UDPSocket;
class NetPacket;

// collection of UDP sockets to communicate on
// can allocate and free packets
class PacketChannel
{
public:
    // tries to bind to an additional UDP Socket
    // returns the index assigned to this socket.
    // returns -1 if it failed to bind;
    int Bind( const NetAddress& addr );

    void Send( const NetAddress& to, const NetPacket& packet );

    bool Receive( const NetAddress& from, const NetPacket& packet );

public:

    UDPSocket* m_socket;


};