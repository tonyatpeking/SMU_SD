#pragma once
#include "Engine/Net/NetDefines.hpp"

class NetMessageDefinition;

namespace NetMessageDatabase
{

bool RegisterDefinition(
    const string& name,
    NetMessageCB cb,
    eNetMessageFlag flags =  eNetMessageFlag::DEFAULT,
    uint8 messageId = NET_MESSAGE_ID_AUTO,
    uint8 channelIdx = 0
);

bool Finalize();

// Lookup
// Only valid after Finalize
const NetMessageDefinition* GetDefinitionByName( const string& name );
const NetMessageDefinition* GetDefinitionByID( const MessageID idx );

// overcomes unknown static init order
vector<NetMessageDefinition*>& GetMessageDefinitions();

};
