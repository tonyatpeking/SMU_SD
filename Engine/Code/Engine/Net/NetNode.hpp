#pragma once
#include "Engine/Net/NetAddress.hpp"


class NetNode
{
public:

	NetNode(){};
	~NetNode(){};

public:
    bool m_isActive = false;
    NetAddress m_address;
    int m_pingMS = 9999;
    float m_health = 0;


};
