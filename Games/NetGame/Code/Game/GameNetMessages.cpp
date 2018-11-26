#include "Game/GameNetMessages.hpp"
#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessage.hpp"

//--------------------------------------------------------------------------------------
// Game Message Test

namespace GameNetMessages
{

NET_MESSAGE_STATIC_REGSITER_AUTO( test )
{
    UNUSED( netMessage );
    return true;
}


//--------------------------------------------------------------------------------------
// UnreliableTest

NetMessage* Compose_UnreliableTest( uint currentCount, uint totalCount )
{
    NetMessage* msg = new NetMessage( "unreliable_test" );
    string msgString = Stringf( "(%u,%u)", currentCount, totalCount );
    msg->WriteString( msgString.c_str() );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER( unreliable_test, eNetMessageFlag::DEFAULT,
                             eNetGameMessageIdx::UNRELIABLE_TEST, 0 )
{
    UNUSED( netMessage );
    string out_str;
    netMessage->ReadString( out_str );
    LOG_INFO_TAG( "Net", "Recieved Unreliable Test %s", out_str.c_str() );
    return true;
}

//--------------------------------------------------------------------------------------
// ReliableTest


NetMessage* Compose_ReliableTest( uint currentCount, uint totalCount )
{
    NetMessage* msg = new NetMessage( "reliable_test" );
    string msgString = Stringf( "(%u,%u)", currentCount, totalCount );
    msg->WriteString( msgString.c_str() );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER( reliable_test, eNetMessageFlag::RELIABLE,
                             eNetGameMessageIdx::RELIABLE_TEST, 0 )
{
    UNUSED( netMessage );
    string out_str;
    netMessage->ReadString( out_str );
    LOG_INFO_TAG( "Net", "Recieved Reliable Test %s", out_str.c_str() );
    return true;
}

//--------------------------------------------------------------------------------------
// SequenceTest
NetMessage* Compose_SequenceTest( uint currentCount, uint totalCount )
{
    NetMessage* msg = new NetMessage( "sequence_test" );
    string msgString = Stringf( "(%u,%u)", currentCount, totalCount );
    msg->WriteString( msgString.c_str() );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    sequence_test,
    eNetMessageFlag::RELIABLE_IN_ORDER,
    eNetGameMessageIdx::SEQUENCE_TEST, 0 )
{
    string out_str;
    netMessage->ReadString( out_str );
    LOG_INFO_TAG( "Net", "Recieved Sequence Test %s", out_str.c_str() );
    return true;
}



}