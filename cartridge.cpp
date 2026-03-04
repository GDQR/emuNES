#include <stdio.h>
#include "cartridge.hpp"
#include "cpu.hpp"
#include <stdlib.h>
#include <string>

void Cartridge::LoadGame(const char* file){
    FILE* fileData = fopen(file,"rb");
    if(fileData == NULL){
        printf("error reading file\n");
        exit(1);
    }

    fseek(fileData, 0, SEEK_END); // seek to end of file
    int bufferSize = ftell(fileData); // get current file pointer
    fseek(fileData, 0, SEEK_SET);

    unsigned char *buffer = (unsigned char*) malloc(bufferSize + 1);
    fread(buffer,bufferSize,1,fileData);

    mapper = (unsigned char)(buffer[7] & 0xF0);
    mapper |= (unsigned char)(buffer[6] >> 4);

    PRG_Size = buffer[INES::PRG_SIZE];
    CHR_Size = buffer[INES::CHR_SIZE];
    
    // mirroring = 0 == horizontal mirrored
    // mirroring = 1 == vertically mirrored
    mirroring = (buffer[6] & 1) == 0;
    printf("mirroring: %d\n",mirroring);

    printf("PRG_Size: %d\n",PRG_Size);
    printf("CHR_Size: %d\n",CHR_Size);
    PRGROM = (unsigned char*) malloc(PRG_Size * 0x4000);
    CHRROM = (unsigned char*) malloc(CHR_Size * 0x2000);
    CHRRAM = (unsigned char*) malloc(0x2000);

    fseek(fileData, 0x10, SEEK_SET);
    fread(PRGROM,PRG_Size * 0x4000,1,fileData);
    
    fseek(fileData, 0x10+ (PRG_Size * 0x4000), SEEK_SET);
    fread(CHRROM,(CHR_Size * 0x2000),1,fileData);

    PRGRAM = (unsigned char*) malloc(0x2000);
    fclose(fileData);
    PRG_Size = PRG_Size * 0x4000;
    CHR_Size = CHR_Size * 0x2000;
    
    printf("new PRG_Size: %d\n",PRG_Size);
    printf("new CHR_Size: %d\n",CHR_Size);
    emulator.ppu.useCHRRAM = CHR_Size == 0 ? true : false; 

    if(emulator.ppu.useCHRRAM == true){
        printf("usar CHRAM\n");
    }else{
        printf("no usar CHRAM\n");
        // int j=0;
        
        // FILE* graficos = fopen("graficos.txt","w");
        // std::string texto;
        // for(unsigned int i=0;i<CHR_Size;i++){
        //     printf("0x%X ",CHRROM[i]);
        //     // texto = "0x ";
        //     texto = CHRROM[i];
        //     // texto += " ";
        //     j++;
        //     if(j==256){
        //         printf("\n");
        //         // texto += "\n";
        //         j=0;
        //     }
        //     fputs(texto.c_str(),graficos);
        // }
        // fclose(graficos);
    }
    // int count = 0;
    // for(int i=0;i<PRG_Size;i++){
    //     printf("%x ",PRGROM[i]);
    //     count++;
    //     if(count==16){
    //         printf("\n");
    //         count = 0;
    //     }
    // }

    // while (true)
    // {
    //     /* code */
    // }
    
    printf("GAME LOADED!\n");
}