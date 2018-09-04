#include "Engine/DataUtils/Blob.hpp"
#include "Engine/FileIO/IOUtils.hpp"


void Blob::CopyData( size_t byteCount, const void* data )
{
    //reallocate if size has changed
    if( byteCount != m_byteCount )
    {
        ClearData();
        m_byteCount = byteCount;
        m_data =  (char*) malloc( byteCount );
    }
    memcpy( m_data, data, byteCount );
}

void Blob::StealData( size_t byteCount, void* data )
{
    ClearData();
    m_byteCount = byteCount;
    m_data = (char*) data;
}

void Blob::ClearData()
{
    free( m_data );
    m_byteCount = 0;
}

void Blob::FillFromFile( char const* filename )
{
    m_data = (char *) IOUtils::ReadFileToNewRawBuffer( filename, m_byteCount );
}

Blob::~Blob()
{
    ClearData();
}
