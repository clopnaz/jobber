#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// https://learn.microsoft.com/en-us/windows/win32/procthread/creating-processes
// https://github.com/DerellLicht/unicons/blob/main/common.cpp
// https://github.com/coderforlife/mingw-unicode-main/blob/master/mingw-unicode.c
// https://superjamie.github.io/2022/08/06/ncursesw

// int _tmain( int argc, TCHAR *argv[] )
#include "mingw-unicode.c"
int _tmain( int argc, TCHAR *argv[] )
// int wmain( int argc, TCHAR *argv[] )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( argc != 2 )
    {
        wprintf(L"Usage: %s [cmdline]\n", argv[0]);
        return 1;
    }

    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
        argv[1],        // Command line
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
        wprintf( L"CreateProcess failed (%ld).\n", GetLastError() );
        return 1;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

}

