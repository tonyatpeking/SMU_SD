#include "Engine/Core/Logger.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <stdarg.h>


Logger* Logger::GetDefault()
{
    static Logger* s_logger = nullptr;
    if( !s_logger )
        s_logger = new Logger();
    return s_logger;
}

void Logger::LoggerThreadWorker( Logger* logger )
{
    while( logger->IsRunning() )
    {
        logger->LogToHooks();
        Thread::SleepMS( 10 );
    }

    // do the inner side of the loop again right before exiting
    logger->LogToHooks();
    logger->FlushHooks();
}

void Logger::WriteToFile( LogEntry* entry, void* logFile )
{
    std::ofstream* file = ( std::ofstream* ) logFile;
    (*file) << entry->m_tag << ": " << entry->m_text << '\n';

}

void Logger::FlushFile( void* logFile )
{
    std::ofstream* file = ( std::ofstream* ) logFile;
    file->flush();
}

void Logger::StartUp()
{
    m_isRunning = true;
    m_thread = Thread::Create( Logger::LoggerThreadWorker, this );
}

void Logger::ShutDown()
{
    m_isRunning = false;
    Thread::Join( m_thread );
    m_thread = nullptr;
}

void Logger::LogToHooks()
{
    LogEntry *log;
    while( m_logQueue.Pop( &log ) )
    {
        m_logHookLock.lock_shared();
        for( auto& hook : m_logHooks )
        {
            hook.m_callback( log, hook.m_userArg );
        }
        m_logHookLock.unlock_shared();
        delete log;
    }
}

void Logger::FlushHooks()
{
    m_flushHookLock.lock_shared();
    for( auto& hook : m_flushHooks )
    {
        hook.m_callback( hook.m_userArg );
    }
    m_flushHookLock.unlock_shared();
}

void Logger::LogTaggedPrintfv( char const *tag, char const *format, va_list args )
{
    LogEntry* log = new LogEntry();
    log->m_tag = tag;
    log->m_text = Stringf( format, args );

    m_logQueue.Push( log );
}

void Logger::LogTaggedPrintf( char const *tag, char const *format, ... )
{
    va_list args;
    va_start( args, format );
    LogTaggedPrintfv( tag, format, args );
    va_end( args );
}

void Logger::AddLogHook( LogCB cb, void *userArg )
{
    m_logHookLock.lock();
    m_logHooks.push_back( LogHook( cb, userArg ) );
    m_logHookLock.unlock();
}

void Logger::AddFlushHook( FlushCB cb, void *userArg )
{
    m_flushHookLock.lock();
    m_flushHooks.push_back( FlushHook( cb, userArg ) );
    m_flushHookLock.unlock();
}

void Logger::AddFileHook( const String& filePath )
{

    // file
    std::ofstream* file = new std::ofstream();
    file->open( filePath );
    if( file->fail() )
    {
        delete file;
        file = nullptr;
    }
    else
    {
        AddLogHook( WriteToFile, (void*) file );
        AddFlushHook( FlushFile, (void*) file );
    }
}

void Logger::LogPrintf( char const *format, ... )
{
    va_list args;
    va_start( args, format );
    LogTaggedPrintfv( "Log", format, args );
    va_end( args );
}

void Logger::DebuggerPrintf( char const *format, ... )
{
    va_list args;
    va_start( args, format );
    LogTaggedPrintfv( "Debug", format, args );
    va_end( args );
}
