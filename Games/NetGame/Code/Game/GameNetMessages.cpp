#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Game/NetCube.hpp"

#include "Game/GameNetMessages.hpp"
#include "Game/GameState_Playing.hpp"

//--------------------------------------------------------------------------------------
// Game Message Test

namespace GameNetMessages
{


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

//--------------------------------------------------------------------------------------
// CreateCube
NetMessage* Compose_CreateCube( NetCube* cube )
{
    NetMessage* msg = new NetMessage( "create_cube" );
    Transform& t = cube->GetTransform();
    msg->Write( t.GetLocalPosition() );
    msg->Write( t.GetLocalScale() );
    msg->Write( cube->GetColor() );
    msg->Write( cube->m_velocity );
    msg->Write( cube->GetNetID() );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO(
    create_cube,
    eNetMessageFlag::RELIABLE_IN_ORDER )
{
    Vec3 pos, scale, velocity;
    Rgba color;
    uint16 netID;
    netMessage->Read( &pos );
    netMessage->Read( &scale );
    netMessage->Read( &color );
    netMessage->Read( &velocity );
    netMessage->Read( &netID );
    GameState_Playing::GetDefault()->Process_CreateCube(
        pos, velocity, scale, color, netID );
    return true;
}

//--------------------------------------------------------------------------------------
// DestroyCube

NetMessage* Compose_DestroyCube( uint16 netID )
{
    NetMessage* msg = new NetMessage( "destroy_cube" );
    msg->Write( netID );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO(
    destroy_cube,
    eNetMessageFlag::RELIABLE_IN_ORDER )
{
    uint16 netID;
    netMessage->Read( &netID );
    GameState_Playing::GetDefault()->Process_DestroyCube( netID );
    return true;
}

//--------------------------------------------------------------------------------------
// UpdateCube

NetMessage* Compose_UpdateCube( NetCube* cube )
{
    NetMessage* msg = new NetMessage( "update_cube" );
    Transform& t = cube->GetTransform();
    msg->Write( t.GetLocalPosition() );
    msg->Write( cube->GetColor() );
    msg->Write( cube->GetNetID() );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO(
    update_cube,
    eNetMessageFlag::DEFAULT )
{
    Vec3 pos;
    Rgba color;
    uint16 netID;
    netMessage->Read( &pos );
    netMessage->Read( &color );
    netMessage->Read( &netID );
    GameState_Playing::GetDefault()->Process_UpdateCube(
        pos, color, netID );
    return true;
}

//--------------------------------------------------------------------------------------
// SendInputs


NetMessage* Compose_SendInputs( const ClientInputs& inputs )
{
    NetMessage* msg = new NetMessage( "send_inputs" );
    msg->Write( inputs.up );
    msg->Write( inputs.left );
    msg->Write( inputs.down );
    msg->Write( inputs.right );
    msg->Write( inputs.fire );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO(
    send_inputs,
    eNetMessageFlag::DEFAULT )
{
    ClientInputs inputs;
    netMessage->Read( &inputs.up );
    netMessage->Read( &inputs.left );
    netMessage->Read( &inputs.down );
    netMessage->Read( &inputs.right );
    netMessage->Read( &inputs.fire );
    uint8 playerID = netMessage->m_senderIdx;
    GameState_Playing::GetDefault()->Process_SendInputs( playerID, inputs );
    return true;
}

//--------------------------------------------------------------------------------------
// EnterGame
NetMessage* Compose_EnterGame()
{
    NetMessage* msg = new NetMessage( "enter_game" );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO(
    enter_game,
    eNetMessageFlag::RELIABLE_IN_ORDER )
{
    uint8 playerID = netMessage->m_senderIdx;

    GameState_Playing::GetDefault()->Process_EnterGame( playerID );
    return true;
}

}