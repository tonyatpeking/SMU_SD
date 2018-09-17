#ifdef _WIN32
#define PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdarg.h>
#include <iostream>

#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Log/Logger.hpp"
#include "Engine/Log/LogEntry.hpp"


void Log( const String& filePath, const String& functionName, int lineNum,
          LogLevel logLevel, const String& tag, const String& messageText )
{
    Logger::GetDefault()->Log( filePath, functionName, lineNum, logLevel, tag, messageText );
}


void Log( const String& filePath, const String& functionName, int lineNum,
           LogLevel logLevel, const String& tag, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    Logfv( filePath, functionName, lineNum, logLevel, tag, format, args );
    va_end( args );
}


void Logfv( const String& filePath, const String& functionName, int lineNum,
            LogLevel logLevel, const String& tag, const char* format, va_list args )
{
    Log( filePath, functionName, lineNum, logLevel, tag, Stringf( format, args ) );
}
//
//  void Log(
//      const String& filePath, const String& functionName, int lineNum,
//      LogLevel logLevel, const String& tag,
//      const String& messageText )
//  {
//       String fullMessageText = messageText;
//       fullMessageText += Stringf( " line %i of %s, in %s()\n",
//                                                lineNum, fileName, functionName );
//       Rgba textColor = LogLevelToColor( logLevel );
//       if( Console::GetDefault() )
//           Console::GetDefault()->Print( fullMessageText, textColor );
//
//
//      String logLevelString = LogLevelToString( logLevel );
//
//      Logger::GetDefault()->LogTaggedPrintf( logLevelString.c_str(), fullMessageText.c_str() );
//       DebuggerPrintf( "\n======================ERROR======================\n" );
//       DebuggerPrintf( "RUN-TIME %s on line %i of %s, in %s()\n", logLevelString.c_str(),
//                       lineNum, fileName, functionName );
//       // Use this format so VS users can double-click to jump to file-and-line of error
//       DebuggerPrintf( "%s(%d): %s\n", filePath, lineNum, messageText.c_str() );
//  }



const char* FindStartOfFileNameWithinFilePath( const char* filePath )
{
    if( filePath == nullptr )
        return nullptr;

    size_t pathLen = strlen( filePath );
    // start with null terminator after last character
    const char* scan = filePath + pathLen;
    while( scan > filePath )
    {
        --scan;

        if( *scan == '/' || *scan == '\\' )
        {
            ++scan;
            break;
        }
    }
    return scan;
}

void DebuggerLoggerCB( LogEntry* entry, void* _ )
{
    UNUSED( _ );
    if( entry->m_level < LOG_LEVEL_WARNING )
        return;
    const char* fileName = FindStartOfFileNameWithinFilePath( entry->m_file.c_str() );
    String logLevelStr = LogLevelToString( entry->m_level );

    DebuggerPrintf( "\n====================== %s ======================\n", logLevelStr.c_str() );
    DebuggerPrintf( "RUN-TIME %s on line %i of %s, in %s()\n", logLevelStr.c_str(),
                    entry->m_line, fileName, entry->m_function.c_str() );
    // Use this format so VS users can double-click to jump to file-and-line of error
    DebuggerPrintf( "%s(%d): %s\n", entry->m_file.c_str(), entry->m_line, entry->m_text.c_str());
}

void HookDebuggerToLogger( Logger* logger )
{
    logger->AddLogHook( DebuggerLoggerCB, 0 );
}

bool IsDebuggerAvailable()
{
#if defined( PLATFORM_WINDOWS )
	typedef BOOL (CALLBACK IsDebuggerPresentFunc)();

	// Get a handle to KERNEL32.DLL
	static HINSTANCE hInstanceKernel32 = GetModuleHandle( TEXT( "KERNEL32" ) );
	if( !hInstanceKernel32 )
		return false;

	// Get a handle to the IsDebuggerPresent() function in KERNEL32.DLL
	static IsDebuggerPresentFunc* isDebuggerPresentFunc
        = (IsDebuggerPresentFunc*) GetProcAddress(
            hInstanceKernel32, "IsDebuggerPresent" );
	if( !isDebuggerPresentFunc )
		return false;

	// Now CALL that function and return its result
	static BOOL isDebuggerAvailable = isDebuggerPresentFunc();
	return( isDebuggerAvailable == TRUE );
#else
	return false;
#endif
}



