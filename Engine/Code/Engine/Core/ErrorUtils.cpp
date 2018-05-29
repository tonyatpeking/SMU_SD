#ifdef _WIN32
#define PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdarg.h>
#include <iostream>

#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Rgba.hpp"


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



// Converts a SeverityLevel to a Windows MessageBox icon type (MB_etc)
//
#if defined( PLATFORM_WINDOWS )
UINT GetWindowsMessageBoxIconFlagForSeverityLevel( SeverityLevel severity )
{
	switch( severity )
	{
		case SEVERITY_INFORMATION:  return MB_ICONASTERISK;	  //blue circle with 'i' in Win7
		case SEVERITY_QUESTION:	    return MB_ICONQUESTION;   //blue circle with '?' in Win7
		case SEVERITY_WARNING:		return MB_ICONEXCLAMATION;//yellow triangle with '!' in Win7
		case SEVERITY_FATAL:		return MB_ICONHAND;		  //red circle with 'x' in Win7
		default:					return MB_ICONEXCLAMATION;
	}
}
#endif



const char* FindStartOfFileNameWithinFilePath( const char* filePath )
{
	if( filePath == nullptr )
		return nullptr;

	size_t pathLen = strlen( filePath );
    // start with null terminator after last character
	const char* scan = filePath + pathLen;
	while( scan > filePath )
	{
		-- scan;

		if( *scan == '/' || *scan == '\\' )
		{
			++ scan;
			break;
		}
	}

	return scan;
}



void SystemDialogue_Okay(
    const String& messageTitle, const String& messageText, SeverityLevel severity )
{
	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( severity );
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
    const String& messageTitle, const String& messageText, SeverityLevel severity )
{
	bool isAnswerOkay = true;

	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( severity );
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
    const String& messageTitle, const String& messageText, SeverityLevel severity )
{
	bool isAnswerYes = true;

	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( severity );
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
    const String& messageTitle, const String& messageText, SeverityLevel severity )
{
	int answerCode = 1;

	#if defined( PLATFORM_WINDOWS )
	{
		ShowCursor( TRUE );
		UINT dialogueIconTypeFlag
            = GetWindowsMessageBoxIconFlagForSeverityLevel( severity );
		int buttonClicked = MessageBoxA(
            NULL, messageText.c_str(), messageTitle.c_str(),
            MB_YESNOCANCEL | dialogueIconTypeFlag | MB_TOPMOST );
		answerCode = (buttonClicked == IDYES ? 1 : (buttonClicked == IDNO ? 0 : -1) );
		ShowCursor( FALSE );
	}
	#endif

	return answerCode;
}


void Log(
    const char* filePath, const char* functionName, int lineNum,
    const String& messageText, SeverityLevel severity )
{
    const char* fileName = FindStartOfFileNameWithinFilePath( filePath );
    String fullMessageText = messageText;
    fullMessageText += Stringf( " line %i of %s, in %s()\n",
                                             lineNum, fileName, functionName );
    Rgba textColor = SeverityLevelToColor( severity );
    if( Console::DefaultConsole() )
        Console::DefaultConsole()->Print( fullMessageText, textColor );

    String severityString = SeverityToString( severity );
    DebuggerPrintf( "\n======================ERROR======================\n" );
    DebuggerPrintf( "RUN-TIME %s on line %i of %s, in %s()\n", severityString.c_str(),
                    lineNum, fileName, functionName );
    // Use this format so VS users can double-click to jump to file-and-line of error
    DebuggerPrintf( "%s(%d): %s\n", filePath, lineNum, messageText.c_str() );
}


Rgba SeverityLevelToColor( SeverityLevel severity )
{
    switch( severity )
    {
    case SEVERITY_INFORMATION:
        return Rgba::CYAN;
    case SEVERITY_QUESTION:
        return Rgba::GREEN_CYAN;
    case SEVERITY_WARNING:
        return Rgba::YELLOW;
    case SEVERITY_FATAL:
        return Rgba::RED;
    default:
        return Rgba::WHITE;
    }
}

String SeverityToString( SeverityLevel severity )
{
    switch( severity )
    {
    case SEVERITY_INFORMATION:
        return "INFORMATION";
    case SEVERITY_QUESTION:
        return "QUESTION";
    case SEVERITY_WARNING:
        return "WARNING";
    case SEVERITY_FATAL:
        return "FATAL ERROR";
    default:
        return "UNKNOWN ERROR";
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
            fullMessageTitle, fullMessageText, SEVERITY_FATAL );
		ShowCursor( TRUE );
		if( isAnswerYes )
		{
			__debugbreak();
		}
	}
	else
	{
		SystemDialogue_Okay( fullMessageTitle, fullMessageText, SEVERITY_FATAL );
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
            fullMessageTitle, fullMessageText, SEVERITY_WARNING );
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
            fullMessageTitle, fullMessageText, SEVERITY_WARNING );
		ShowCursor( TRUE );
		if( !isAnswerYes )
		{
			exit( 0 );
		}
	}
}


