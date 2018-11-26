#include "Engine/Net/EngineNetMessages.hpp"

#include "Engine/Net/NetCommonC.hpp"


NetMessageSelfRegister::NetMessageSelfRegister(
    const string& name,
    NetMessageCB cb,
    eNetMessageFlag flags /*= eNetMessageFlag::DEFAULT*/,
    uint8 messageId /*= NET_MESSAGE_ID_AUTO*/,
    uint8 channelIdx /*= 0 */ )
{
    NetMessageDatabase::RegisterDefinition( name, cb, flags, messageId, channelIdx );
}

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
    ping, eNetMessageFlag::CONNECTIONLESS,
    eNetCoreMessageIdx::NETMSG_CORE_PING, 0 )
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
    pong, eNetMessageFlag::CONNECTIONLESS,
    eNetCoreMessageIdx::NETMSG_CORE_PONG, 0 )
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
    heartbeat, eNetMessageFlag::DEFAULT,
    eNetCoreMessageIdx::NETMSG_CORE_HEARTBEAT, 0 )
{
    UNUSED( netMessage );
    //     LOG_INFO_TAG( "Net", "Incomming [%s] [heartbeat]",
    //                   netMessage->m_sender->m_address.ToStringAll().c_str() );
    return true;
}

//--------------------------------------------------------------------------------------
// JoinRequest
NetMessage* Compose_JoinRequest()
{
    NetMessage* msg = new NetMessage( "join_request" );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    join_request, eNetMessageFlag::CONNECTIONLESS,
    eNetCoreMessageIdx::NETMSG_JOIN_REQUEST, 0 )
{
    return NetSession::GetDefault()->ProcessJoinRequest( netMessage );
}

//--------------------------------------------------------------------------------------
// JoinDeny
NetMessage* Compose_JoinDeny()
{
    NetMessage* msg = new NetMessage( "join_deny" );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    join_deny, eNetMessageFlag::CONNECTIONLESS,
    eNetCoreMessageIdx::NETMSG_JOIN_DENY, 0 )
{
    return NetSession::GetDefault()->ProcessJoinDeny( netMessage );
}

//--------------------------------------------------------------------------------------
// JoinAccept
NetMessage* Compose_JoinAccept( uint8 assignedConnectionIdx )
{
    NetMessage* msg = new NetMessage( "join_accept" );
    msg->Write( assignedConnectionIdx );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    join_accept, eNetMessageFlag::RELIABLE_IN_ORDER,
    eNetCoreMessageIdx::NETMSG_JOIN_ACCEPT, 0 )
{
    uint8 assignedIdx;
    if( !netMessage->Read( &assignedIdx ) )
        return false;

    return NetSession::GetDefault()->ProcessJoinAccept( assignedIdx );
}

//--------------------------------------------------------------------------------------
// NewConnection
NetMessage* Compose_NewConnection()
{
    NetMessage* msg = new NetMessage( "new_connection" );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    new_connection, eNetMessageFlag::RELIABLE_IN_ORDER,
    eNetCoreMessageIdx::NETMSG_NEW_CONNECTION, 0 )
{
    return NetSession::GetDefault()->ProcessNewConnection( netMessage );
}

//--------------------------------------------------------------------------------------
// JoinFinished
NetMessage* Compose_JoinFinished()
{
    NetMessage* msg = new NetMessage( "join_finished" );
    return msg;
}

NET_MESSAGE_STATIC_REGSITER(
    join_finished, eNetMessageFlag::RELIABLE_IN_ORDER,
    eNetCoreMessageIdx::NETMSG_JOIN_FINISHED, 0 )
{
    return NetSession::GetDefault()->ProcessJoinFinished( netMessage );
}

//--------------------------------------------------------------------------------------
// UpdateConnectionState
NetMessage* Compose_UpdateConnectionState( eConnectionState state )
{
    NetMessage* msg = new NetMessage( "update_connection_state" );
    msg->Write( (uint8) state );
    return msg;
}


NET_MESSAGE_STATIC_REGSITER(
    update_connection_state, eNetMessageFlag::RELIABLE_IN_ORDER,
    eNetCoreMessageIdx::NETMSG_UPDATE_CONNECTION_STATE, 0 )
{
    uint8 state;
    if( !netMessage->Read( &state ) )
        return false;
    return NetSession::GetDefault()->ProcessUpdateConnectionState(
        netMessage, (eConnectionState) state );
}



}


