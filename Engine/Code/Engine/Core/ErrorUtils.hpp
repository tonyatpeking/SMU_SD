#pragma once

#include "Engine/Core/Types.hpp"
#include "Engine/Log/LogLevel.hpp"

class Rgba;
class Logger;
struct LogEntry;

void Log( const String& filePath, const String& functionName, int lineNum,
          LogLevel logLevel, const String& tag, const String& messageText );

void Log( const String& filePath, const String& functionName, int lineNum,
           LogLevel logLevel, const String& tag, const char* format, ... );

void Logfv( const String& filePath, const String& functionName, int lineNum,
            LogLevel logLevel, const String& tag, const char* format, va_list args );

void DebuggerLoggerCB( LogEntry* entry, void* _ );
void HookDebuggerToLogger( Logger* logger );

void DebuggerPrintf( const char* messageFormat, ... );

bool IsDebuggerAvailable();
__declspec( noreturn ) void FatalError(
    const char* filePath, const char* functionName, int lineNum,
    const String& reasonForError, const char* conditionText=nullptr );

void RecoverableWarning(
    const char* filePath, const char* functionName, int lineNum,
    const String& reasonForWarning, const char* conditionText=nullptr );

void SystemDialogue_Okay(
    const String& messageTitle, const String& messageText, LogLevel logLevel );

bool SystemDialogue_OkayCancel(
    const String& messageTitle, const String& messageText, LogLevel logLevel );

bool SystemDialogue_YesNo(
    const String& messageTitle, const String& messageText, LogLevel logLevel );

int SystemDialogue_YesNoCancel(
    const String& messageTitle, const String& messageText, LogLevel logLevel );


// ERROR_AND_DIE
//
// Present in all builds.
// No condition; always triggers if reached.
// Depending on the platform, this typically:
//	- Logs an error message to the console and/or log file
//	- Opens an error/message dialogue box
//	- Triggers a debug breakpoint (if appropriate development suite is present)
//	- Shuts down the app
//
// Use this when reaching a certain line of code should never happen ever,
// and continued execution is dangerous or impossible.
//
#define ERROR_AND_DIE( errorMessageText )												\
{																						\
	FatalError( __FILE__,  __FUNCTION__, __LINE__, errorMessageText );					\
}



// ERROR_RECOVERABLE
//
// Present in all builds.
// No condition; always triggers if reached.
// Depending on the platform, this typically:
//	- Logs a warning message to the console and/or log file
//	- Opens an warning/message dialogue box
//	- Triggers a debug breakpoint (if appropriate development suite is present)
//	- Continues execution
//
#define ERROR_RECOVERABLE( errorMessageText )											\
{																						\
	RecoverableWarning( __FILE__,  __FUNCTION__, __LINE__, errorMessageText );			\
}


// GUARANTEE_OR_DIE
//
// Present in all builds.
// Triggers if condition is false.
// Depending on the platform, this typically:
//	- Logs an error message to the console and/or log file
//	- Opens an error/message dialogue box
//	- Triggers a debug breakpoint (if appropriate development suite is present)
//	- Shuts down the app
//
#define GUARANTEE_OR_DIE( condition, errorMessageText )									\
{																						\
	if( !(condition) )																	\
	{																					\
		const char* text = #condition;											        \
		FatalError( __FILE__,  __FUNCTION__, __LINE__, errorMessageText, text );        \
	}																					\
}



// GUARANTEE_RECOVERABLE
//
// Present in all builds.
// Triggers if condition is false.
// Depending on the platform, this typically:
//	- Logs a warning message to the console and/or log file
//	- Opens an warning/message dialogue box
//	- Triggers a debug breakpoint (if appropriate development suite is present)
//	- Continues execution
//
#define GUARANTEE_RECOVERABLE( condition, errorMessageText )							\
{																						\
	if( !(condition) )																	\
	{																					\
		const char* text = #condition;													\
		RecoverableWarning( __FILE__,  __FUNCTION__, __LINE__, errorMessageText, text );\
	}																					\
}


