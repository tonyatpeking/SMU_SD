#include "Engine/DataUtils/EndianUtils.hpp"


#include <utility>

Endianness EndianUtils::GetPlatformEndianness()
{
    int num = 1;

    if( *( (char*) &num ) == 1 )
        return Endianness::LITTLE;

    else
        return Endianness::BIG;
}

void EndianUtils::FromEndianness( size_t const size, void* data, Endianness from )
{
    ConvertEndianness( size, data, from, GetPlatformEndianness() );
}

void EndianUtils::ToEndianness( size_t const size, void* data, Endianness to )
{
    ConvertEndianness( size, data, GetPlatformEndianness(), to );
}

void EndianUtils::ConvertEndianness( size_t const size, void* data,
                                     Endianness from, Endianness to )
{
    if( from == to )
        return;

    Byte* dataPtr = (Byte*) data;
    size_t start = 0;
    size_t end = size - 1;
    while( end > start )
    {
        std::swap( *( dataPtr + start ), *( dataPtr + end ) );
        ++start;
        --end;
    }

}
