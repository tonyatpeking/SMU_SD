#include "Engine/Net/NetPacket.hpp"





NetPacket::NetPacket()
    : BytePacker( PACKET_MTU, m_localBuffer )
{

}
