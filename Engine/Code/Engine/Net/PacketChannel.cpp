
#include "Engine/Net/NetCommonC.hpp"

#include "Engine/Math/MathUtils.hpp"

bool PacketChannel::Bind( const NetAddress& addr )
{
    delete m_socket;
    m_socket = new UDPSocket();
    if( !m_socket->Bind( addr ) )
    {
        LOG_WARNING_TAG( "Net", "Failed to bind." );
        return false;
    }
    else
    {
        m_socket->SetBlocking( false );

        LOG_INFO_TAG( "Net", "Socket bound: %s", m_socket->m_address.ToStringAll().c_str() );
        return true;
    }
}

void PacketChannel::Close()
{
    m_socket->Close();
}

void PacketChannel::SendImmediate( const NetPacket& packet )
{
    if( IsClosed() )
        LOG_ERROR_TAG( "Net", "Cannot send, socket is closed or unbound" );

    if( packet.m_receiverAddress.IsInvalid() )
        LOG_ERROR_TAG( "Net", "Cannot send, invalid packet receiver address" );

    m_socket->SendTo( packet.m_receiverAddress,
                      packet.m_localBuffer,
                      packet.GetWrittenByteCount() );
}

bool PacketChannel::Receive( NetPacket& out_packet )
{
    NetAddress senderAddr;
    size_t readBytes = m_socket->ReceiveFrom(
        senderAddr,
        out_packet.GetBuffer(),
        out_packet.GetBufferMaxSize() );

    out_packet.SetWriteHead( readBytes );

    if( readBytes > 0U )
    {
        if( !out_packet.UnpackHeader() )
            return false;

        NetConnection* connection = m_owningSession->GetConnection(
            out_packet.m_header.m_senderConnectionIdx );
        if( !connection )
            connection = m_owningSession->GetConnection( senderAddr );

        out_packet.m_sender = connection; // can be null
        out_packet.m_senderAddress = senderAddr;
        return true;
    }
    else
    {
        return false;
    }
}

bool PacketChannel::IsClosed()
{
    if( m_socket )
        return m_socket->IsClosed();
    return true;
}
