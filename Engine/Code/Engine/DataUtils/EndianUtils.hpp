#pragma once
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/DataUtils/EndianUtils.hpp"
#include "Engine/Core/SmartEnum.hpp"

SMART_ENUM(
    Endianness,

    LITTLE,
    BIG
)



namespace EndianUtils
{

Endianness GetPlatformEndianness();

// Assumes data is in platform endianness, and will convert to supplied endianness;
void FromEndianness( size_t const size, void* data, Endianness from );

// Assumes data is in supplied endianness, and will convert to platform's endianness
void ToEndianness( size_t const size, void* data, Endianness to );

void ConvertEndianness( size_t const size, void* data, Endianness from, Endianness to );


template <typename T>
void FromEndianness( T* data, Endianness from )
{
    FromEndianness( sizeof( T ), (void*) data, from );
}

template <typename T>
void ToEndianness( T* data, Endianness to )
{
    ToEndianness( sizeof( T ), (void*) data, to );
}

template <typename T>
void ConvertEndianness( T* data, Endianness from, Endianness to )
{
    ConvertEndianness( sizeof( T ), (void*) data, from, to );
}

};