void DebuggerPrintf( const char* messageFormat, ... )
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, messageFormat );
	vsnprintf_s(
        messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat,
        variableArgumentList );
	va_end( variableArgumentList );
    // In case vsnprintf overran (doesn't auto-terminate)
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0';

#if defined( PLATFORM_WINDOWS )
	if( IsDebuggerAvailable() )
	{
		OutputDebugStringA( messageLiteral );
	}
#endif

	std::cout << messageLiteral;
}



// Converts a LogLevel to a Windows MessageBox icon type (MB_etc)
//
#if defined( PLATFORM_WINDOWS )
UINT GetWindowsMessageBoxIconFlagForSeverityLevel( LogLevel logLevel )
{
	switch( logLevel )
	{
        case LOG_LEVEL_DEBUG:       return MB_ICONASTERISK;	  //blue circle with 'i' in Win7
        case LOG_LEVEL_INFO:        return MB_ICONASTERISK;	  //blue circle with 'i' in Win7
		case LOG_LEVEL_WARNING:		return MB_ICONEXCLAMATION;//yellow triangle with '!' in Win7
        case LOG_LEVEL_ERROR:		return MB_ICONHAND;		  //red circle with 'x' in Win7
        case LOG_LEVEL_FATAL:		return MB_ICONHAND;		  //red circle with 'x' in Win7
		default:					return MB_ICONEXCLAMATION;
	}
}
#endif





void SystemDialogue_Okay(
    const String& messageTitle, const String& messageText, LogLevel logLevel )
{
	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( logLevel );
		MessageBoxA(
            NULL, messageText.c_str(), messageTitle.c_str(),
            MB_OK | dialogueIconTypeFlag | MB_TOPMOST );
		ShowCursor( FALSE );
	}
	#endif
}



// Returns true if OKAY was chosen, false if CANCEL was chosen.
//
bool SystemDialogue_OkayCancel(
    const String& messageTitle, const String& messageText, LogLevel logLevel )
{
	bool isAnswerOkay = true;

	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( logLevel );
		int buttonClicked = MessageBoxA(
            NULL, messageText.c_str(), messageTitle.c_str(),
            MB_OKCANCEL | dialogueIconTypeFlag | MB_TOPMOST );
		isAnswerOkay = (buttonClicked == IDOK);
		ShowCursor( FALSE );
	}
	#endif

	return isAnswerOkay;
}



// Returns true if YES was chosen, false if NO was chosen.
//
bool SystemDialogue_YesNo(
    const String& messageTitle, const String& messageText, LogLevel logLevel )
{
	bool isAnswerYes = true;

	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( logLevel );
		int buttonClicked = MessageBoxA(
            NULL, messageText.c_str(), messageTitle.c_str(),
            MB_YESNO | dialogueIconTypeFlag | MB_TOPMOST );
		isAnswerYes = (buttonClicked == IDYES);
		ShowCursor( FALSE );
	}
	#endif

	return isAnswerYes;
}



// Returns 1 if YES was chosen, 0 if NO was chosen, -1 if CANCEL was chosen.
//
int SystemDialogue_YesNoCancel(
    const String& messageTitle, const String& messageText, LogLevel logLevel )
{
	int answerCode = 1;

	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( logLevel );
		int buttonClicked = MessageBoxA(
            NULL, messageText.c_str(), messageTitle.c_str(),
            MB_YESNOCANCEL | dialogueIconTypeFlag | MB_TOPMOST );
		answerCode = (buttonClicked == IDYES ? 1 : (buttonClicked == IDNO ? 0 : -1) );
		ShowCursor( FALSE );
	}
	#endif

	return answerCode;
}





void AssertBreakpoint( bool condition )
{
    if( !condition && IsDebuggerAvailable() )
    {
        __debugbreak();
    }
}

