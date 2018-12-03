#pragma once

#include "Engine/Core/EngineCommonH.hpp"

#define SIM_NET_LOSS
#define SIM_NET_LATENCY

typedef uint8 MessageID;
typedef uint16 ReliableID;
typedef uint16 SequenceID;

// General
constexpr uint GAME_PORT = 10084;
constexpr uint ETHERNET_MTU = 1500;  // maximum transmission unit - determined by hardware part of OSI model.;
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it;
constexpr uint PACKET_MTU = ( ETHERNET_MTU - 40 - 8 );
constexpr uint MESSAGE_MTU = 1024; // arbitrary;
constexpr uint INVALID_PORT = (uint) ( ~0 );
constexpr uint INVALID_IPV4_ADDR = (uint) ( ~0 );
constexpr uint8 MAX_MESSAGES_PER_PACKET = (uint8) ( ~0 );

// Packet
constexpr uint16 INVALID_PACKET_ACK = (uint16) ( ~0 );
constexpr bool VALIDATE_PACKET = true;
constexpr bool LOG_PACKET_BYTES = false;
constexpr size_t PACKET_TRACKER_COUNT = 100; // 5 seconds of history

// Message
constexpr MessageID NET_MESSAGE_ID_INVALID = (MessageID) ( ~0 );
constexpr MessageID NET_MESSAGE_ID_AUTO = (MessageID) ( ~0 );
constexpr size_t MAX_MESSAGE_CHANNELS = 8;


// Connection
constexpr uint8 INVALID_CONNECTION_INDEX = (uint8) ( ~0 );
constexpr size_t MAX_CONNECTION_ID_LENGTH = 16;
constexpr size_t MAX_CONNECTION_COUNT = INVALID_CONNECTION_INDEX - 1;

// Send rate
#define DEFAULT_SESSION_SEND_RATE (20) // Hz
#define MAX_SEND_RATE (200) // Hz
#define DEFAULT_HEARTBEAT_RATE (20) // Hz

// Reliable
#define RELIABLE_RESEND_WAIT_MULTIPLIER (1.2f) // This is multiplied with the RTT
#define RELIABLE_RESEND_WAIT_MIN (0.03f) // Seconds
#define RELIABLE_RESEND_WAIT_MAX (1.f) // Seconds
#define RELIABLE_RESEND_WAIT_FIXED (0.1f) // Seconds
#define MAX_RELIABLES_PER_PACKET (32)
#define RELIABLE_WINDOW (64)
#define JOIN_REQUEST_RESEND_TIME (0.1f) // Seconds
#define JOIN_TIMEOUT (5.f) // Seconds

// Timeout
#define DEFAULT_CONNECTION_TIMEOUT (5.f) // Seconds
// NetClock
#define ROUND_TRIP_TIME_INERTIA (0.8f) // [0,1]  higher values mean changes slower
#define MAX_NET_TIME_DILATION (0.1f)
#define DESIRED_CLIENT_TIME_INERTIA (0.8f) // [0,1] higher values mean changes slower


class NetMessage;
typedef bool( *NetMessageCB )( NetMessage* netMessage );

struct NetAddress;
struct NetConnectionInfo;
class NetPacket;
class BytePacker;
class NetConnection;
class UDPSocket;
class NetMessageDefinition;
class NetMessageChannel;
class PacketChannel;
class PacketTracker;
class NetSession;


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
    NETMSG_CORE_PING = 0,    // unreliable, connectionless
    NETMSG_CORE_PONG = 2, 		// unreliable, connectionless
    NETMSG_CORE_HEARTBEAT,	// unreliable

    NETMSG_JOIN_REQUEST, // unreliable, connectionless
    NETMSG_JOIN_DENY, // unreliable, connectionless
    NETMSG_JOIN_ACCEPT, // reliable in-order
    NETMSG_NEW_CONNECTION, // reliable in-order
    NETMSG_JOIN_FINISHED, // reliable in-order
    NETMSG_UPDATE_CONNECTION_STATE, // reliable in-order
    NETMSG_HANGUP, // unreliable


    NET_CORE_COUNT,
};

enum class eSessionState
{
    DISCONNECTED = 0,  // Session can be modified
    SHOULD_DISCONNECT,
    BOUND,             // Bound to a socket - can send and receive connectionless messages.  No connections exist
    CONNECTING,        // Attempting to connecting - waiting for response from the host
    JOINING,           // Has established a connection, waiting final setup information/join completion
    READY,             // We are fully in the session
};

enum class eSessionError
{
    OK,
    USER,                 // user disconnected

    JOIN_DENIED,          // generic deny error (release)
    JOIN_DENIED_NOT_HOST, // debug - tried to join someone who isn't hosting
    JOIN_DENIED_CLOSED,   // debug - not in a listen state
    JOIN_DENIED_FULL,     // debug - session was full

    FAILED_TO_BIND_PORT,
};

enum class eConnectionState
{
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    READY
};

