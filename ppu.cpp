#include "cpu.hpp"
#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <stdlib.h>

RGBstruct SYSTEM_PALLETE_2C07[64] = {
    { 98,  98,  98}, {  0,  34,  99}, {13, 16, 125}, {43, 2, 125}, {68, 0, 99}, {83, 0, 54}, {83, 5, 2}, {68, 21, 0}, {43, 39, 0}, {13, 54, 0}, {0, 62, 0}, {0, 61, 2}, {0, 51, 54}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {171, 171, 171}, { 18,  81, 168}, {52, 56, 203}, {92, 36, 203}, {126, 25, 168}, {146, 27, 107}, {146, 41, 36}, {126, 63, 0}, {92, 87, 0}, {52, 107, 0}, {18, 118, 0}, {0, 116, 36}, {0, 103, 107}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {255, 255, 255}, { 98, 161, 250}, {133, 137, 255}, {172, 117, 255}, {207, 106, 250}, {227, 108, 188}, {227, 121, 117}, {207, 144, 55}, {172, 168, 20}, {133, 188, 20}, {98, 199, 55}, {78, 197, 117}, {78, 183, 188}, {78, 78, 78}, {0, 0, 0}, {0, 0, 0},
    {255, 255, 255}, {196, 221, 255}, {209, 211, 255}, {225, 203, 255}, {239, 199, 255}, {246, 200, 231}, {246, 205, 203}, {239, 214, 179}, {225, 223, 166}, {209, 231, 166}, {196, 235, 179}, {188, 235, 203}, {188, 229, 231}, {184, 184, 184}, {0, 0, 0}, {0, 0, 0}
};

int debugIndexNameTable;
int debugFrame = 1;
int debugCont = 0;
int deubgCont2 = 0;

void PPU::Initialize(){
    debugIndexNameTable = 0;
    cycleCounter = 0;
    // renderState = PPURENDER_STATE::NORMAL_RENDER;
    ppustatus = 0;
    scanlineDot = 0;
    scanlineRow = 0;
    tileLSB = 0;
    tileHSB = 0;
    Power();
    // paletteRAM[0x00] = 0x09;
    // paletteRAM[0x01] = 0x01;
    // paletteRAM[0x02] = 0x00;
    // paletteRAM[0x03] = 0x01;
    // paletteRAM[0x04] = 0x00;
    // paletteRAM[0x05] = 0x02;
    // paletteRAM[0x06] = 0x02;
    // paletteRAM[0x07] = 0x0D;
    // paletteRAM[0x08] = 0x08;
    // paletteRAM[0x09] = 0x10;
    // paletteRAM[0x0A] = 0x08;
    // paletteRAM[0x0B] = 0x24;
    // paletteRAM[0x0C] = 0x00;
    // paletteRAM[0x0D] = 0x00;
    // paletteRAM[0x0E] = 0x04;
    // paletteRAM[0x0F] = 0x2C;
    // paletteRAM[0x10] = 0x09;
    // paletteRAM[0x11] = 0x01;
    // paletteRAM[0x12] = 0x34;
    // paletteRAM[0x13] = 0x03;
    // paletteRAM[0x14] = 0x00;
    // paletteRAM[0x15] = 0x04;
    // paletteRAM[0x16] = 0x00;
    // paletteRAM[0x17] = 0x14;
    // paletteRAM[0x18] = 0x08;
    // paletteRAM[0x19] = 0x3A;
    // paletteRAM[0x1A] = 0x00;
    // paletteRAM[0x1B] = 0x02;
    // paletteRAM[0x1C] = 0x00;
    // paletteRAM[0x1D] = 0x20;
    // paletteRAM[0x1E] = 0x2C;
    // paletteRAM[0x1F] = 0x08;
}

void PPU::Power(){
    renderState = PPURENDER_STATE::PRE_RENDER;

    // Screen Settings
    scanlineDot = 321;
    scanlineRow = 261;

    // Registers
    ppuctrl = 0;
    ppumask = 0;
    // ppustatus = 0;
    oamaddr = 0;
    ppuscroll = 0;
    ppuAddress = 0;
}
void PPU::Reset(){
    // Screen Settings
    scanlineDot = 0;
    scanlineRow = 0;

    // Registers
    ppuctrl = 0;
    ppumask = 0;
    // ppustatus = 0;
    // oamaddr not change only in power
}

