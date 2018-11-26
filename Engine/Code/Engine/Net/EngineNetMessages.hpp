#pragma once
#include "Engine/Net/NetCommonH.hpp"

class NetSession;
class NetMessage;

namespace EngineNetMessages
{

// Callbacks are named Execute_Name
NetMessage* Compose_Ping( const string& str );
NetMessage* Compose_Pong();
NetMessage* Compose_Add( float a, float b );
NetMessage* Compose_AddResponse( float a, float b, float sum );
NetMessage* Compose_Heartbeat();

NetMessage* Compose_JoinRequest();
NetMessage* Compose_JoinDeny();
NetMessage* Compose_JoinAccept( uint8 assignedConnectionIdx );
NetMessage* Compose_NewConnection();
NetMessage* Compose_JoinFinished();
NetMessage* Compose_UpdateConnectionState(eConnectionState state);



};


//--------------------------------------------------------------------------------------
// Self register using static magic

#define NET_MESSAGE_STATIC_REGSITER( name, flags, messageId, channelIdx )\
bool Execute_ ## name ( NetMessage* netMessage );\
static NetMessageSelfRegister name ## _self_register\
    = NetMessageSelfRegister( #name, Execute_ ## name, flags, messageId, channelIdx );\
bool Execute_ ## name ( NetMessage* netMessage )

#define NET_MESSAGE_STATIC_REGSITER_AUTO( name ) \
NET_MESSAGE_STATIC_REGSITER( name, eNetMessageFlag::DEFAULT, NET_MESSAGE_ID_AUTO, 0 )

// make a static variable of this to self register
class NetMessageSelfRegister
{
public:
    NetMessageSelfRegister(
        const string& name,
        NetMessageCB cb,
        eNetMessageFlag flags =  eNetMessageFlag::DEFAULT,
        uint8 messageId = NET_MESSAGE_ID_AUTO,
        uint8 channelIdx = 0 );
};

