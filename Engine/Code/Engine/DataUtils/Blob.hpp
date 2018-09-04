#pragma once
#include "Engine/Core/Types.hpp"

class Blob
{
public:
    Blob() {};
    void CopyData( size_t byteCount, const void* data );
    void StealData( size_t byteCount, void* data );
    void ClearData();
    void FillFromFile( char const* filename );
    ~Blob();

    char* m_data = nullptr;
    size_t m_byteCount = 0;
};
