#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Net/NetDefines.hpp"
#include "Engine/DataUtils/BytePacker.hpp"

class NetMessageDefinition;

class NetMessage : public BytePacker
{
public:
	NetMessage( const string& name );
	~NetMessage(){};


private:

    Byte m_localBuffer[MESSAGE_MTU];

    const NetMessageDefinition* m_definition;

};
