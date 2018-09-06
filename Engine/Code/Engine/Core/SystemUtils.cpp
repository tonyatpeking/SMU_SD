#include "Engine/Core/SystemUtils.hpp"
#include "Engine/Core/EngineCommonC.hpp"
#include "Engine/Core/WindowsCommon.hpp"


String SystemUtils::GetExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA( NULL, buffer, MAX_PATH );
    return String( buffer );
}

bool SystemUtils::SpawnProcess( const String& processPath, const String& commandLine /*= "" */ )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof( pi ) );

    String pathAndCommand = ( processPath + " " + commandLine );
    std::wstring widestr = std::wstring( pathAndCommand.begin(), pathAndCommand.end() );

    wchar_t buffer[MAX_PATH];
    wcscpy_s( buffer, widestr.c_str() );


    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
                         buffer,        // Command line
                         NULL,           // Process handle not inheritable
                         NULL,           // Thread handle not inheritable
                         FALSE,          // Set handle inheritance to FALSE
                         0,              // No creation flags
                         NULL,           // Use parent's environment block
                         NULL,           // Use parent's starting directory
                         &si,            // Pointer to STARTUPINFO structure
                         &pi )           // Pointer to PROCESS_INFORMATION structure
        )
    {
        //printf( "CreateProcess failed (%d).\n", GetLastError() );
        return false;
    }

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return true;
}

bool SystemUtils::CloneProcess( const String& commandLine /*= "" */ )
{
    String path = GetExePath();
    return SpawnProcess( path, commandLine );
}
