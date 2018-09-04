#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <iostream>
#include <fstream>
#include <stdio.h>


#include "Engine/FileIO/IOUtils.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/String/StringUtils.hpp"

namespace IOUtils
{

const String SEPARATORS = "\\/";

String GetCurrentDir()
{
    char working_directory[MAX_PATH + 1];
    GetCurrentDirectoryA( sizeof( working_directory ), working_directory );
    return String( working_directory );
}

String RelativeToFullPath( const String& relativePath )
{
    int len = (int) relativePath.size();
    if( len > 1 )
    {
        char firstChar = relativePath[0];
        if( SEPARATORS.find( firstChar ) != std::string::npos )
            return GetCurrentDir() + relativePath;
    }

    return GetCurrentDir() + "/" + relativePath;

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

bool MakeDirR( const String& path )
{
    DWORD fileAttr = ::GetFileAttributesA( path.c_str() );

    if( fileAttr == INVALID_FILE_ATTRIBUTES )
    {
        std::size_t slashIdx = path.find_last_of( SEPARATORS );
        if( slashIdx != std::wstring::npos )
        {
            bool parentResult = MakeDirR( path.substr( 0, slashIdx ) );
            if( parentResult == false )
                return false;
        }

        BOOL result = ::CreateDirectoryA( path.c_str(), NULL );

        if( result == FALSE )
        {
            LOG_WARNING( "Could not create dir: " + path );
            return false;
        }
    }
    else
    {
        bool isDirectoryOrLink =
            ( ( fileAttr & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) ||
            ( ( fileAttr & FILE_ATTRIBUTE_REPARSE_POINT ) != 0 );

        if( !isDirectoryOrLink )
        {
            LOG_WARNING( "Could not create dir, file with same name exists: " + path );
            return false;
        }
    }

    return true;
}

bool MakeFileR( const String& path )
{
    DWORD fileAttr = ::GetFileAttributesA( path.c_str() );

    if( fileAttr == FILE_ATTRIBUTE_DIRECTORY )
    {
        LOG_WARNING( "Could not create file, dir with same name exists: " + path );
        return false;
    }

    if( fileAttr == INVALID_FILE_ATTRIBUTES )
    {
        std::size_t slashIdx = path.find_last_of( SEPARATORS );
        if( slashIdx != std::wstring::npos )
        {
            bool parentResult = MakeDirR( path.substr( 0, slashIdx ) );
            if( parentResult == false )
                return false;
        }
        std::ofstream myfile;
        myfile.open( path );
        if( myfile.fail() )
            return false;
        myfile.close();
        if( myfile.fail() )
            return false;
    }
    return true;
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
    if( MakeFileR( path ) == false )
        return false;

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

    Byte *buffer = (Byte*) malloc( size + 1U ); // space for NULL

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

    Byte *buffer = (Byte*) malloc( out_byteCount );

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

Strings ReadFileToStrings( String& path )
{
    String str = ReadFileToString( path.c_str() );
    Strings strs;
    StringUtils::ParseToLines( str, strs );
    return strs;
}

bool CanOpenFile( char const* filename )
{
    std::ifstream fileStream( filename );
    if( !fileStream.is_open() )
        return false;
    return true;
}

}