#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <signal.h>

// attesting that calc.exe is winRT and breaks away from job objects
// G
// https://stackoverflow.com/questions/54105795/children-processes-created-in-asp-net-core-process-gets-killed-on-exit

// jobs: https://www.youtube.com/watch?v=2QRkNCrBrjI&t=579s
// process of parent: https://stackoverflow.com/questions/185254/how-can-a-win32-process-get-the-pid-of-its-parent

// nnoremap <silent> <leader>yRw <cmd>execute 'YcmCompleter RefactorRename' input( 'Rename to: ' )<CR>
// nnoremap <leader>yRw :YcmCompleter RefactorRename
void handle_signal(int sig) 
{
    fprintf(stderr, "ERROR: signal %d received\n", sig);
}
void print_last_error(DWORD error_number)
{
    // DWORD error_number = GetLastError();
    LPWSTR message_buffer = NULL;
    int msg_size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error_number,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&message_buffer,
            0,
            NULL
    );
    if (msg_size == 0)
    {
        wprintf(L"Failed to get error message\n");
    }
    wprintf(L"%s", message_buffer);
    LocalFree(message_buffer);
}

BOOL create_process(PROCESS_INFORMATION* pi, TCHAR* cmdline)
{
    STARTUPINFO si;
    // PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( pi, sizeof(*pi) );

    // Start the child process.

    // TCHAR cmdline[] = L"notepad.exe";
    // TCHAR cmdline[] = L"calc.exe";
    if( !CreateProcess(
        NULL,   // No module name (use command line)
        // cmdline,
        cmdline,
        // argv[1],       // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
        int error_number = GetLastError();
        wprintf(L"CreateProcess failed (%ld): ", error_number);
        print_last_error(error_number);
        wprintf(L"\n");
        return FALSE;
    }

    // Wait until child process exits.
    // WaitForSingleObject( pi->hProcess, INFINITE );

    // Close process and thread handles.
    // CloseHandle( pi.hProcess );
    // CloseHandle( pi.hThread );

    return TRUE;
}


int ppid(DWORD pid)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processentry = { 0 };
    processentry.dwSize = sizeof(PROCESSENTRY32);
    if ( !Process32First(hSnapshot, &processentry) )
    {
        printf("Process32First Failed (%ld)\n", GetLastError() );
        return -1;
    }
    DWORD ppid;
    BOOL failed = TRUE;
    do {
        if (processentry.th32ProcessID == pid)
        {
            ppid = processentry.th32ParentProcessID;
            failed = FALSE;
            printf("my PPID: (%ld)\n", ppid);
            break;
        };
    } while ( Process32Next(hSnapshot, &processentry) );
    if ( failed )
    {
        printf("Failed to get parent process. GetLastError=(%ld)\n", GetLastError() );
        return -1;
    };
    return ppid;
}

// #include "mingw-unicode.c"
// int _tmain( int argc, TCHAR *argv[] )
int main(void)
{
    signal(SIGSEGV, handle_signal);
    // if (argc < 2)
    // {
    //     printf("USAGE: create-job.exe PID");
    //     return 1;
    // };

    // GET PROCESS ID
    DWORD mypid = GetCurrentProcessId();
    printf("my PID: (%ld)\n", mypid);
    // DWORD myppid = ppid(mypid);

    // CREATE JOB OBJECT
    HANDLE hJob = CreateJobObject(NULL, _T("my job"));
    if (!hJob)
    {
        printf("CreateJobObject Failed (%ld)\n", GetLastError() );
        return 1;
    };
    // GET HANDLE FOR THIS PROCESS
    HANDLE hThisProcess = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, mypid);
    if ( !hThisProcess )
    {
        printf("Failed to get process handle. GetLastError=(%ld)\n", GetLastError() );
        return 1;
    }

    // ASSIGN THIS PROCESS TO JOB OBJECT
    if (!AssignProcessToJobObject(hJob, hThisProcess))
    {
        printf("Failed to assign process to job. GetLastError=(%ld)\n", GetLastError() );
        return 1;
    } else { printf("Assigned to job\n"); }

    // CREATE JOB LIMITS
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION info;
    // JOBOBJECT_BASIC_LIMIT_INFORMATION info;
    info.BasicLimitInformation.ActiveProcessLimit = 10;
    info.BasicLimitInformation.LimitFlags = 0;
    info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
    info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    wprintf(L"Job Object Limit Flags: 0x%X\n", info.BasicLimitInformation.LimitFlags);
    wprintf(L"Job Object Limit Flags: 0d%d\n", info.BasicLimitInformation.LimitFlags);
    if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &info, sizeof(info)))
    {
        printf("Failed to SetInformatinoJobObject. GetLastError=(%ld)\n", GetLastError() );
        return 1;
    } else {
        printf("SetInformationJobObject: Success!\n");
    };

    // AssignProcessToJobObject(hJob, hThisProcess);
    PROCESS_INFORMATION pi;
    TCHAR cmdline[] = L"cmd.exe /c \"start /wait cmd.exe\"";
    if (!create_process(&pi, cmdline)) {
        printf("Quitting!\n");
        return 1;
    } else { printf("success\n"); };
    Sleep(100000);
    // wprintf(L"KILLING!\n");
    // Sleep(500);
    // CloseHandle(hJob);
    wprintf(L"NOT KILLING\n");



    return 2;

}
