#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"

class NetMessageDefinition
{
public:
	NetMessageDefinition( const string& name, NetMessageCB cb );
	~NetMessageDefinition(){};

public:

    string m_name;
    NetMessageCB m_callback;
    MessageID m_id = INVALID_MESSAGE_ID;

};
