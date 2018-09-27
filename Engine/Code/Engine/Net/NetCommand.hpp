#pragma once
#include "Engine/Core/EngineCommonH.hpp"

class NetPacket;

typedef void( *NetCommandCallback )( NetPacket& );
typedef size_t( *GetLengthCallback )( NetPacket& );


class NetCommand
{
public:
    NetCommand(){};
	~NetCommand(){};

public:

    string m_name;
    NetCommandCallback m_callback = nullptr;
    GetLengthCallback m_getLengthCallback = nullptr;
    size_t m_fixedLength;

};
