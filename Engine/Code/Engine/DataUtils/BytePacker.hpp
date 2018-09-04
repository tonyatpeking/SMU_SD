#pragma once

#include "Engine/DataUtils/EndianUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"

enum BytePackerOptionBit : uint
{
    // a byte packer allocated its own memory and
    // should free it when destroyed;
    BYTEPACKER_OWNS_MEMORY    = BIT_FLAG( 0 ),

    // If the bytepacker runs out of space, will
    // allocate more memory (maintaining the data that was stored)
    // Must have flag BYTEPACKER_OWNS_MEMORY
    BYTEPACKER_CAN_GROW       = BIT_FLAG( 1 )
};
typedef uint BytePackerOptions;



class BytePacker
{
public:
    // owns and can grow memory
    BytePacker( Endianness byteOrder = Endianness::LITTLE );

    // owns with set memory size
    BytePacker( size_t bufferSize,
                Endianness byteOrder = Endianness::LITTLE );

    // does not own memory with set size
    BytePacker( size_t bufferSize, void* buffer,
                Endianness byteOrder = Endianness::LITTLE );

    // only releases memory if it owns it
    ~BytePacker();

    // will fail if cannot grow
    bool Reserve( size_t byteCount );

    void SetEndianness( Endianness byteOrder );

    // all write calls go through this
    bool WriteBytes( size_t byteCount, const void* data );

    template <typename T>
    bool Write( T data )
    {
        EndianUtils::ToEndianness( &data, m_endianness );
        return WriteBytes( sizeof( T ), &data );
    }

    template <typename T>
    size_t Read( T* out_data )
    {
        size_t readByteCount = ReadBytes( out_data, sizeof( T ) );
        EndianUtils::FromEndianness( out_data, m_endianness );
        return readByteCount;
    }


    // all read calls go through this
    size_t ReadBytes( void* out_Data, size_t maxByteCount );

    size_t WriteLengthHeader( size_t length ); // returns how many bytes used

    size_t ReadLengthHeader( size_t& out_length ); // returns how many bytes read fills out_size

    // See notes on encoding!
    bool WriteString( const char* str );

    // if buffer maxByteSize cannot hold string, returns 0 and read head is not moved
    size_t ReadString( char* out_str, size_t maxByteSize ); // max_str_size should be enough to contain the null terminator as well;

    // HELPERS

    bool ReadAndWriteHeadsValid() const;

    bool CanGrow() const;

    bool OwnsMemory() const;

    void SetWriteHead( size_t byteCount );

    void SetReadHead( size_t byteCount );

    void OffsetWriteHead( int offset );

    void OffsetReadHead( int offset );

    void ResetWriteHead();  // resets writing to the beginning of the buffer.  Make sure read head stays valid (<= write_head)

    void ResetReadHead();   // resets reading to the beginning of the buffer

    Endianness GetEndianness() const;

    size_t GetWrittenByteCount() const;

    // how much more can I write to this buffer (if can grow,
    // this returns std::numeric_limits<size_t>::max())
    size_t GetWritableByteCount() const;

    size_t GetReadHead() const;

    size_t GetReadableByteCount() const;   // how much more data can I read;

    Byte* GetWriteHeadPtr();

    Byte* GetBuffer();

private:
    // Caution, may or may not own this memory, check BytePackerOptions
    Byte* m_buffer = nullptr;

    size_t m_readHead = 0;

    size_t m_writeHead = 0;

    size_t m_bufferSize = 0;

    Endianness m_endianness = Endianness::LITTLE;

    BytePackerOptions m_bytePackerOptions = 0;
};