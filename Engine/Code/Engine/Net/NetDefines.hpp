#pragma once

#define GAME_PORT 10084
#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it;
#define PACKET_MTU (ETHERNET_MTU - 40 - 8)

#define MESSAGE_MTU 1024 // arbitrary
#define INVALID_CONNECTION_INDEX (~0)
#define INVALID_PORT (~0)
#define INVALID_IPV4_ADDR (~0)
#define INVALID_MESSAGE_INDEX (~0)

typedef uint16_t MessageID;

class NetMessage;
typedef bool( *NetMessageCB )( NetMessage* netMessage );