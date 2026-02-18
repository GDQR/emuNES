#include <stdio.h>
#include <windows.h>
#include "pipes.hpp"

#if defined(_WIN32) || defined(_WIN64)
    HANDLE serverPipe;
    char buffer[1024];
    DWORD dwRead;

    HANDLE clientPipe;
    DWORD dwWritten;
#endif

void pipeServer(){
    #if defined(_WIN32) || defined(_WIN64)
        // HANDLE hPipe;
        // char buffer[1024];
        // DWORD dwRead;


        serverPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\emunes"),
                                PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
                                1,
                                1024 * 16,
                                1024 * 16,
                                NMPWAIT_USE_DEFAULT_WAIT,
                                NULL);
        // while (hPipe != INVALID_HANDLE_VALUE)
        // {
        //     if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
        //     {
        //         while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
        //         {
        //             /* add terminating zero */
        //             buffer[dwRead] = '\0';

        //             /* do something with data in buffer */
        //             printf("%s", buffer);
        //         }
        //     }

        //     DisconnectNamedPipe(hPipe);
        // }
    #elif defined(__linux__)
    #endif
}

void pipeServerRead(){
    if (serverPipe != INVALID_HANDLE_VALUE){
        if (ConnectNamedPipe(serverPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
        {
            // while (ReadFile(serverPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
            // {
            //     /* add terminating zero */
            //     buffer[dwRead] = '\0';
            //     /* do something with data in buffer */
            //     printf("%s", buffer);
            // }
            
            WriteFile(serverPipe,
                "Hello Pipe\n",
                12,   // = length of string + terminating '\0' !!!
                &dwWritten,
                NULL);
        }
        // DisconnectNamedPipe(serverPipe);
    }
}

void pipeClient(){
    #if defined(_WIN32) || defined(_WIN64)
        // HANDLE hPipe;
        // DWORD dwWritten;

        clientPipe = CreateFile(TEXT("\\\\.\\pipe\\emunes"), 
                        GENERIC_READ | GENERIC_WRITE, 
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);
        if (clientPipe == INVALID_HANDLE_VALUE)
        {
            while (true)
            {
                printf("error en client\n");
            }
            
        }
    #elif defined(__linux__)
    #endif
}

void pipeClientWrite(){
    // WriteFile(clientPipe,
    //     "Hello Pipe\n",
    //     12,   // = length of string + terminating '\0' !!!
    //     &dwWritten,
    //     NULL);
    while (ReadFile(clientPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE){
        /* add terminating zero */
        buffer[dwRead] = '\0';
        /* do something with data in buffer */
        printf("%s", buffer);
    }
    // CloseHandle(clientPipe);
}