#pragma once

#include "Engine/Net/NetCommonH.hpp"
#include "Engine/Core/EngineCommonH.hpp"

class NetMessage;
class NetCube;
class ClientInputs;

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

NetMessage* Compose_CreateCube( NetCube* cube );
NetMessage* Compose_DestroyCube( uint16 netID );
NetMessage* Compose_UpdateCube( NetCube* cube );
NetMessage* Compose_SendInputs( const ClientInputs& inputs );

NetMessage* Compose_EnterGame();

}