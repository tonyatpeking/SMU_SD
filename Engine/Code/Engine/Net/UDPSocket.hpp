#pragma once
#include "Engine/Net/Socket.hpp"

struct NetAddress;

class UDPSocket : public Socket
{
public:
    bool Bind(
        const NetAddress &addr, // address I want to listen for traffic on
        uint16_t port_range = 0U );		  // how many additional ports to bind on (so you bind from [addr.port,addr.port + port_range])

    // return 0 on failure, otherwise how large is the next datagram
    size_t SendTo(
        NetAddress const &addr,
        const void *data,
        const size_t byte_count );

    size_t ReceiveFrom(
        NetAddress& out_addr,
        void *buffer,
        const size_t max_read_size );
};