// ASSERT_OR_DIE
//
// Removed if DISABLE_ASSERTS is defined, typically in a Final build configuration.
// Triggers if condition is false.
// Depending on the platform, this typically:
//	- Logs an error message to the console and/or log file
//	- Opens an error/message dialogue box
//	- Triggers a debug breakpoint (if appropriate development suite is present)
//	- Shuts down the app
//
#if defined( DISABLE_ASSERTS )
#define ASSERT_OR_DIE( condition, errorMessageText ) { (void)( condition ); }
#else
#define ASSERT_OR_DIE( condition, errorMessageText )									\
{																						\
	if( !(condition) )																	\
	{																					\
		const char* text = #condition;													\
		FatalError( __FILE__,  __FUNCTION__, __LINE__, errorMessageText, text );		\
	}																				    \
}
#endif


// ASSERT_RECOVERABLE
//
// Removed if DISABLE_ASSERTS is defined, typically in a Final build configuration.
// Triggers if condition is false.
// Depending on the platform, this typically:
//	- Logs a warning message to the console and/or log file
//	- Opens an warning/message dialogue box
//	- Triggers a debug breakpoint (if appropriate development suite is present)
//	- Continues execution
//
#if defined( DISABLE_ASSERTS )
#define ASSERT_RECOVERABLE( condition, errorMessageText ) { (void)( condition ); }
#else
#define ASSERT_RECOVERABLE( condition, errorMessageText )								\
{																						\
	if( !(condition) )																	\
	{																					\
		const char* text = #condition;													\
		RecoverableWarning( __FILE__,  __FUNCTION__, __LINE__, errorMessageText, text );\
	}																					\
}
#endif


#if defined( DISABLE_LOGGING )
#define LOG_TAG(  params ) { (void)( params ); }
#else
#define LOG_TAG( logLevel, tag, ... )\
{ Log(__FILE__,  __FUNCTION__, __LINE__, logLevel, tag , __VA_ARGS__ ); }
#endif


#define LOG( logLevel, ... ) { LOG_TAG( logLevel, "", __VA_ARGS__ );	}


#define LOG_INFO( ... ) { LOG(LOG_LEVEL_INFO, __VA_ARGS__); }
#define LOG_WARNING( ... ) { LOG(LOG_LEVEL_WARNING, __VA_ARGS__); }
#define LOG_ERROR( ... ) { LOG(LOG_LEVEL_ERROR, __VA_ARGS__); }
#define LOG_FATAL( ... ) { LOG(LOG_LEVEL_FATAL, __VA_ARGS__); }

#define LOG_INFO_TAG( tag, ... ) { LOG_TAG(LOG_LEVEL_INFO, tag, __VA_ARGS__); }
#define LOG_WARNING_TAG( tag, ... ) { LOG_TAG(LOG_LEVEL_WARNING, tag, __VA_ARGS__); }
#define LOG_ERROR_TAG( tag, ... ) { LOG_TAG(LOG_LEVEL_ERROR, tag, __VA_ARGS__); }
#define LOG_FATAL_TAG( tag, ... ) { LOG_TAG(LOG_LEVEL_FATAL, tag, __VA_ARGS__); }


#if defined( _DEBUG )
#define LOG_DEBUG( ... ) { LOG(LOG_LEVEL_DEBUG, __VA_ARGS__); }
#else
#define LOG_DEBUG( params ) { (void)( params ); }
#endif

#if defined( _DEBUG )
#define LOG_DEBUG_TAG( tag, ... ) { LOG_TAG(LOG_LEVEL_DEBUG, tag, __VA_ARGS__); }
#else
#define LOG_DEBUG_TAG( params ) { (void)( params ); }
#endif


#define LOG_ASSET_LOAD_FAILED( errorMessageText )\
{ LOG_WARNING( "Asset failed to load: [%s]", errorMessageText); }

#define LOG_ASSET_NOT_LOADED( errorMessageText )\
{ LOG_WARNING( "Trying to use asset that was not loaded: [%s]", errorMessageText); }

#define LOG_NULL_POINTER( errorMessageText )\
{ LOG_WARNING( "Using null pointer: [%s]", errorMessageText); }

#define LOG_INVALID_PARAMETERS( errorMessageText )\
{ LOG_WARNING( "Invalid parameters on: [%s]", errorMessageText); }


// "TODO" Macro
// Source from http://www.flipcode.com/archives/FIXME_TODO_Notes_As_Warnings_In_Compiler_Output.shtml
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// THE IMPORANT BITS
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
        " ------------------------------------------------------------------\n" \
        "|  TODO :   " ##x "\n" \
        " ------------------------------------------------------------------\n" )

// "UNIMPLEMENTED" Macro
#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" );\
ERROR_RECOVERABLE("UNIMPLEMENTED")





