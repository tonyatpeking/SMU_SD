#pragma once
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetDefines.hpp"

class NetMessage;
class NetConnection;

// All BytePackers are LITTLE_ENDIAN
// A packet header is..
// [uint8 sender_conn_idx]
// [uint8 unreliable_count]

// Followed by all unreliables, each message having the format
// [uint16 message_and_header_length]
// [uint8 message_index] // this is header for now
// [Byte* message_payload] // will be message_and_header_length - 1U long for now

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
    void PackHeader();
    bool UnpackHeader();

    bool WriteUnreliableMessage( NetMessage const &msg );
    bool WriteMessage( NetMessage const &msg );
    bool ReadMessage( NetMessage& out_msg );
    bool IsValid();

    uint16 GetBufferMaxSize() { return PACKET_MTU; };

public:

    PacketHeader m_header;

    Byte m_localBuffer[PACKET_MTU];

    NetConnection* m_sender = nullptr;
    NetConnection* m_receiver = nullptr;
};
