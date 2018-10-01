#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Net/NetConnection.hpp"

#define NET_MESSAGE_EXECUTER( name )\
bool Execute_ ## name ( NetMessage* netMessage );\
static NetMessageSelfRegister name ## _self_register = NetMessageSelfRegister( #name, Execute_ ## name );\
bool Execute_ ## name ( NetMessage* netMessage )

namespace
{

map<string, NetMessageCB> s_allEngineMessages;

// make a static variable of this to self register
class NetMessageSelfRegister
{
public:
    NetMessageSelfRegister( const string& name, NetMessageCB cb )
    {
        if( ContainerUtils::ContainsKey( s_allEngineMessages, name ) )
        {
            LOG_ERROR_TAG( "Net", "Adding duplicate message def" );
        }
        s_allEngineMessages[name] = cb;
    }
};

}

namespace EngineNetMessages
{

void RegisterAllToSession( NetSession* session )
{
    if( !session )
        return;

    for( auto& pair : s_allEngineMessages )
    {
        session->RegisterMessageDefinition( pair.first, pair.second );
    }
}

//--------------------------------------------------------------------------------------
// Ping

NetMessage* ComposePing( const string& str )
{
    NetMessage* msg = new NetMessage( "ping" );
    msg->WriteString( str.c_str() );
    return msg;
}

NET_MESSAGE_EXECUTER( ping )
{
    string pingMsg;
    netMessage->ReadString( pingMsg );
    LOG_INFO_TAG( "Net",
                  "Incomming [%s] [ping] %s",
                  netMessage->m_sender->m_address.ToStringAll().c_str(),
                  pingMsg.c_str() );

    NetMessage* pongMsg = ComposePong();
    netMessage->m_sender->QueueSend( pongMsg );
    return true;
}

//--------------------------------------------------------------------------------------
// Pong

NetMessage* ComposePong()
{
    return new NetMessage( "pong" );
}

NET_MESSAGE_EXECUTER( pong )
{
    LOG_INFO_TAG( "Net",
                  "Incomming [%s] [pong]",
                  netMessage->m_sender->m_address.ToStringAll().c_str() );
    return true;
}

//--------------------------------------------------------------------------------------
// Add

NetMessage* ComposeAdd( float a, float b )
{
    NetMessage* msg = new NetMessage( "add" );
    msg->Write( a );
    msg->Write( b );
    return msg;
}

NET_MESSAGE_EXECUTER( add )
{
    float a, b;

    if( !netMessage->Read( &a ) || !netMessage->Read( &b ) )
    {
        LOG_WARNING_TAG( "Net", "Bad add message" );
        return false;
    }
    LOG_INFO_TAG( "Net", "Incomming [%s] [add] %f + %f",
                  netMessage->m_sender->m_address.ToStringAll().c_str(),
                  a, b );

    NetMessage* response = ComposeAddResponse( a, b, a + b );
    netMessage->m_sender->QueueSend( response );
    return true;
}

//--------------------------------------------------------------------------------------
// Add Response

NetMessage* ComposeAddResponse( float a, float b, float sum )
{
    NetMessage* msg = new NetMessage( "add_response" );
    msg->Write( a );
    msg->Write( b );
    msg->Write( sum );
    return msg;
}

NET_MESSAGE_EXECUTER( add_response )
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
                  netMessage->m_sender->m_address.ToStringAll().c_str(),
                  a, b, sum );
    return true;
}






}