__declspec( noreturn ) void FatalError(
    const char* filePath, const char* functionName, int lineNum,
    const String& reasonForError, const char* conditionText )
{
	String errorMessage = reasonForError;
	if( reasonForError.empty() )
	{
		if( conditionText )
			errorMessage = Stringf( "ERROR: \"%s\" is false!", conditionText );
		else
			errorMessage = "Unspecified fatal error";
	}

	const char* fileName = FindStartOfFileNameWithinFilePath( filePath );
//	String appName = theApplication ? theApplication->GetApplicationName() : "Unnamed Application";
	String appName = "Unnamed Application";
	String fullMessageTitle = appName + " :: Error";
	String fullMessageText = errorMessage;
	fullMessageText += "\n\nThe application will now close.\n";
	bool isDebuggerPresent = (IsDebuggerPresent() == TRUE);
	if( isDebuggerPresent )
	{
		fullMessageText += "\nDEBUGGER DETECTED!\nWould you like to break and debug?\n  (Yes=debug, No=quit)\n";
	}

	fullMessageText += "\n---------- Debugging Details Follow ----------\n";
	if( conditionText )
	{
		fullMessageText += Stringf( "\nThis error was triggered by a run-time condition check:\n  %s\n  from %s(), line %i in %s\n",
			conditionText, functionName, lineNum, fileName );
	}
	else
	{
		fullMessageText += Stringf( "\nThis was an unconditional error triggered by reaching\n line %i of %s, in %s()\n",
			lineNum, fileName, functionName );
	}

	DebuggerPrintf( "\n======================ERROR======================\n" );
	DebuggerPrintf( "RUN-TIME FATAL ERROR on line %i of %s, in %s()\n",
                    lineNum, fileName, functionName );
	DebuggerPrintf( "%s(%d): %s\n", filePath, lineNum, errorMessage.c_str() );

	if( isDebuggerPresent )
	{
		bool isAnswerYes = SystemDialogue_YesNo(
            fullMessageTitle, fullMessageText, LOG_LEVEL_FATAL );
		ShowCursor( TRUE );
		if( isAnswerYes )
		{
			__debugbreak();
		}
	}
	else
	{
		SystemDialogue_Okay( fullMessageTitle, fullMessageText, LOG_LEVEL_FATAL );
		ShowCursor( TRUE );
	}

	exit( 0 );
}



void RecoverableWarning(
    const char* filePath, const char* functionName, int lineNum,
    const String& reasonForWarning, const char* conditionText )
{
	String errorMessage = reasonForWarning;
	if( reasonForWarning.empty() )
	{
		if( conditionText )
			errorMessage = Stringf( "WARNING: \"%s\" is false!", conditionText );
		else
			errorMessage = "Unspecified warning";
	}

	const char* fileName = FindStartOfFileNameWithinFilePath( filePath );
//	String appName = theApplication ? theApplication->GetApplicationName() : "Unnamed Application";
	String appName = "Unnamed Application";
	String fullMessageTitle = appName + " :: Warning";
	String fullMessageText = errorMessage;

	bool isDebuggerPresent = (IsDebuggerPresent() == TRUE);
	if( isDebuggerPresent )
	{
		fullMessageText += "\n\nDEBUGGER DETECTED!\nWould you like to continue running?\n  (Yes=continue, No=quit, Cancel=debug)\n";
	}
	else
	{
		fullMessageText += "\n\nWould you like to continue running?\n  (Yes=continue, No=quit)\n";
	}

	fullMessageText += "\n---------- Debugging Details Follow ----------\n";
	if( conditionText )
	{
		fullMessageText += Stringf( "\nThis warning was triggered by a run-time condition check:\n  %s\n  from %s(), line %i in %s\n",
			conditionText, functionName, lineNum, fileName );
	}
	else
	{
		fullMessageText += Stringf( "\nThis was an unconditional warning triggered by reaching\n line %i of %s, in %s()\n",
			lineNum, fileName, functionName );
	}

	DebuggerPrintf( "\n-----------------------WARNING-----------------------\n" );
	DebuggerPrintf( "RUN-TIME RECOVERABLE WARNING on line %i of %s, in %s()\n",
                    lineNum, fileName, functionName );
	DebuggerPrintf( "%s(%d): %s\n", filePath, lineNum, errorMessage.c_str() );

	if( isDebuggerPresent )
	{
		int answerCode = SystemDialogue_YesNoCancel(
            fullMessageTitle, fullMessageText, LOG_LEVEL_WARNING );
		ShowCursor( TRUE );
		if( answerCode == 0 ) // "NO"
		{
			exit( 0 );
		}
		else if( answerCode == -1 ) // "CANCEL"
		{
			__debugbreak();
		}
	}
	else
	{
		bool isAnswerYes = SystemDialogue_YesNo(
            fullMessageTitle, fullMessageText, LOG_LEVEL_WARNING );
		ShowCursor( TRUE );
		if( !isAnswerYes )
		{
			exit( 0 );
		}
	}
}


