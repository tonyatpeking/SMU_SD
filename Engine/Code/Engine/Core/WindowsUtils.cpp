#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/WindowsUtils.hpp"


String WindowsUtils::ErrorCodeToString( int errorCode )
{

    LPSTR errString = NULL;  // will be allocated and filled by FormatMessage

    int size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
        0,       // 0 since source is internal message table
        errorCode, // this is the error code returned by WSAGetLastError()
                   // Could just as well have been an error code from generic
                   // Windows errors from GetLastError()
        0,       // auto-determine language to use
        (LPSTR) &errString, // this is WHERE we want FormatMessage
                            // to plunk the error string.  Note the
                            // peculiar pass format:  Even though
                            // errString is already a pointer, we
                            // pass &errString (which is really type LPSTR* now)
                            // and then CAST IT to (LPSTR).  This is a really weird
                            // trip up.. but its how they do it on msdn:
                            // http://msdn.microsoft.com/en-us/library/ms679351(VS.85).aspx
        0,                 // min size for buffer
        0 );               // 0, since getting message from system tables
    printf( "Error code %d:  %s\n\nMessage was %d bytes, in case you cared to know this.\n\n", errorCode, errString, size );

    String str = std::string( errString );
    StringUtils::RemoveOuterWhitespace( str );
    LocalFree( errString ); // if you don't do this, you will get an
                            // ever so slight memory leak, since we asked
                            // FormatMessage to FORMAT_MESSAGE_ALLOCATE_BUFFER,
                            // and it does so using LocalAlloc
                            // Gotcha!  I guess.

    return str;
}