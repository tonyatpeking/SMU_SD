#include "Engine/DataUtils/BytePacker.hpp"
#include "Engine/Core/EngineCommonC.hpp"

BytePacker::BytePacker()
{
    m_bytePackerOptions = BYTEPACKER_OWNS_MEMORY | BYTEPACKER_CAN_GROW;

    m_bufferSize = 0;
    m_buffer = nullptr;
}

BytePacker::BytePacker( size_t bufferSize )
{
    m_bytePackerOptions = BYTEPACKER_OWNS_MEMORY;

    m_bufferSize = bufferSize;
    m_buffer = (Byte*) malloc( bufferSize );
}

BytePacker::BytePacker( size_t bufferSize, void* buffer )
{
    m_bytePackerOptions = 0;

    m_bufferSize = bufferSize;
    m_buffer = (Byte*) buffer;
}

BytePacker::~BytePacker()
{
    if( OwnsMemory() )
        free( m_buffer );
}

bool BytePacker::Reserve( size_t byteCount )
{
    if( !CanGrow() || byteCount <= m_bufferSize )
        return false;

    // realloc works if m_buffer is nullptr so no need to check
    void* m_newBuffer = realloc( m_buffer, byteCount );

    if( m_newBuffer == nullptr )
    {
        return false;
    }

    m_buffer = (Byte*) m_newBuffer;
    m_bufferSize = byteCount;
    return true;
}

void BytePacker::SetEndianness( Endianness byteOrder )
{
    m_endianness = byteOrder;
}

bool BytePacker::WriteBytes( size_t byteCount, const void* data )
{
    bool hasRoom = m_bufferSize - m_writeHead > byteCount;
    hasRoom = hasRoom && ReadAndWriteHeadsValid();
    if( !hasRoom )
    {
        if( !OwnsMemory() )
        {
            return false;
        }
        else
        {
            bool reserveSuccess = Reserve( GetWrittenByteCount() + byteCount );
            if( !reserveSuccess )
            {
                return false;
            }
        }
    }

    memcpy( m_buffer + m_writeHead, data, byteCount );
    m_writeHead += byteCount;
    return true;
}

size_t BytePacker::ReadBytes( void* out_Data, size_t maxByteCount )
{
    size_t bytesToRead = Min( maxByteCount, GetReadableByteCount() );

    memcpy( out_Data, m_buffer + m_readHead, bytesToRead );
    m_readHead += bytesToRead;
    return bytesToRead;
}

size_t BytePacker::CopyFrom( BytePacker& copyFrom, size_t maxBytesToCopy )
{
    size_t actualBytesToCopy = Min( maxBytesToCopy, copyFrom.GetReadableByteCount() );
    actualBytesToCopy = Min( actualBytesToCopy, GetWritableByteCount() );
    WriteBytes( actualBytesToCopy, copyFrom.GetReadHeadPtr() );
    copyFrom.OffsetReadHead( (int)actualBytesToCopy );
    return actualBytesToCopy;
}

// Size Header internal
namespace
{
Byte s_bitMaskLowest7 = 0b1111111;
int s_shiftAmount = 7;
Byte s_bitMaskHighest = 0b10000000;
}

size_t BytePacker::WriteLengthHeader( size_t length )
{
    if( length == 0 )
    {
        WriteBytes( 1, &length );
        return 1;
    }

    size_t bytesUsed = 0;
    while( length != 0 )
    {
        Byte byteToWrite = (Byte) ( length & s_bitMaskLowest7 );
        length = length >> s_shiftAmount;

        if( length != 0 )
        {
            byteToWrite |= s_bitMaskHighest;
        }

        WriteBytes( 1, &byteToWrite );
        ++bytesUsed;
    }

    return bytesUsed;
}

