#pragma once

#include "Engine/Net/NetCommonH.hpp"

class NetMessage;

namespace GameNetMessages
{
enum eNetGameMessageIdx : uint8
{
    TEST_GAME_MESSAGE = eNetCoreMessageIdx::NET_CORE_COUNT,

    UNRELIABLE_TEST = 128,
    RELIABLE_TEST = 129,
    SEQUENCE_TEST = 130
};

NetMessage* Compose_UnreliableTest( uint currentCount, uint totalCount );
NetMessage* Compose_ReliableTest( uint currentCount, uint totalCount );
NetMessage* Compose_SequenceTest( uint currentCount, uint totalCount );

}