#pragma once
#include "Engine/Net/NetCommonH.hpp"
#include "Engine/Net/NetAddress.hpp"

struct NetConnectionInfo
{
    NetAddress m_address;
    char m_id[MAX_CONNECTION_ID_LENGTH];
    uint8 m_sessionIdx;
};