size_t BytePacker::ReadLengthHeader( size_t& out_length )
{
    out_length = 0;
    size_t byteCount = 0;
    bool moreToRead = true;

    do
    {
        Byte b;
        size_t bytesRead = ReadBytes( &b, 1 );
        if( bytesRead != 1 )
        {
            LOG_WARNING( "expected 1 byte to be read" );
        }
        moreToRead = ( 0 != ( b & s_bitMaskHighest ) );
        // clear highest bit
        b &= s_bitMaskLowest7;
        // shift b
        size_t bShift = (size_t) b << ( byteCount * s_shiftAmount );

        out_length |= bShift;

        ++byteCount;


    } while( moreToRead );

    return byteCount;
}

bool BytePacker::WriteString( const char* str )
{
    size_t len = strlen( str );
    WriteLengthHeader( len );
    return WriteBytes( len, str );
}

size_t BytePacker::ReadString( char* out_str, size_t maxByteSize )
{
    size_t len;
    size_t lenBytes;
    lenBytes = ReadLengthHeader( len );
    if( len + 1 > maxByteSize ) // + 1 for '\0' at end
    {
        LOG_WARNING( "too much data to fit string buffer" );
        OffsetReadHead( -(int) lenBytes );
        return 0;
    }

    size_t readByteCount = ReadBytes( out_str, len );
    if( readByteCount != len )
    {
        LOG_WARNING( "String length did not match Length header" );
    }

    out_str[readByteCount] = '\0';
    return readByteCount;
}

void BytePacker::ReadString( string& out_str )
{
    char strBuffer[BYTE_PACKER_MAX_STRING_LENGTH];
    ReadString( strBuffer, BYTE_PACKER_MAX_STRING_LENGTH );
    out_str = string( strBuffer );
}

bool BytePacker::ReadAndWriteHeadsValid() const
{
    return m_readHead <= m_writeHead && m_writeHead <= m_bufferSize;
}

bool BytePacker::CanGrow() const
{
    return ( m_bytePackerOptions & BYTEPACKER_CAN_GROW ) != 0;
}

bool BytePacker::OwnsMemory() const
{
    return ( m_bytePackerOptions & BYTEPACKER_OWNS_MEMORY ) != 0;
}

void BytePacker::SetWriteHead( size_t byteCount )
{
    m_writeHead = byteCount;
    if( m_readHead > m_writeHead )
        m_readHead = m_writeHead;
}

void BytePacker::SetReadHead( size_t byteCount )
{
    m_readHead = byteCount;
}

void BytePacker::OffsetWriteHead( int offset )
{
    m_writeHead += (size_t) offset;
}

void BytePacker::OffsetReadHead( int offset )
{
    m_readHead += (size_t) offset;
}

void BytePacker::ResetWriteHead()
{
    m_writeHead = 0;
}

void BytePacker::ResetReadHead()
{
    m_readHead = 0;
}

Endianness BytePacker::GetEndianness() const
{
    return m_endianness;
}

size_t BytePacker::GetWrittenByteCount() const
{
    return m_writeHead;
}

size_t BytePacker::GetWritableByteCount() const
{
    if( CanGrow() )
        return MAX_SIZE_T;

    if( !ReadAndWriteHeadsValid() )
    {
        LOG_WARNING( "Invalid write head" );
        return 0;
    }
    return m_bufferSize - m_writeHead;
}

size_t BytePacker::GetReadHead() const
{
    return m_readHead;
}

size_t BytePacker::GetReadableByteCount() const
{
    if( !ReadAndWriteHeadsValid() )
    {
        LOG_WARNING( "Invalid read head" );
        return 0;
    }
    return m_writeHead - m_readHead;
}

const Byte* BytePacker::GetReadHeadPtr() const
{
    return m_buffer + m_readHead;
}

Byte* BytePacker::GetWriteHeadPtr()
{
    return m_buffer + m_writeHead;
}

Byte* BytePacker::GetBuffer()
{
    return m_buffer;
}

const Byte* BytePacker::GetBuffer() const
{
    return m_buffer;
}

string BytePacker::ToString() const
{
    return StringUtils::ToHex(m_buffer,m_writeHead);
}
