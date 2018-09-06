#pragma once
#include "Engine/Core/EngineCommonH.hpp"

namespace SystemUtils
{
String GetExePath();
bool SpawnProcess( const String& processPath, const String& commandLine = "" );
bool CloneProcess( const String& commandLine = "" );

};
