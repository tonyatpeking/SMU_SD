#pragma once

#include <string>
#include "Engine/Core/Types.hpp"
namespace IOUtils
{
// returns in the following format, no slash at end
// "D:\\Guildhall\\p4\\C27\\Students\\hongjiny\\Projects\\SD0\\Protogame2D\\Run_Win32"
String GetCurrentDir();
String RelativeToFullPath( const String& relativePath );
// returns false for files
bool DirExists( const String& path );
bool FileExists( const String& path );
bool MakeDirR( const String& path );
bool MakeFileR( const String& path );

bool WriteToFile( const String& path, const String& text );
bool WriteToFile( const String& path, const Strings& text );
void* ReadFileToNewStringBuffer( char const* filename );
void* ReadFileToNewRawBuffer( char const* filename, size_t& out_byteCount );
String ReadFileToString( char const* filename );
Strings ReadFileToStrings( String& path );
bool CanOpenFile( char const* filename );
}