void PPU::SetFlag(SECTIONS6502 typeRegister, PPU_FLAGS insertFlag){
    switch (typeRegister)
    {
    case PPUCTRL:
        ppuctrl |= insertFlag;
        break;
    case PPUMASK:
        ppumask |= insertFlag;
        break;
    case PPUSTATUS:
        ppustatus |= insertFlag;
        break;
    case OAMADDR:
        oamaddr |= insertFlag;
        break;
    case OAMDATA:
        oamdata |= insertFlag;
        break;
    case PPUSCROLL:
        ppuscroll |= insertFlag;
        break;
    case PPUADDR:
        ppuAddress |= insertFlag;
        break;
    case OAMDMA:
        oamdma |= insertFlag;
        break;
    default:
        printf("PPU REGISTER INVALID\n");
        while (true){}
        break;
    }
}

void PPU::ClearFlag(SECTIONS6502 typeRegister, PPU_FLAGS clearFlag)
{
    switch (typeRegister)
    {
    case PPUCTRL:
        ppuctrl &= ~clearFlag;
        break;
    case PPUMASK:
        ppumask &= ~clearFlag;
        break;
    case PPUSTATUS:
        ppustatus &= ~clearFlag;
        break;
    case OAMADDR:
        oamaddr &= ~clearFlag;
        break;
    case OAMDATA:
        oamdata &= ~clearFlag;
        break;
    case PPUSCROLL:
        ppuscroll &= ~clearFlag;
        break;
    case PPUADDR:
        ppuAddress &= ~clearFlag;
        break;
    case OAMDMA:
        oamdma &= ~clearFlag;
        break;
    default:
        printf("PPU REGISTER INVALID\n");
        while (true){}
        break;
    }

}

RGBstruct pantalla[256*240];

