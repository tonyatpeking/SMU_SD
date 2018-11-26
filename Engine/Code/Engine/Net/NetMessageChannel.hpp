#pragma once

#include "Engine/Core/EngineCommonH.hpp"

class NetMessage;

class NetMessageChannel
{
public:
	NetMessageChannel();
	~NetMessageChannel();

    void IncrementExpectedSequenceID();
    // caller should delete msg after use
    NetMessage* PopMessageInOrder();
    bool IsMessageExpected( NetMessage* msg );
    // clones message internally, caller should delete original
    void CloneAndPushMessage( NetMessage* msg );

    uint16 m_nextSequenceIDToSend = 0;            // used for sending
    uint16 m_nextExpectedSequenceID = 0;        // used for receiving
    vector<NetMessage*> m_outOfOrderMessages;   // used for receiving


};
