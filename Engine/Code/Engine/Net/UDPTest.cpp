#include "Engine/Net/UDPTest.hpp"

#include "Engine/Core/Console.hpp"
#include "Engine/Math/MathUtils.hpp"


// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Headre Size: 20B-60B
// UDP Header Size: 8B
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)


bool UDPTest::Start( const String& port )
{
    String portCopy;
    portCopy = port;
    if( portCopy == "" )
        portCopy = GAME_PORT;
    // get an address to use;
    NetAddress addr = NetAddress::GetLocal( portCopy );

    if( !m_socket.Bind( addr, 10 ) )
    {
        LOG_WARNING_TAG( "Net", "Failed to bind." );
        return false;
    }
    else
    {
        m_socket.SetBlocking( false ); // if you have cached options, you could do this
                                       // in the constructor;
        LOG_INFO_TAG( "Net", "Socket bound: %s", m_socket.m_address.ToStringAll().c_str() );
        return true;
    }
}

void UDPTest::Stop()
{
    m_socket.Close();
}

void UDPTest::SendTo( NetAddress const &addr, void const *buffer, size_t byte_count )
{
    m_socket.SendTo( addr, buffer, byte_count );
}

void UDPTest::Update()
{
    if( m_socket.IsClosed() )
        return;
    Byte buffer[PACKET_MTU];

    NetAddress from_addr;
    size_t read = m_socket.ReceiveFrom( from_addr, buffer, PACKET_MTU );

    if( read > 0U )
    {
        uint max_bytes = Min( (int) read, 128 );
        //String output = "0x";
        char* out = (char*) malloc( max_bytes * 2U + 3U );
        out[0] = '0';
        out[1] = '1';
        //output.set_max_size( max_bytes * 2U + 3U );
        char *iter = out;
        iter += 2U; // skip the 0x
        for( uint i = 0; i < read; ++i )
        {
            sprintf_s( iter, 3U, "%02X", buffer[i] );
            iter += 2U;
        }
        *iter = NULL;
        //output.recalculate_sizes();

        LOG_INFO( "Received from %s \n%s", from_addr.ToStringAll().c_str(), out );
        free( out );
    }
}

UDPTest * UDPTest::GetInstance()
{
    static UDPTest* s_instance = nullptr;
    if( s_instance == nullptr )
        s_instance = new UDPTest();

    return s_instance;
}
