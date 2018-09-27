#include "Engine/Net/EngineNetMessages.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/EngineCommonC.hpp"



#define NET_MESSAGE( functionName )\
void functionName ## _net_message( NetMessage* netMessage );\
static NetMessageSelfRegister functionName ## _self_register = NetMessageSelfRegister( #functionName, functionName ## _net_message );\
void functionName ## _net_message( NetMessage* netMessage )


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


void EngineNetMessages::RegisterAllToSession( NetSession* session )
{
//     if( session )
//         for( auto& pair : s_allEngineMessages )
//         {
//             session->RegisterMessageDefinition( pair.first, pair.second );
//         }
}


NET_MESSAGE( ping )
{
    UNUSED( netMessage );
}