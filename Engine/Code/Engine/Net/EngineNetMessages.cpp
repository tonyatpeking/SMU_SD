#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetMessageDatabase.hpp"


namespace EngineNetMessages
{

//--------------------------------------------------------------------------------------
// Ping

NetMessage* Compose_Ping( const string& str )
{
    NetMessage* msg = new NetMessage( "ping" );
    msg->WriteString( str.c_str() );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    ping, eNetMessageFlag::CONNECTIONLESS, eNetCoreMessageIdx::PING, 0 )
{
    string pingMsg;
    netMessage->ReadString( pingMsg );
    LOG_INFO_TAG( "Net",
                  "Incomming [%s] [ping] %s",
                  netMessage->m_senderAddress.ToStringAll().c_str(),
                  pingMsg.c_str() );

    if( netMessage->m_sender )
    {
        NetMessage* pongMsg = Compose_Pong();
        netMessage->m_sender->QueueSend( pongMsg );
    }

    return true;
}

//--------------------------------------------------------------------------------------
// Pong

NetMessage* Compose_Pong()
{
    return new NetMessage( "pong" );
}

NET_MESSAGE_STATIC_REGSITER(
    pong, eNetMessageFlag::CONNECTIONLESS, eNetCoreMessageIdx::PONG, 0 )
{
    LOG_INFO_TAG( "Net",
                  "Incomming [%s] [pong]",
                  netMessage->m_senderAddress.ToStringAll().c_str() );
    return true;
}

//--------------------------------------------------------------------------------------
// Add

NetMessage* Compose_Add( float a, float b )
{
    NetMessage* msg = new NetMessage( "add" );
    msg->Write( a );
    msg->Write( b );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO( add )
{
    float a, b;

    if( !netMessage->Read( &a ) || !netMessage->Read( &b ) )
    {
        LOG_WARNING_TAG( "Net", "Bad add message" );
        return false;
    }
    LOG_INFO_TAG( "Net", "Incomming [%s] [add] %f + %f",
                  netMessage->m_senderAddress.ToStringAll().c_str(),
                  a, b );

    NetMessage* response = Compose_AddResponse( a, b, a + b );
    netMessage->m_sender->QueueSend( response );
    return true;
}

//--------------------------------------------------------------------------------------
// Add Response

NetMessage* Compose_AddResponse( float a, float b, float sum )
{
    NetMessage* msg = new NetMessage( "add_response" );
    msg->Write( a );
    msg->Write( b );
    msg->Write( sum );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER_AUTO( add_response )
{
    float a, b, sum;

    if( !netMessage->Read( &a )
        || !netMessage->Read( &b )
        || !netMessage->Read( &sum ) )
    {
        LOG_WARNING_TAG( "Net", "Bad add_response message" );
        return false;
    }
    LOG_INFO_TAG( "Net", "Incomming [%s] [add_response] %f + %f = %f",
                  netMessage->m_senderAddress.ToStringAll().c_str(),
                  a, b, sum );
    return true;
}

//--------------------------------------------------------------------------------------
// Heartbeat

NetMessage* Compose_Heartbeat()
{
    NetMessage* msg = new NetMessage( "heartbeat" );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    heartbeat, eNetMessageFlag::DEFAULT, eNetCoreMessageIdx::HEARTBEAT, 0 )
{
    //     LOG_INFO_TAG( "Net", "Incomming [%s] [heartbeat]",
    //                   netMessage->m_sender->m_address.ToStringAll().c_str() );
    return true;
}


}

NetMessageSelfRegister::NetMessageSelfRegister(
    const string& name,
    NetMessageCB cb,
    eNetMessageFlag flags /*= eNetMessageFlag::DEFAULT*/,
    uint8 messageId /*= NET_MESSAGE_ID_AUTO*/,
    uint8 channelIdx /*= 0 */ )
{
    NetMessageDatabase::RegisterDefinition( name, cb, flags, messageId, channelIdx );
}
