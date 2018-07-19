#pragma once
#include "Engine/Core/Thread.hpp"
#include "Engine/Core/ThreadSafeQueue.hpp"
#include "Engine/Core/LogEntry.hpp"
#include <shared_mutex>
#include <functional>
#include <iostream>
#include <fstream>

typedef std::function<void( LogEntry*, void* )> LogCB;
typedef std::function<void( void* )> FlushCB;

struct LogHook
{
public:
    LogHook( LogCB cb, void* args )
        : m_callback( cb )
        , m_userArg( args ) {}
    LogCB m_callback;
    void* m_userArg;
};

struct FlushHook
{
public:
    FlushHook( FlushCB cb, void* args )
        : m_callback( cb )
        , m_userArg( args ) {}
    FlushCB m_callback;
    void* m_userArg;
};



class Logger
{
public:
	Logger(){};
	~Logger(){};
    static Logger* GetDefault();
    static void LoggerThreadWorker( Logger* logger );
    static void WriteToFile( LogEntry* entry, void* logFile );
    static void FlushFile( void* logFile );

    void StartUp();
    void ShutDown();
    bool IsRunning() { return m_isRunning; };
    void LogToHooks();
    void FlushHooks();
    void LogTaggedPrintfv( char const *tag, char const *format, va_list args );
    void LogTaggedPrintf( char const *tag, char const *format, ... );
    void AddLogHook( LogCB cb, void *userArg );
    void AddFlushHook( FlushCB cb, void *userArg );
    void AddFileHook( const String& filePath );

    void LogPrintf( char const *format, ... );
    void DebuggerPrintf( char const *format, ... );
private:
    bool m_isRunning = false;
    Thread::Handle m_thread = nullptr;
    ThreadSafeQueue<LogEntry*> m_logQueue;

    std::shared_mutex m_logHookLock;
    std::vector<LogHook> m_logHooks;
    std::vector<FlushHook> m_flushHooks;
    std::shared_mutex m_flushHookLock;

};
