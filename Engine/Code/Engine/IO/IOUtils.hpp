#pragma once

#include <string>
#include "Engine/Core/Types.hpp"
namespace IOUtils
{

// returns in the following format
// "D:\\Guildhall\\p4\\C27\\Students\\hongjiny\\Projects\\SD0\\Protogame2D\\Run_Win32"
String GetCurrentDir();
bool WriteToFile( const String& path, const String& text );
bool WriteToFile( const String& path, const Strings& text );
void* ReadFileToNewStringBuffer( char const* filename );
void* ReadFileToNewRawBuffer( char const* filename, size_t& out_byteCount );
String ReadFileToString( char const* filename );
bool CanOpenFile( char const* filename );
}