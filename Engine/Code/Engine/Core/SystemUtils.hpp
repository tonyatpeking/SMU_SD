#pragma once
#include "Engine/Core/EngineCommonH.hpp"

namespace SystemUtils
{
string GetExePath();
bool SpawnProcess( const string& processPath, const string& commandLine = "" );
bool CloneProcess( const string& commandLine = "" );

};
