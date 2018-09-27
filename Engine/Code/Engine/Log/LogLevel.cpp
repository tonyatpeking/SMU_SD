#include "Engine/Log/LogLevel.hpp"
#include "Engine/Core/Rgba.hpp"


Rgba LogLevelToColor( LogLevel logLevel )
{
    switch( logLevel )
    {
    case LOG_LEVEL_DEBUG:
        return Rgba::CYAN;
    case LOG_LEVEL_INFO:
        return Rgba::WHITE;
    case LOG_LEVEL_WARNING:
        return Rgba::YELLOW;
    case LOG_LEVEL_ERROR:
        return Rgba::ORANGE;
    case LOG_LEVEL_FATAL:
        return Rgba::RED;
    default:
        return Rgba::WHITE;
    }
}

string LogLevelToString( LogLevel logLevel )
{
    switch( logLevel )
    {
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
    case LOG_LEVEL_INFO:
        return "INFO";
    case LOG_LEVEL_WARNING:
        return "WARNING";
    case LOG_LEVEL_ERROR:
        return "ERROR";
    case LOG_LEVEL_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN LOG LEVEL";
    }
}