void PPU::ShiftRegisterMoveBit(){
    if ((scanlineDot >= 1 && scanlineDot <= 257) || (scanlineDot >= 321 && scanlineDot <= 336)){ 
        if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
            || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
            // https://www.nesdev.org/wiki/PPU_rendering#Picture_region
            // The logical value of these constants is 1 for the high bitplane and 0 for the low bitplane.
            // In the circuit, the input for both is 1, 
            // but the two bitplanes undergo a different number of inversions,
            // making the constant logically 0 for the low one.
            shiftRegisterLowBytes  = (unsigned short) ((shiftRegisterLowBytes  << 1) | 0);
            shiftRegisterHighBytes = (unsigned short) ((shiftRegisterHighBytes << 1) | 1);
            shiftRegisterAttribute1 = (unsigned char) ((shiftRegisterAttribute1 << 1) | (latchRegisterAttribute & 1)); // Set Bit 0 from latchRegisterAttribute
            shiftRegisterAttribute2 = (unsigned char) ((shiftRegisterAttribute2 << 1) | ((latchRegisterAttribute & 10) >> 1)); // Set Bit 1 from latchRegisterAttribute
        }
    }
}
char debugPalattechange = 0;
void PPU::EmulateCycle(){
    // printf("ppu cycleCounter: 0x%X\n",cycleCounter);
    // printf("ppu render state: %d\n",renderState);
    // printf("dot, row: %d,%d\n",scanlineDot,scanlineRow);
    // printf("register T: %x\n",registerT);
    // printf("register V: %x\n",registerV);
    switch (renderState){
    case PPURENDER_STATE::PRE_RENDER: // scanline 261 or -1
        if(scanlineDot == 1){ // Power ON or Reset
            ClearFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_VBLANK);
            ClearFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_SPRITEHIT);
            emulator.cpu.beforeNMI = false;
        }

        // need the 337 to set new value to shift registers for "Donkey Kong", this need to be 0
        if((scanlineDot >= 1 && scanlineDot <= 256) || (scanlineDot >= 321 && scanlineDot <= 337)){
            if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
                TileFetch();
            }
        }

        if ((scanlineDot > 0 && scanlineDot <= 257) || (scanlineDot > 320 && scanlineDot <= 336)){
            if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
                ShiftRegisterMoveBit();
            }
        }

        if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){

            if(scanlineDot == 257){
                registerV = (registerV & 0xFBE0) | (registerT & 0x41F);
                // printf("257 registerV: 0x%x\n",registerV);
            }
            if(scanlineDot >= 280 && scanlineDot <= 304){
                registerV = (registerV & 0x041F) | (registerT & 0x7BE0);
                // printf("280 registerV: 0x%x\n",registerV);
            }
        }

        scanlineDot++;
        if(scanlineDot > PPU_SPECIALCYCLES::FINAL_SCANLINE){
            isEvenFrame = false;
            renderState = PPURENDER_STATE::NORMAL_RENDER;
            cycleCounter = 0;
            scanlineDot = 0;
            scanlineRow = 0;
            // printf("estoy en NORMAL_RENDER\n");
        }
        break;
    case PPURENDER_STATE::NORMAL_RENDER: // scanline 0 to 239
        // 1)Fetch a nametable entry from $2000-$2FFF.
        // 2)Fetch the corresponding attribute table entry from $23C0-$2FFF
        // and increment the current VRAM address within the same row.
        // 3)Fetch the low-order byte of an 8x1 pixel sliver
        // of pattern table from $0000-$0FF7 or $1000-$1FF7.
        // 4)Fetch the high-order byte of this sliver from an address 8 bytes higher.
        // 5)Turn the attribute data and the pattern table data into palette indices, and combine them with data from sprite data using priority.
        // empieza el dibujo

        if ((scanlineDot > 0 && scanlineDot <= 257) || (scanlineDot > 320 && scanlineDot <= 336)){
            if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
                ShiftRegisterMoveBit();
            }
        }

        // render pixel
        if(scanlineDot >= 4 && scanlineDot <=259){
            if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){

                // https://www.nesdev.org/wiki/PPU_rendering#Picture_region
                // On every dot in these background fetch regions,
                // a 4-bit pixel is selected by the fine x register from
                // the low 8 bits of the pattern and attributes shift registers,
                // which are then shifted.
                // char pixel = (char) ((shiftRegisterLowBytes >> (15 - registerX)) & 1); 
                // pixel = (char) (pixel | ((shiftRegisterHighBytes >> (15 - registerX)) & 1));
                    
                char col0 = (char)(((shiftRegisterLowBytes >> (15 - registerX))) & 1); // take the bit from the shift register for the pattern low bit plane
                char col1 = (char)(((shiftRegisterHighBytes >> (15 - registerX))) & 1); // take the bit from the shift register for the pattern high bit plane
                char pixelColor = (char)((col1 << 1) | col0);

                char pal0 = (char)(((shiftRegisterAttribute1) >> (7 - registerX)) & 1); // take the bit from the shift register for the attribute low bit plane
                char pal1 = (char)(((shiftRegisterAttribute2) >> (7 - registerX)) & 1); // take the bit from the shift register for the attribute high bit plane
                char Palette = (char)((pal1 << 1) | pal0);
                
                // 4bit0
                // -----
                // SAAPP
                // |||||
                // |||++- Pixel value from tile pattern data
                // |++--- Palette number from attributes
                // +----- Background/Sprite select
                char PaletteRAMAddress = (char)(Palette << 2 | pixelColor); // the Palette RAM address is determined by the palette and color we found.
                
                RGBstruct pixel = SYSTEM_PALLETE_2C07[((paletteRAM[0x00 | PaletteRAMAddress]) & 0x3F)];
                
                // // // if(pixelColor != 0){
                // // if(scanlineDot-4 == 21 && scanlineRow == 32){
                // // if(PaletteRAMAddress == 2){
                // if(scanlineDot-4 >=24 && scanlineDot-4 < 32 && scanlineRow == 33 && debugPalattechange>0x1F){
                //     printf("pixel x,y: %d, %d\n",scanlineDot-4,scanlineRow);
                //     // printf("low byte: 0x%x\n",shiftRegisterLowBytes);
                //     // printf("high byte: 0x%x\n",shiftRegisterHighBytes);
                    
                //     printf("color low byte: 0x%x\n",shiftRegisterAttribute1);
                //     printf("color high byte: 0x%x\n",shiftRegisterAttribute2);
                //     printf("pixel value: 0x%x\n",pixelColor);
                //     printf("Palette value: 0x%x\n",Palette);
                //     printf("PaletteRAMAddress value: 0x%x\n",PaletteRAMAddress);
                //     printf("final PaletteRAMAddress value: 0x%x\n",((paletteRAM[0x00 | PaletteRAMAddress] & 0x3F)));
                //     // for(int i=0; i<0x1F;i++){
                //     //     printf("palette[0x%x]: 0x%x\n",i,paletteRAM[i]);
                //     // }
                //     printf("final r,g,b: %d,%d,%d\n",pixel.R,pixel.G,pixel.B);
                //     printf("\n");
                //     // if(PaletteRAMAddress == 2){
                //     // printf("\n");
                //     // }
                // }

                // pantalla[(scanlineDot-4)+scanlineRow*256] = pixelColor;
                pantalla[(scanlineDot-4)+scanlineRow*256] = pixel;

            }
        }

        // need the dot 337 to set new value to shift registers for "Donkey Kong", this need to be in dot 0
        if((scanlineDot >= 1 && scanlineDot <= 259) || (scanlineDot>=321 && scanlineDot <= 337)){
            if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
                TileFetch();
            }
        }

        if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){

            if(scanlineDot == 257){
                registerV = (registerV & 0xFBE0) | (registerT & 0x41F);
                // printf("257 registerV: 0x%x\n",registerV);
            }
        }

        scanlineDot++;
        if(scanlineDot % 341 == 0){
            scanlineRow++;
            scanlineDot = 0;
        }
        if (scanlineRow > 239){ // visible scanlines
            cycleCounter = 0;
            scanlineDot = 0;
            renderState = POST_RENDER;
            debugIndexNameTable = 0;
            // printf("estoy en POST_RENDER\n");
        }

        break;
    case PPURENDER_STATE::POST_RENDER:
        if(scanlineDot == 1 && scanlineRow == 241){
            SetFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_VBLANK);

            BeginTextureMode(target);
            ClearBackground(BLACK);
            int x = 0;
            int y = 0;
            int cont=0;
            for(int i=0; i< 61440;i++){

                // DrawText(TextFormat("%02x",pantalla[i]),x,y,8,BEIGE);
                // if(pantalla[i] == 0){
                //     DrawPixel(x,y,BLACK);
                // }else{
                //     DrawPixel(x,y,WHITE);
                // }
                DrawPixel(x,y,Color{pantalla[i].R,pantalla[i].G,pantalla[i].B,255});
                // x+=12;
                x++;
                cont++;
                if(cont == 256){
                    cont=0;
                    // y+=9;
                    y++;
                    x=0;
                }
            }
            EndTextureMode();

            BeginDrawing();
                ClearBackground(BLACK);     // Clear screen background

                // Draw render texture to screen, properly scaled
                DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                            (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                            (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
            EndDrawing();
            // BeginDrawing();
            // ClearBackground(BLACK);
            // int x = 0;
            // int y = 0;
            // int cont=0;
            // for(int i=0; i< 61440;i++){

            //     // DrawText(TextFormat("%02x",pantalla[i]),x,y,8,BEIGE);
            //     // if(pantalla[i] == 0){
            //     //     DrawPixel(x,y,BLACK);
            //     // }else{
            //     //     DrawPixel(x,y,WHITE);
            //     // }
            //     DrawPixel(x,y,Color{pantalla[i].R,pantalla[i].G,pantalla[i].B,255});
            //     // x+=12;
            //     x++;
            //     cont++;
            //     if(cont == 256){
            //         cont=0;
            //         // y+=9;
            //         y++;
            //         x=0;
            //     }
            // }
            // EndDrawing();
        }

        scanlineDot++;

        if(scanlineDot%341 == 0){
            scanlineRow++;
            scanlineDot=0;
        }

        if(scanlineRow > 260){
            renderState = PRE_RENDER;
            scanlineDot=0;
        }
        break;
    default:
        break;
    }
    cycleCounter++;
}

