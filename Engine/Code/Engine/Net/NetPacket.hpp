#pragma once
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetDefines.hpp"

class NetMessage;
class NetConnection;

// All BytePackers are LITTLE_ENDIAN
// A packet header is..
// [uint8_t sender_conn_idx]
// [uint8_t unreliable_count]

// Followed by all unreliables, each message having the format
// [uint16_t message_and_header_length]
// [uint8_t message_index] // this is header for now
// [byte_t* message_payload] // will be message_and_header_length - 1U long for now

struct PacketHeader
{
    uint8 m_senderConnectionIdx;
    uint8 m_unreliableCount;
};

class NetPacket : public BytePacker
{
public:
	NetPacket();
	virtual ~NetPacket(){};

    // Header
    void SetSenderIdx( uint8 senderIdx ) { m_header.m_senderConnectionIdx = senderIdx; }
    bool ReadHeader( PacketHeader& out_header );

    bool WriteUnreliableMessage( NetMessage const &msg );
    bool WriteMessage( NetMessage const &msg );
    bool ReadMessage( NetMessage& out_msg );

    uint16 GetBufferMaxSize() { return PACKET_MTU; };

public:
    void FinalizeHeader();

    bool m_headerFinalized = false;

    PacketHeader m_header;

    Byte m_localBuffer[PACKET_MTU];

    NetConnection* m_sender = nullptr;
    NetConnection* m_receiver = nullptr;
};
