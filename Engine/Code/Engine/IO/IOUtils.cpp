#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <fstream>
#include <stdio.h>


#include "Engine/IO/IOUtils.hpp"



namespace IOUtils
{

String GetCurrentDir()
{
    char working_directory[MAX_PATH + 1];
    GetCurrentDirectoryA( sizeof( working_directory ), working_directory );
    return String( working_directory );
}

bool DirExists( const String& path )
{
    DWORD attrib = GetFileAttributesA( path.c_str() );

    return ( attrib != INVALID_FILE_ATTRIBUTES
             && ( attrib & FILE_ATTRIBUTE_DIRECTORY ) );
}

bool FileExists( const String& path )
{
    DWORD attrib = GetFileAttributesA( path.c_str() );
    // if last error was not ERROR_FILE_NOT_FOUND then it exists
    // but we don't have access to the file
    if( INVALID_FILE_ATTRIBUTES == attrib && GetLastError() == ERROR_FILE_NOT_FOUND )
    {
        //File not found
        return false;
    }
    return true;
}

bool MakeDir( const String& path )
{
    return CreateDirectoryA( path.c_str(), NULL );
}

bool WriteToFile( const String& path, const String& text )
{
    Strings strings;
    strings.push_back( text );
    WriteToFile( path, strings );
    return true;
}

bool WriteToFile( const String& path, const Strings& text )
{
    std::ofstream myfile;
    myfile.open( path );
    if( myfile.fail() )
        return false;
    for( auto& line : text )
    {
        myfile << line << '\n';
        if( myfile.fail() )
        {
            myfile.close();
            return false;
        }
    }
    myfile.close();
    if( myfile.fail() )
        return false;
    return true;
}

void* ReadFileToNewStringBuffer( char const* filename )
{
    FILE *fp = nullptr;
    fopen_s( &fp, filename, "r" );
    if( fp == nullptr )
        return nullptr;

    size_t size = 0U;

    fseek( fp, 0L, SEEK_END );
    size = ftell( fp );

    fseek( fp, 0L, SEEK_SET );

    byte_t *buffer = (byte_t*) malloc( size + 1U ); // space for NULL

    size_t read = fread( buffer, 1, size, fp );
    fclose( fp );

    buffer[read] = NULL;
    return buffer;
}

void* ReadFileToNewRawBuffer( char const* filename, size_t& out_byteCount )
{
    FILE *fp = nullptr;
    fopen_s( &fp, filename, "r" );
    out_byteCount = 0U;
    if( fp == nullptr )
        return nullptr;

    fseek( fp, 0L, SEEK_END );
    out_byteCount = ftell( fp );

    fseek( fp, 0L, SEEK_SET );

    byte_t *buffer = (byte_t*) malloc( out_byteCount );

    fread( buffer, 1, out_byteCount, fp );
    fclose( fp );

    return buffer;
}

String ReadFileToString( char const* filename )
{
    std::ifstream fileStream( filename );
    if( !fileStream.is_open() )
        return "";
    fileStream.seekg( 0, std::ios::end );
    size_t size = fileStream.tellg();
    String buffer( size, ' ' );
    fileStream.seekg( 0 );
    fileStream.read( &buffer[0], size );
    return buffer;
}

bool CanOpenFile( char const* filename )
{
    std::ifstream fileStream( filename );
    if( !fileStream.is_open() )
        return false;
    return true;
}

}