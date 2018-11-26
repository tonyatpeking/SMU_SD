#include "Engine/Net/NetMessageChannel.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Net/NetMessage.hpp"


NetMessageChannel::NetMessageChannel()
{

}

NetMessageChannel::~NetMessageChannel()
{
    for( auto& msg : m_outOfOrderMessages )
    {
        delete msg;
    }
}

void NetMessageChannel::IncrementExpectedSequenceID()
{
    ++m_nextExpectedSequenceID;
}

NetMessage* NetMessageChannel::PopMessageInOrder()
{
    for( int i = (int) m_outOfOrderMessages.size() - 1; i >= 0; --i )
    {
        NetMessage* msg = m_outOfOrderMessages[i];
        if( IsMessageExpected( msg ) )
        {
            ContainerUtils::EraseAtIndexFast( m_outOfOrderMessages, i );
            ++m_nextExpectedSequenceID;
            return msg;
        }
    }
    return nullptr;
}

bool NetMessageChannel::IsMessageExpected( NetMessage* msg )
{
    return msg->m_sequenceID == m_nextExpectedSequenceID;
}

void NetMessageChannel::CloneAndPushMessage( NetMessage* msg )
{
    NetMessage* clone = new NetMessage( *msg );
    m_outOfOrderMessages.push_back( clone );
}
