#pragma once
#include "Engine/Core/EngineCommonH.hpp"

class NetSession;
class NetMessage;

namespace EngineNetMessages
{

void RegisterAllToSession( NetSession* session );

NetMessage* ComposePing( const string& str );
NetMessage* ComposePong();
NetMessage* ComposeAdd( float a, float b );
NetMessage* ComposeAddResponse( float a, float b, float sum );

};
