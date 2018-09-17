#pragma once

#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/UDPSocket.hpp"

struct NetAddress;

// class test
#define GAME_PORT "10084"
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it;
#define PACKET_MTU (ETHERNET_MTU - 40 - 8)

class UDPTest
{
public:

    static UDPTest * GetInstance();

    bool Start( const String& port = GAME_PORT );

    void Stop();

    void SendTo( const NetAddress &addr, void const *buffer, size_t byte_count );

    void Update();


public:
    // if you have multiple address, you can use multiple sockets
    // but you have to be clear on which one you're sending from;
    UDPSocket m_socket;
};