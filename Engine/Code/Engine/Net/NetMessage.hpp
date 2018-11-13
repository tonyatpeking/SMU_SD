#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetAddress.hpp"

class NetConnection;
class NetMessageDefinition;

// [uint8 messageID] // this is header for now
// [uint16 reliableID // only for reliable
// [byte_t* messagePayload] // will be message_and_header_length - 1U long for now


class NetMessage : public BytePacker
{
public:
    NetMessage( const string& name );
    NetMessage();
    ~NetMessage() {};

    // also sets the read head to be after the header
    MessageID GetMessageID();
    bool UnpackHeader();
    void PackHeader();
    void SetWriteHeadToPayload();

public:

    Byte m_localBuffer[MESSAGE_MTU];

    string m_name;
    const NetMessageDefinition* m_def;

    NetAddress m_senderAddress;
    NetConnection* m_sender = nullptr;
    NetConnection* m_receiver = nullptr;

    MessageID m_id;

    // reliable
    uint16 m_reliableID = 0;
    float m_lastSentTime = 0.f;



};
