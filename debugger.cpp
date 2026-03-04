#include <raylib.h>
#include "cpu.hpp"
#include <stdio.h>
#include <cstring>
#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "sharedDataMemory.hpp"

using namespace boost::interprocess;

void DrawDebuggerWindow(){
    DrawRectangle(0,0,500,500,WHITE);    
}

Emulator emulator;
int main(int argc, char const *argv[])
{
    //Remove shared memory on construction and destruction
    struct shm_remove
    {
        ~shm_remove(){ shared_memory_object::remove("MyName"); }
    } remover;

    //Create a shared memory object.
    shared_memory_object shm
        ( open_only                      //only open
        , "MyName"    //name
        , read_write                     //read-write mode
        );

    //Map the whole shared memory in this process
    mapped_region region
        (shm                       //What to map
        ,read_write //Map it as read-write
        );

    //Get the address of the mapped region
    void * addr       = region.get_address();

    //Obtain the shared structure
    shared_memory_buffer * data = static_cast<shared_memory_buffer*>(addr);

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    
    data->nstored.wait();
    data->mutex.wait();
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    data->mutex.post();
    data->nempty.post();
    //--------------------------------------------------------------------------------------

    // int command = 0;
    // unsigned int sendCommand = 0;
    
    std::string dataText;
    // SendToMain(sendCommand);
    // printf("sendToMain finish\n");
    // Main game loop

    data->commandDebugger=0;

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if(data->commandDebugger == 0){

            data->nstored.wait();
            data->mutex.wait();
            printf("debugger\n");
            emulator.cpu.pc = data->debugger.pc;
            emulator.cpu.registerX = data->debugger.x;
            emulator.cpu.registerY = data->debugger.y;
            emulator.cpu.accumulator = data->debugger.a;
            emulator.ppu.scanlineDot = data->debugger.ppuDot;
            emulator.ppu.scanlineRow = data->debugger.ppuScanline;
            data->commandDebugger = 1;
            data->mutex.post();
            data->nempty.post();
        }
        // if(command == 0){
        //     while (ReceiveMain(dataText) == 1)
        //     {
        //     }
        //     printf("dataText[0]: 0x%x\n", (unsigned char) dataText[0]);
        //     printf("dataText[1]: 0x%x\n", (unsigned char) dataText[1]);
        //     emulator.cpu.pc = (unsigned char) dataText[0];
        //     emulator.cpu.pc = emulator.cpu.pc | (((unsigned char) dataText[1])<<8);
        //     emulator.cpu.registerX = (unsigned char) dataText[2];
        //     emulator.cpu.registerY = (unsigned char) dataText[3];
        //     dataText[4] = '\0';
        //     command = 1;
        // }
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawRectangle(0,0,64,32,GRAY);
            DrawText("PAUSE",2,2,16,BLACK);
            if(CheckCollisionPointRec(GetMousePosition(),Rectangle{0,0,64,32}) == true && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) == true){
                printf("colision\n");
                data->commandDebugger = 0;
                data->commandMain = 2;
            }
            
            DrawRectangle(64,0,64,32,GRAY);
            DrawText("Play",66,2,16,BLACK);
            if(CheckCollisionPointRec(GetMousePosition(),Rectangle{64,0,64,32}) == true && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) == true){
                printf("colision\n");
                data->commandMain = 0;
            }

            
            DrawRectangle(64+32,0,64,32,GRAY);
            DrawText("NEXT",66+32,2,16,BLACK);
            if(CheckCollisionPointRec(GetMousePosition(),Rectangle{64+32,0,64,32}) == true && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) == true){
                data->commandDebugger = 0;
                if(data->commandMain == 3){
                    data->commandMain = 4;
                }else{
                    data->commandMain = 3;
                }
            }

            DrawText(TextFormat("PC: 0x%x",emulator.cpu.pc), 190, 200, 20, LIGHTGRAY);
            DrawText(TextFormat("X: 0x%x",emulator.cpu.registerX), 190, 220, 20, LIGHTGRAY);
            DrawText(TextFormat("Y: 0x%x",emulator.cpu.registerY), 190, 240, 20, LIGHTGRAY);
            DrawText(TextFormat("A: 0x%x",emulator.cpu.accumulator), 190, 260, 20, LIGHTGRAY);
            DrawText(TextFormat("PPU Dot,Scanline: 0x%x,0x%x",emulator.ppu.scanlineDot,emulator.ppu.scanlineRow), 190, 280, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
