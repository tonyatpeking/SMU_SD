#pragma once
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetDefines.hpp"

class NetMessage;

struct PacketHeader
{
    uint8_t m_senderConnectionIdx;
    uint8_t m_unreliableCount;
};

class NetPacket : public BytePacker
{
public:
	NetPacket();
	~NetPacket(){};

    void WriteHeader( const PacketHeader& header );
    bool ReadHeader( PacketHeader& out_header );

    bool WriteMessage( NetMessage const &msg );
    bool ReadMessage( NetMessage& out_msg );

public:

    Byte m_localBuffer[PACKET_MTU];

    NetNode m_sender;
    NetNode m_receiver;


};