unsigned char PPU::FetchPPU(unsigned short Address)
{
    Address &= 0x3FFF;
    // printf("PPU Address: 0x%X\n",Address);
    if(Address < 0x2000){
        if(useCHRRAM){
            return emulator.game->CHRRAM[Address];
        }
        switch (emulator.game->mapper){
            case 0: return emulator.game->CHRROM[Address & (emulator.game->CHR_Size - 1)];
            default:
                printf("error mapper ppu\n");
                while (true){}

                break;
        }


    }else if(Address < 0x3F00){
        // 0x2000 to 0x2FFF VRAM
        // 0x3000 to 0x3EFF usually a mirror
        Address = MirroringAddress(Address);
        Address &= 0x7FF;
        return VRAM[Address];
    }else if (Address >= 0x3F00){
        // read from palette RAM.
        // Palette RAM only returns bits 0-5, so bits 6 and 7 are PPU open bus.
        Address = MirroringAddress(Address);
        return (unsigned char)((paletteRAM[Address & 0x1F] & 0x3F) | (ppuBus & 0xC0));
    }
    return 0;
}

unsigned short PPU::MirroringAddress(unsigned short Address)
{
    // No mirror
    if(Address < 0x2000){
        return Address;
    }

    // if the vram address is pointing to the color palettes:
    if (Address >= 0x3F00)
    {
        Address &= 0x3F1F;
        if ((Address & 3) == 0)
        {
            Address &= 0x3F0F;
        }
        return Address;
    }

    Address &= 0x2FFF; // $3000 through $3F00 is always mirrored down.
    switch(emulator.game->mapper){
        case 0:
            if(emulator.game->mirroring == 0){
                // Vertical Mirroring
                // |0|1|
                // |2|3|
                // nametable 0 = nametable 2
                // nametable 1 = nametable 3
                Address &= 0x37FF; // mask away $0800
            }else{
                // Horizontal Mirroring
                // |0|1|
                // |2|3|
                // nametable 0 = nametable 1
                // nametable 2 = nametable 3
                Address = (unsigned short)((Address & 0x33FF) | ((Address & 0x0800) >> 1)); // mask away $0C00, bit 10 becomes the former bit 11
            }
            break;
        default:
            printf("mapper no support\n");
            while (true){}

            break;
    }
    return Address;
}



