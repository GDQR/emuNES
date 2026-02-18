#include <stdio.h>
#include <windows.h>
#include "pipes.hpp"

int OpenWindow(){
    //  // additional information
    // STARTUPINFO si;     
    // PROCESS_INFORMATION pi;

    // // set the size of the structures
    // ZeroMemory( &si, sizeof(si) );
    // si.cb = sizeof(si);
    // ZeroMemory( &pi, sizeof(pi) );

    // LPCWSTR dab = L"F:\\Ed\\c-c++ Visual Studio\\emunes\\debugger.exe";
    // // start the program up
    // if( !CreateProcess(dab,   // No module name (use command line)
    //     NULL,        // Command line
    //     NULL,           // Process handle not inheritable
    //     NULL,           // Thread handle not inheritable
    //     FALSE,          // Set handle inheritance to FALSE
    //     0,              // No creation flags
    //     NULL,           // Use parent's environment block
    //     NULL,           // Use parent's starting directory 
    //     &si,            // Pointer to STARTUPINFO structure
    //     &pi )           // Pointer to PROCESS_INFORMATION structure
    // ) 
    // {
    //     printf( "CreateProcess failed (%ld).\n", GetLastError() );
    //     return 1;
    // }
    // // Close process and thread handles. 
    // CloseHandle( pi.hProcess );
    // CloseHandle( pi.hThread );
    // return 0;
}