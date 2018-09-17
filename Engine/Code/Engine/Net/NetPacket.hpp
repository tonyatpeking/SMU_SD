#pragma once
#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Net/NetNode.hpp"


//------- Packet protocol -----------------------------------------------
//  2 bytes packet length
//  n bytes BytePacker
//      uint16_t NetCommandID
//      n bytes parameters
//      uint16_t NetCommandID
//      n bytes parameters
//      ...
//
//
// NetCommands will unpack and read parameters themselves
// the way to determine the length of parameters is unspecified, but each NetCommand
// must provide a GetParameterLength() functions that returns the length
// this allows NetCommands to be more flexible as variable length arguments are possible,
// and also allows NetCommands to have set length parameters to save space




typedef int UID;

class NetPacket
{
public:
	NetPacket(){};
	~NetPacket(){};
public:

    uint16_t m_length;
    BytePacker m_bytePacker;

    NetNode m_sender;
    NetNode m_receiver;

    UID m_UID;

};
