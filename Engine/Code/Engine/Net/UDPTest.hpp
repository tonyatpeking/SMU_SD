#pragma once

#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetDefines.hpp"
struct NetAddress;


class UDPTest
{
public:

    static UDPTest * GetInstance();

    bool Start( uint port = GAME_PORT );

    void Stop();

    void SendTo( const NetAddress &addr, void const *buffer, size_t byte_count );

    void Update();


public:
    // if you have multiple address, you can use multiple sockets
    // but you have to be clear on which one you're sending from;
    UDPSocket m_socket;
};