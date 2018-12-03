#pragma once
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetCommonH.hpp"
#include "Engine/Net/NetAddress.hpp"

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

#pragma pack(push,1)
struct PacketHeader
{
    uint8 m_senderConnectionIdx = INVALID_CONNECTION_INDEX;
    uint16 m_ack = INVALID_PACKET_ACK;
    uint16 m_lastReceivedAck = INVALID_PACKET_ACK;
    uint16 m_receivedAckBitfield = 0;
    uint8 m_message_count = 0;
};
#pragma pack(pop)

class NetPacket : public BytePacker
{
public:
	NetPacket();
	virtual ~NetPacket(){};

    // Header
    void PackHeader();
    bool UnpackHeader();

    bool WriteMessage( NetMessage& msg );
    bool CanWriteMessage( NetMessage& msg );
    bool ExtractMessage( NetMessage& out_msg );
    bool IsValid();

    uint16 GetBufferMaxSize() { return PACKET_MTU; };

public:

    PacketHeader m_header;

    Byte m_localBuffer[PACKET_MTU];

    NetAddress m_senderAddress;
    NetAddress m_receiverAddress;

    float m_timeOfReceive = 0;
    uint m_reliableMessageCount = 0;

    uint8 m_senderIdx = 0U;
    uint8 m_receiverIdx = 0U;
};
