#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"
#include "Engine/DataUtils/BytePacker.hpp"

class NetConnection;
class NetMessageDefinition;

// [uint8_t message_index] // this is header for now
// [byte_t* message_payload] // will be message_and_header_length - 1U long for now


class NetMessage : public BytePacker
{
public:
    NetMessage( const string& name );
    ~NetMessage() {};

    void WriteMessageHeader( uint8 msgIdx );
    // also sets the read head to be after the header
    uint8 ReadMessageIndex();
    NetMessage() {};
    ~NetMessage() {};

public:

    Byte m_localBuffer[MESSAGE_MTU];

    const NetMessageDefinition* m_definition;
    string m_name;

    NetConnection* m_sender = nullptr;
    NetConnection* m_receiver = nullptr;

};
