#pragma once
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/EngineCommonH.hpp"

class NetMessage;
class NetSession;

class NetNode
{
public:

	NetNode(){};
	~NetNode(){};

    void ProcessOutgoing();

public:
    NetAddress m_address;

    vector<NetMessage*> m_outboundUnreliables;

    NetSession* m_owningSession = nullptr;

    uint8 m_idxInSession = (uint8)-1;
};
