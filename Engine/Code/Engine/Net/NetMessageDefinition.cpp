#include "Engine/Net/NetMessageDefinition.hpp"



NetMessageDefinition::NetMessageDefinition(
    const string& name, NetMessageCB cb )
    : m_name( name )
    , m_callback( cb )
{

}
