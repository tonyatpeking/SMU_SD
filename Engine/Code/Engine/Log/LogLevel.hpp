#pragma once

#include "Engine/Core/EngineCommonH.hpp"

class Rgba;


enum LogLevel
{
    LOG_LEVEL_NOTSET = 0,
    LOG_LEVEL_DEBUG = 10,
    LOG_LEVEL_INFO = 20,
    LOG_LEVEL_WARNING = 30,
    LOG_LEVEL_ERROR = 40,
    LOG_LEVEL_FATAL = 50
};

Rgba LogLevelToColor( LogLevel logLevel );

String LogLevelToString( LogLevel logLevel );