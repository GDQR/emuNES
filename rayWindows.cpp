#include <stdio.h>
#include <windows.h>
#include "rayWindows.hpp"
static PROCESS_INFORMATION pi;
int OpenWindow(){
     // additional information
    STARTUPINFO si;     

    // set the size of the structures
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    LPCWSTR dab = L"F:\\Ed\\c-c++ Visual Studio\\emunes\\debugger.exe";
    // start the program up
    if( !CreateProcess(dab,   // No module name (use command line)
        NULL,        // Command line
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
        printf( "CreateProcess failed (%ld).\n", GetLastError() );
        return 1;
    }
    // Close process and thread handles. 
    // CloseHandle( pi.hProcess );
    // CloseHandle( pi.hThread );
    return 0;
}

void closeDebuggerWindow(){
    // Close process and thread handles. 
    TerminateProcess(pi.hProcess, 0);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

static HANDLE mainHMutex;
int CreateMainWindow(){
// Create a named Mutex
    mainHMutex = CreateMutex(NULL, TRUE, L"aesmiapp");
    if (mainHMutex == NULL) {
        printf("CreateMutex error: %ld\n", GetLastError());
        return 1;
    }

    // Check if the mutex already exists
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("Another instance is already running.\n");
        // Optional: find the existing window and bring it to the foreground
        HWND existingApp = FindWindow(0, L"emunes");
        if (existingApp) SetForegroundWindow(existingApp);
        // CloseHandle(mainHMutex); // Close the handle for the second instance
        return 1; // Exit the new instance
    }
    return 0;
}
void CloseMainWindow(){
        CloseHandle(mainHMutex);
}