void PPU::TileFetch(){
    // 8 cycles
    unsigned char actualCycle = (unsigned char) ((scanlineDot-1) & 0x7);
    // printf("tile fetch cycle: %d\n",actualCycle);
    //Each memory fetch takes 2 dots:
    // dot 1: the full address is placed onto the PPU's address bus
    //        and the low 8 bits are stored into an external address latch
    // dot 2: the read is performed.

    if(actualCycle == 0){

        // shift start in dot 9 
        shiftRegisterLowBytes = (unsigned short) ((shiftRegisterLowBytes & 0xFF00) | tileLSB);
        shiftRegisterHighBytes = (unsigned short) ((shiftRegisterHighBytes & 0xFF00) | tileHSB);
        latchRegisterAttribute = attributeColor;
    
        // Load registers: load the pixel byte from before
        // shiftRegisterLowBytes = (unsigned short)((shiftRegisterLowBytes & 0xFF00) | (registerT & 0x00FF));
        // shiftRegisterHighBytes = (unsigned short)((shiftRegisterHighBytes & 0xFF00) | ((registerT & 0xFF00)>>8));
        // 1)Fetch a nametable entry from $2000-$2FFF.
        int baseNametableAddress = (ppuctrl & 0x3);

        if(baseNametableAddress == 0){
            // printf("base: 0x2000\n");
            ppuAddress = 0x2000;
        }else if(baseNametableAddress == 1){
            // printf("base: 0x2400\n");
            ppuAddress = 0x2400;
        }else if(baseNametableAddress == 2){
            // printf("base: 0x2800\n");
            ppuAddress = 0x2800;
        }else if(baseNametableAddress == 3){
            // printf("base: 0x2C00\n");
            ppuAddress = 0x2C00;
        }

        ppuAddress = (unsigned short)(0x2000 | (registerV & 0x0FFF));
        // printf("registerV: 0x%X\n",registerV);

        ppuBus = (unsigned char)(ppuAddress & 0x00FF);
        // printf("nametable address: 0x%X\n",ppuAddress);
        // printf("coarse x,y: %d,%d\n",(registerV & 0x001F),((registerV & 0x03E0)>>5));
        // nametableTest = FetchPPU(ppuAddress);
        // printf("%X ",nametableTest);
    }else if(actualCycle == 1){
        nametableByte = FetchPPU(ppuAddress);
        // printf("scanline x, y: %d, %d\n",scanlineDot,scanlineRow);
        // printf("ppuAddress: 0x%x\n",ppuAddress);
        // printf("nametableByte:0x%x\n",nametableByte);
        // printf("HB,LB: 0x%X,0x%X\n",shiftRegisterHighBytes,shiftRegisterLowBytes);
        // ppuBus = FetchPPU(ppuAddress);
    }else if(actualCycle == 2){
        // 2)Fetch the corresponding attribute table entry from $23C0-$2FFF
        // and increment the current VRAM address within the same row.
        // https://www.nesdev.org/wiki/PPU_scrolling#Tile_and_attribute_fetching
        // start in 0x23C0
        // NN 1111 YYY XXX
        // || |||| ||| +++-- high 3 bits of coarse X (x/4)
        // || |||| +++------ high 3 bits of coarse Y (y/4)
        // || ++++---------- attribute offset (960 bytes)
        // ++--------------- nametable select
        ppuAddress = (unsigned short)(0x23C0 | (registerV & 0x0C00) | ((registerV >> 4) & 0x38) | ((registerV >> 2) & 0x07));
    }else if(actualCycle == 3){
        attributeColor = FetchPPU(ppuAddress);
    }else if(actualCycle == 4){
        // 3)Fetch the low-order byte of an 8x1 pixel sliver
        // of pattern table from $0000-$0FF7 or $1000-$1FF7.
        // https://www.nesdev.org/wiki/PPU_pattern_tables
        // DCBA98 76543210
        // ---------------
        // 0HNNNN NNNNPyyy
        // |||||| |||||+++- T: Fine Y offset, the row number within a tile
        // |||||| ||||+---- P: Bit plane (0: less significant bit; 1: more significant bit)
        // ||++++-++++----- N: Tile number from name table
        // |+-------------- H: Half of pattern table (0: "left"; 1: "right")
        // +--------------- 0: Pattern table is at $0000-$1FFF
        // esto solo funciona para el background
        // (registerV & 0b0111 0000 0000 0000) >> 12 = T = Fine Y offset
        ppuAddress = (unsigned short)(((registerV & 0b0111000000000000) >> 12) | nametableByte * 16 | ((ppuctrl & BACKGROUND_PATTERN_FLAG) == BACKGROUND_PATTERN_FLAG ? 0x1000 : 0));
        tileLSB = FetchPPU(ppuAddress);
        // registerT = tileLSB;
    }else if(actualCycle == 5){
        // 4)Fetch the high-order byte of this sliver from an address 8 bytes higher.
        ppuAddress += 8; // +8
    }else if(actualCycle == 6){
        tileHSB = FetchPPU(ppuAddress);
        // registerT = tileHSB << 8;
    }else if(actualCycle == 7){
        tileValue = tileLSB | tileHSB; // TODO: DELETE THIS
        // printf("tileValue: 0x%X\n",tileValue);
        // if(tileLSB != 0 || tileHSB != 0){
        //     printf("new\n");
        // }

        if((registerV & 0x001F) == 31){
            registerV &= ~0x001F;
            registerV ^= 0x0400; // increment 1024
        }else{
            registerV++; // debe ser 1 para los ciclos precisos
        }
        // printf("cycle 7 registerV: 0x%x\n",registerV);

        // printf("scanlineRow: %d\n",scanlineRow);
        // printf("scanlineDot: %d\n",scanlineDot);
        if(scanlineDot == 256){ // debe ser 256
            if ((registerV & 0x7000) != 0x7000){        // if fine Y < 7
                registerV += 0x1000;                    // increment fine Y
            }else{

            // printf("\n");
                registerV &= ~0x7000;                   // fine Y = 0
                int y = (registerV & 0x03E0) >> 5;       // let y = coarse Y
                if (y == 29){
                    y = 0;                          // coarse Y = 0
                    registerV ^= 0x0800;                    // switch vertical nametable
                }else if (y == 31){
                    y = 0;                          // coarse Y = 0, nametable not switched
                }else{
                    y += 1;// increment coarse Y
                }
                registerV = (registerV & ~0x03E0) | (y << 5);     // put coarse Y back into v
            }

            // printf("vertical registerV: 0x%X\n",registerV);
        }
    }



    // 5)Turn the attribute data and the pattern table data into palette indices, and combine them with data from sprite data using priority.
    // https://www.nesdev.org/wiki/PPU_sprite_evaluati
    //First, it clears the list of sprites to draw.
    //Second, it reads through OAM, checking which sprites will be on this scanline. It chooses the first eight it finds that do.
    //Third, if eight sprites were found, it checks (in a wrongly-implemented fashion) for further sprites on the scanline to see if the sprite overflow flag should be set.
    //Fourth, using the details for the eight (or fewer) sprites chosen, it determines which pixels each has on the scanline and where to draw the

    // tileValue = FetchPPU(tileValue);
    // printf("new tileValue: 0x%X\n",tileValue
}

