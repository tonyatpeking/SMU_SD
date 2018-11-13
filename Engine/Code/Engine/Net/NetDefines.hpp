#pragma once
#include "Engine/Core/EngineCommonH.hpp"

#define SIM_NET_LOSS
#define SIM_NET_LATENCY

typedef uint8 MessageID;

constexpr uint GAME_PORT = 10084;
constexpr uint ETHERNET_MTU = 1500;  // maximum transmission unit - determined by hardware part of OSI model.;
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it;
constexpr uint PACKET_MTU = ( ETHERNET_MTU - 40 - 8 );

constexpr uint MESSAGE_MTU = 1024; // arbitrary;
constexpr uint8 INVALID_CONNECTION_INDEX =  (uint8) ( ~0 );
constexpr uint INVALID_PORT =  (uint) ( ~0 );
constexpr uint INVALID_IPV4_ADDR =  (uint) ( ~0 );


constexpr uint16 INVALID_PACKET_ACK =  (uint16) ( ~0 );
constexpr bool VALIDATE_PACKET = true;
constexpr bool LOG_PACKET_BYTES = false;
constexpr size_t PACKET_TRACKER_COUNT = 100; // 5 seconds of history

constexpr MessageID NET_MESSAGE_ID_INVALID =  (MessageID) ( ~0 );
constexpr MessageID NET_MESSAGE_ID_AUTO = (MessageID) ( ~0 );

#define DEFAULT_SESSION_SEND_RATE (20) // Hz
#define MAX_SEND_RATE (200) // Hz
#define DEFAULT_HEARTBEAT_RATE (0.5) // Hz


class NetMessage;
typedef bool( *NetMessageCB )( NetMessage* netMessage );


enum class eNetMessageFlag : uint
{
    DEFAULT = 0,       // Unreliable, requires a connection

    CONNECTIONLESS   = BIT_FLAG( 0 ), // can be sent/received without having a connection
    HOST_ONLY        = BIT_FLAG( 1 ), // Only the host is allowed to send this message
    CLIENT_ONLY      = BIT_FLAG( 2 ), // Only the client is allowed to send this message

    RELIABLE         = BIT_FLAG( 3 ),
    IN_ORDER         = BIT_FLAG( 4 ),
    RELIABLE_IN_ORDER = RELIABLE | IN_ORDER,
};


enum eNetCoreMessageIdx : uint8_t
{
    PING = 0,    // unreliable, connectionless
    PONG = 2, 		// unreliable, connectionless
    HEARTBEAT,	// unreliable

    COUNT,
};
