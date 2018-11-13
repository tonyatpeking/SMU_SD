#pragma once

#include "Engine/Net/NetDefines.hpp"

class NetMessage;

namespace GameNetMessages
{
enum eNetGameMessageIdx : uint8
{
    TEST_GAME_MESSAGE = eNetCoreMessageIdx::COUNT,

    UNRELIABLE_TEST = 128,
    RELIABLE_TEST = 129,
};

NetMessage* Compose_UnreliableTest( uint currentCount, uint totalCount );
NetMessage* Compose_ReliableTest( uint currentCount, uint totalCount );

}