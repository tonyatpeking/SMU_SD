#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"
#include "Engine/DataUtils/BytePacker.hpp"

class NetConnection;
class NetMessageDefinition;

// [uint8_t message_id] // this is header for now
// [byte_t* message_payload] // will be message_and_header_length - 1U long for now


class NetMessage : public BytePacker
{
public:
    NetMessage();
    NetMessage( const string& name );
    ~NetMessage() {};

    void WriteMessageHeader( MessageID msgID );
    // also sets the read head to be after the header
    MessageID ReadMessageID();

public:

    Byte m_localBuffer[MESSAGE_MTU];

    string m_name;

    NetConnection* m_sender = nullptr;
    NetConnection* m_receiver = nullptr;

};
