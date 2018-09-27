#pragma once

#include <string>
#include "Engine/Core/Types.hpp"
namespace IOUtils
{
// returns in the following format, no slash at end
// "D:\\Guildhall\\p4\\C27\\Students\\hongjiny\\Projects\\SD0\\Protogame2D\\Run_Win32"
string GetCurrentDir();
string RelativeToFullPath( const string& relativePath );
// returns false for files
bool DirExists( const string& path );
bool FileExists( const string& path );
bool MakeDirR( const string& path );
bool MakeFileR( const string& path );

bool WriteToFile( const string& path, const string& text );
bool WriteToFile( const string& path, const Strings& text );
void* ReadFileToNewStringBuffer( char const* filename );
void* ReadFileToNewRawBuffer( char const* filename, size_t& out_byteCount );
string ReadFileToString( char const* filename );
Strings ReadFileToStrings( string& path );
bool CanOpenFile( char const* filename );
}