void PPU::GetPallete() {
}

void PPU::StorePPUData(unsigned char value, unsigned short Address){
    // writing to the PPU's VRAM.
    // first, check if the address has any mirroring going on:
    // Address = PPUAddressWithMirroring(Address);
    Address = MirroringAddress(Address);
    if (Address < 0x2000) // if this is pointing to CHR RAM
    {
        emulator.game->CHRRAM[Address] = value;
    }
    else if (Address >= 0x3F00)
    {
        printf("set pallete 0x%x: 0x%x\n",Address&0x1f,value);
        debugPalattechange++;
        paletteRAM[Address & 0x1F] = value;
    }
    else // if this is not pointing to CHR RAM or palettes
    {
        VRAM[Address & 0x7FF] = value;

    }
}

void PPU::StoreInPPU(unsigned char value, unsigned short address) {
    unsigned char data = 0;
    // printf("store ppu pc: 0x%X\n",emulator.cpu.pc);

    // printf("ppu write register: 0x%X\n",address);
    // printf("ppuvalue: 0x%X\n",value);
    if(address == SECTIONS6502::PPUCTRL){ // 0x2000
        ppuBus = value;
        // printf("pc en 2000: 0x%X\n",emulator.cpu.pc);
        // printf("value: 0x%x\n",value);
        // only need to update the bit 10 and 11 from registerT
        // with the bit 0 and 1 from "value"
        registerT = (unsigned short) ((registerT & 0x73FF) | ((value & 0x3)<<10));
        if((value & PPU_FLAGS::BASE_NAMETABLE_BIT1) == PPU_FLAGS::BASE_NAMETABLE_BIT1){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::BASE_NAMETABLE_BIT1);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::BASE_NAMETABLE_BIT1);
        }

        if((value & PPU_FLAGS::BASE_NAMETABLE_BIT2) == PPU_FLAGS::BASE_NAMETABLE_BIT2){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::BASE_NAMETABLE_BIT2);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::BASE_NAMETABLE_BIT2);
        }

        if((value & PPU_FLAGS::VRAM_ADDRESS_FLAG) == PPU_FLAGS::VRAM_ADDRESS_FLAG){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::VRAM_ADDRESS_FLAG);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::VRAM_ADDRESS_FLAG);
        }

        if((value & PPU_FLAGS::SPRITE_PATTERN_FLAG) == PPU_FLAGS::SPRITE_PATTERN_FLAG){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::SPRITE_PATTERN_FLAG);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::SPRITE_PATTERN_FLAG);
        }

        if((value & PPU_FLAGS::BACKGROUND_PATTERN_FLAG) == PPU_FLAGS::BACKGROUND_PATTERN_FLAG){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::BACKGROUND_PATTERN_FLAG);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::BACKGROUND_PATTERN_FLAG);
        }

        if((value & PPU_FLAGS::SPRITE_SIZE_FLAG) == PPU_FLAGS::SPRITE_SIZE_FLAG){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::SPRITE_SIZE_FLAG);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::SPRITE_SIZE_FLAG);
        }

        if((value & PPU_FLAGS::PPU_MASTER_SLAVE) == PPU_FLAGS::PPU_MASTER_SLAVE){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::PPU_MASTER_SLAVE);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::PPU_MASTER_SLAVE);
        }

        if((value & PPU_FLAGS::NMI_FLAG) == PPU_FLAGS::NMI_FLAG){
            SetFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::NMI_FLAG);
        }else{
            ClearFlag(SECTIONS6502::PPUCTRL,PPU_FLAGS::NMI_FLAG);
        }

    }else if(address == SECTIONS6502::PPUMASK){ // 0x2001
        ppuBus = value;
        if((value & PPU_FLAGS::GREYSCALE_BIT_0) == PPU_FLAGS::GREYSCALE_BIT_0){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::GREYSCALE_BIT_0);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::GREYSCALE_BIT_0);
        }

        if((value & PPU_FLAGS::BACKGROUND_LEFT_COLUMN_BIT_1) == PPU_FLAGS::BACKGROUND_LEFT_COLUMN_BIT_1){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::BACKGROUND_LEFT_COLUMN_BIT_1);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::BACKGROUND_LEFT_COLUMN_BIT_1);
        }

        if((value & PPU_FLAGS::SPRITE_LEFT_COLUMN_BIT_2) == PPU_FLAGS::SPRITE_LEFT_COLUMN_BIT_2){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::SPRITE_LEFT_COLUMN_BIT_2);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::SPRITE_LEFT_COLUMN_BIT_2);
        }

        if((value & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == PPU_FLAGS::BACKGROUND_ENABLE_BIT_3){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::BACKGROUND_ENABLE_BIT_3);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::BACKGROUND_ENABLE_BIT_3);
        }

        if((value & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == PPU_FLAGS::SPRITE_ENABLE_BIT_4){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::SPRITE_ENABLE_BIT_4);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::SPRITE_ENABLE_BIT_4);
        }

        if((value & PPU_FLAGS::COLOR_R_BIT_5) == PPU_FLAGS::COLOR_R_BIT_5){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::COLOR_R_BIT_5);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::COLOR_R_BIT_5);
        }

        if((value & PPU_FLAGS::COLOR_G_BIT_6) == PPU_FLAGS::COLOR_G_BIT_6){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::COLOR_G_BIT_6);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::COLOR_G_BIT_6);
        }

        if((value & PPU_FLAGS::COLOR_B_BIT_7) == PPU_FLAGS::COLOR_B_BIT_7){
            SetFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::COLOR_B_BIT_7);
        }else{
            ClearFlag(SECTIONS6502::PPUMASK,PPU_FLAGS::COLOR_B_BIT_7);
        }

        // registerT = (ppuctrl & 0x03); // WHY is this here?

    }else if(address == SECTIONS6502::PPUSTATUS){ // 0x2002
        ppuBus = value;
        // data = ppu.ppustatus; //+ (PPUBus & 0x1F);
        // ppu.ClearFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_VBLANK);
        printf("ppu data write: 0x%X\n",data);
    }else if(address == SECTIONS6502::OAMADDR){ // 0x2003
        ppuBus = value;
        printf("OAMADDR\n");
        // while (true){}
    }else if(address == SECTIONS6502::OAMDATA){ // 0x2004
        ppuBus = value;
        // data = ReadOAM();
        printf("readOAMDATA\n");
        // while (true){}

    }else if(address == SECTIONS6502::PPUSCROLL){ // 0x2005
        ppuBus = value;
        // printf("PPUSCROLL\n");
        // printf("pc en 2005: 0x%X\n",emulator.cpu.pc);
        // only need to update the bit 0 to 4 from registerT
        // with bit 3 to 7 from value
        if(registerW == false){
            registerT = (unsigned short) ((registerT & 0x7FE0) | ((value & 0xF8)>>3));
            registerX = value & 0x7; // only need the bit 0 to 2 from value
            // printf("1 scroll register T: %x\n",registerT);
        }else{
            registerT = (unsigned short) ((registerT & 0xC1F) | ((value & 0xF8)<<2) | ((value & 0x7)<<12));
            // printf("2 scroll register T: %x\n",registerT);
        }
        registerW = !registerW;
    }else if(address == SECTIONS6502::PPUADDR){ // 0x2006
        ppuBus = value;
        // printf("PPUADDR\n");
        // printf("store ppu pc: 0x%X\n",emulator.cpu.pc);
        if(registerW == false){
            registerT = (unsigned short) ((registerT & 0b0100000011111111) | ((value & 0x3F) << 8));
            registerT |= (0 << 14); // bit 14 (or Z) is clear
            // printf("1 register T: %x\n",registerT);
        }else{
            registerT = (unsigned short) ((registerT & 0b0111111100000000) | (value));
            registerV = registerT;
            // printf("2 register T: %x\n",registerT);
            // printf("register V: %x\n",registerV);
        }
        registerW = !registerW;
    }else if(address == SECTIONS6502::PPUDATA){ // 0x2007
        // printf("PPUDATA\n");
        ppuBus = value;
        ppudata = value;
        StorePPUData(ppudata, registerV);
        // printf("2007 pc: 0x%X\n",emulator.cpu.pc);
        // printf("2007 registerV: 0x%X\n",registerV);
        // printf("ppudata: 0x%X\n",ppudata);
        // printf("registerY: 0x%X\n",emulator.cpu.registerY);
        if((ppuctrl & PPU_FLAGS::VRAM_ADDRESS_FLAG) == PPU_FLAGS::VRAM_ADDRESS_FLAG){
            // if bit is one (1)
            registerV += 32;
        }else{
            registerV++;
            // if bit is zero (0)
        }
        // ppuctrl
        // while (true){}
    }
}
