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
    for(int i=0;i<256;i++){
        oam1[i] = 0xFF;
    }
    for(int i=0;i<32;i++){
        oam2[i] = 0xFF;
    }
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
    renderState = PPURENDER_STATE::NORMAL_RENDER;

    // Screen Settings
    scanlineDot = 0; 
    scanlineRow = 0;

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
    // https://forums.nesdev.org/viewtopic.php?t=10348
    // "The shifters seem to shift between dots 2...257 and dots 322...337 (inclusive, with a shift on each of those dots).
    // ... it wouldn't make sense for it to keep shifting after it fetches the second tile at the end of the line,
    // since data for the first tile would be shifted out and lost then."
    if ((scanlineDot >= 2 && scanlineDot <= 257) || (scanlineDot >= 322 && scanlineDot <= 337)){ 
        if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
            || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
            // https://www.nesdev.org/wiki/PPU_rendering#Picture_region
            // The logical value of these constants is 1 for the high bitplane and 0 for the low bitplane.
            // In the circuit, the input for both is 1, 
            // but the two bitplanes undergo a different number of inversions,
            // making the constant logically 0 for the low one.
            shiftRegisterLowBytes  = (unsigned short) ((shiftRegisterLowBytes  << 1) | 0);
            shiftRegisterHighBytes = (unsigned short) ((shiftRegisterHighBytes << 1) | 1);
            // save only 2 bits from latchRegisterAttribute
            shiftRegisterAttribute1 = (unsigned char) ((shiftRegisterAttribute1 << 1) | (latchRegisterAttribute & 1)); // Set Bit 0 from latchRegisterAttribute
            shiftRegisterAttribute2 = (unsigned char) ((shiftRegisterAttribute2 << 1) | ((latchRegisterAttribute & 3) >> 1)); // Set Bit 1 from latchRegisterAttribute
            
        }
    }
}

void PPU::ShiftRegisterSprite(){
    if(scanlineDot >=2 && scanlineDot <= 256){
        // shift all 8 sprite shift registers.
        int i = 0;
        while (i < 8)
        {
            if (spriteBuffer[i].posX > 0)
            {
                spriteBuffer[i].posX--; // decrement the X position of all objects in secondary OAM. When this is zero, the ppu can draw it.
            }
            else
            {
                if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
                    spriteBuffer[i].SRLowByte = (spriteBuffer[i].SRLowByte << 1); // shift 1 bit to the left.
                    spriteBuffer[i].SRHighByte = (spriteBuffer[i].SRHighByte << 1); // shift 1 bit to the left.
                }
            }

            i++;
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
            ClearFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_OVERFLOW);
            emulator.cpu.beforeNMI = false;
        }

        ShiftRegisterMoveBit();

        // need the 337 to set new value to shift registers for "Donkey Kong", this need to be 0
        if(/*(scanlineDot >= 1 && scanlineDot <= 256) ||*/ (scanlineDot >= 321 && scanlineDot <= 338)){
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

        // render pixel
        if(scanlineDot >= 2 && scanlineDot <=257){
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

                char backgroundPixelData = (Palette<<2) | pixelColor;
                
                // 4bit0
                // -----
                // SAAPP
                // |||||
                // |||++- Pixel value from tile pattern data
                // |++--- Palette number from attributes
                // +----- Background/Sprite select
                // char PaletteRAMAddress = (char)(Palette << 2 | pixelColor); // the Palette RAM address is determined by the palette and color we found.
                // pixel1 = pixel2;
                // pixel2 = pixel3;
                // pixel3 = PaletteRAMAddress;

                // RGBstruct pixel = SYSTEM_PALLETE_2C07[((paletteRAM[0x00 | PaletteRAMAddress]) & 0x3F)];
                
                int i = 0;
                bool SpixelL = false;
                bool SpixelH = false;
                char spritePixelData = 0;
                char SpritePalette = 0;
                char SpriteColor = 0;
                while (i<8 && SpixelL == false && SpixelH == false){
                    if(spriteBuffer[i].posX == 0){
                        if((spriteBuffer[i].SRLowByte & 0x80) == 0x80){
                            SpixelL = true;
                            SpriteColor=1;
                        }
                        if((spriteBuffer[i].SRHighByte & 0x80) == 0x80){
                            SpixelH = true;
                            SpriteColor |= 2;
                        }
                        SpriteColor = 0;
                        if (SpixelL == true) { SpriteColor=1; spritePixelData = 1; }
                        if (SpixelH == true) { SpriteColor |= 2; spritePixelData |= 2; }

                        SpritePalette = ((spriteBuffer[i].latchAttribute & 0x03) | 0x04); // read the palette from secondary OAM attributes.
                        spritePixelData |= ((spriteBuffer[i].latchAttribute<<2)) | 0x04;
                    }
                    i++;
                }
                
                char debugSprite = (SpritePalette<<2) | SpriteColor;

                // Sprite 0 hit
                if(scanlineDot < 255 && (ppustatus & PPU_FLAGS::PPU_SPRITEHIT) == 0){
                    if(SpriteColor != 0 && pixelColor != 0){
                        SetFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_SPRITEHIT);
                    }
                }else if(scanlineDot >= 2 && scanlineDot <= 8 && (ppumask & PPU_FLAGS::SPRITE_LEFT_COLUMN_BIT_2) == 0 && (ppumask & PPU_FLAGS::BACKGROUND_LEFT_COLUMN_BIT_1) == 0 && (ppustatus & PPU_FLAGS::PPU_SPRITEHIT) == 0){
                    if(SpriteColor != 0 && pixelColor != 0){
                        SetFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_SPRITEHIT);
                    }
                }
                // palette
                // 4bit0
                // -----
                // SAAPP
                // |||||
                // |||++- Pixel value from tile pattern data
                // |++--- Palette number from attributes
                // +----- Background/Sprite select
                char finalColor = backgroundPixelData;
                if(pixelColor == 0 && SpriteColor != 0){
                    finalColor = debugSprite;
                }else if ((debugSprite & 0x3) != 0) // the background color isn't zero...
                {
                    if ((debugSprite & 0x10) == 0x10) // if the sprite has priority, always draw the sprite.
                    {
                        finalColor = debugSprite;
                    }
                }

                // char PaletteRAMAddress = (char)(Palette << 2 | finalColor); // the Palette RAM address is determined by the palette and color we found.
                pixel1 = pixel2;
                pixel2 = pixel3;
                pixel3 = finalColor;
                
                // // // if(pixelColor != 0){
                // // if(scanlineDot-4 == 21 && scanlineRow == 32){
                // // if(PaletteRAMAddress == 2){
                // // if(scanlineDot-4 >=24 && scanlineDot-4 < 32 && scanlineRow == 33 && debugPalattechange>0x1F){
                // if(scanlineDot-4 >=0 && scanlineDot-4 <= 32 && scanlineRow == 32 && debugPalattechange>0x1F){
                // if(((scanlineRow == 33 && scanlineDot <=35) || (scanlineRow == 32 /*&& scanlineDot>=321*/)) && debugPalattechange>0x1F){
                //     // printf("pixel x,y: %d, %d\n",scanlineDot-4,scanlineRow);
                //     printf("pixel x,y: %d, %d\n",scanlineDot,scanlineRow);
                //     printf("registerX: 0x%x\n",registerX);
                //     printf("NT low byte: 0x%x\n",shiftRegisterLowBytes);
                //     printf("NT high byte: 0x%x\n",shiftRegisterHighBytes);
                    
                //     printf("latchRegisterAttribute: 0x%x\n",latchRegisterAttribute);
                //     printf("color low byte: 0x%x\n",shiftRegisterAttribute1);
                //     printf("color high byte: 0x%x\n",shiftRegisterAttribute2);
                //     printf("pixel value: 0x%x\n",pixelColor);
                //     printf("Palette Index: 0x%x\n",Palette);
                //     printf("Palette color index: 0x%x\n",PaletteRAMAddress);
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

            }
        }

        ShiftRegisterMoveBit();
        ShiftRegisterSprite();
        
        if(scanlineDot>=4 && scanlineDot<=259){
            RGBstruct pixel = SYSTEM_PALLETE_2C07[((paletteRAM[0x00 | pixel1]) & 0x3F)];                
            pantalla[(scanlineDot-4)+scanlineRow*256] = pixel;
            // if(scanlineRow == 33 && debugPalattechange>0x1F){
            // printf("Draw %d,%d: 0x%x\n",(scanlineDot-4),(scanlineRow),pixel1);

            // }
        }


        // need the dot 337 to set new value to shift registers for "Donkey Kong", this need to be in dot 0
        if((scanlineDot >= 1 && scanlineDot <= 257) || scanlineDot==305 || (scanlineDot>=321 && scanlineDot <= 338)){
            if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){
                TileFetch();
            }
        }

        if((ppumask & PPU_FLAGS::BACKGROUND_ENABLE_BIT_3) == BACKGROUND_ENABLE_BIT_3
                || (ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){

            if(scanlineDot >= 1 && scanlineDot <=64){
                if(scanlineDot == 1){
                    oam2Address = 0;
                }
                if((scanlineDot & 1) == 0){
                    oam2[oam2Address] = 0xFF;
                    oam2Address++;
                }
            }else if(scanlineDot >= 65 && scanlineDot <= 256){
                if(scanlineDot == 65){
                    oam2Address = 0;
                }
                // odd cycle
                if((scanlineDot & 1) == 1){
                    // 1. Starting at n = 0, read a sprite's Y-coordinate
                    // (OAM[n][0], copying it to the next open slot in secondary OAM
                    // (unless 8 sprites have been found, in which case the write is ignored).
                    // data go to the even cycle
                    if(scanlineDot != 65 && oamaddr == 0){
                        oamaddr = 4;
                    }
                    oamSpriteEvaluationData = oam1[oamaddr];

                    if(oamaddr % 4 == 0){
                        spriteY = false;
                        
                        if((scanlineRow & 0xFF) - oamSpriteEvaluationData >= 0 && (scanlineRow & 0xFF) - oamSpriteEvaluationData < ((ppuctrl & PPUCTRL) == PPUCTRL ? 16 : 8)){
                            spriteY = true;
                        }
                    }

                    oamaddr++; // 2. Increment n
                }
                // even cycle
                else {

                    // 3. Starting at m = 0, evaluate OAM[n][m] as a Y-coordinate.
                    if(spriteY == true && oam2Address < 32){
                        // if(oam2Address & 0x3 == 0x3){
                        //     SetFlag(SECTIONS6502::PPUSTATUS, PPU_OVERFLOW);
                        // }
                        
                        oam2[oam2Address] = oamSpriteEvaluationData;
                        oam2Address++;
                    }else{
                        // oamaddr += 4;
                        oamaddr +=3;
                    }
                    // not implemented
                    // 4. Attempt (and fail) to copy OAM[n][0] into the next free slot in secondary OAM, and increment n (repeat until HBLANK is reached)
                }
            }else if(scanlineDot >= 257 && scanlineDot <= 320){
                SpriteFetch();
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
            // int scrX =0;
            // int scrY =0;
            // for(int i=0; i< 61440;i++){
            //     if(scrX % 8 == 0 || scrY %8 == 8){
            //         pantalla[i].R = 255;
            //         pantalla[i].G = 255;
            //         pantalla[i].B = 255;
            //     }
            //     scrX++;
            //     if(scrX>=256){
            //         scrX = 0;
            //         scrY++;
            //     }
            // }
            for(int i=0; i< 61440;i++){
                DrawPixel(x,y,Color{pantalla[i].R,pantalla[i].G,pantalla[i].B,255});
                x++;
                cont++;
                if(cont == 256){
                    cont=0;
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


unsigned char debugAttribute=0;

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
        if(scanlineDot>=9){
            shiftRegisterLowBytes = (unsigned short) ((shiftRegisterLowBytes & 0xFF00) | tileLSB);
            shiftRegisterHighBytes = (unsigned short) ((shiftRegisterHighBytes & 0xFF00) | tileHSB);
            latchRegisterAttribute = attributeColor;
        }
    
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

        // if((debugAttribute != attributeColor) && debugPalattechange > 0x1F){
        // if(scanlineRow<32 || scanlineRow == 261){
        //     printf("pixel x,y: %d, %d\n",scanlineDot,scanlineRow);
        //     printf("nametable pixelRegister LB: 0x%x\n",shiftRegisterLowBytes);
        //     printf("nametable pixelRegister HB: 0x%x\n",shiftRegisterHighBytes);
        //     printf("\n");
        // }

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
        // 0xC00     0x38  0x7
        // |---|    |----||---|
        // |   |    |    ||   |
        //  NN  1111 YYY   XXX
        //  ||  |||| |||   +++-- high 3 bits of coarse X (x/4)
        //  ||  |||| +++-------- high 3 bits of coarse Y (y/4)
        //  ||  ++++------------ attribute offset (960 bytes)
        //  ++------------------ nametable select
        ppuAddress = (unsigned short)(0x23C0 | (registerV & 0x0C00) | ((registerV >> 4) & 0x38) | ((registerV >> 2) & 0x07));
    }else if(actualCycle == 3){
        attributeColor = FetchPPU(ppuAddress);

        // https://www.nesdev.org/wiki/PPU_attribute_tables
        //  |---- Top left
        //  3 1 - Top right
        //  2 2 - Bottom right
        //  |---- Bottom left
        // value = (bottomright << 6) | (bottomleft << 4) | (topright << 2) | (topleft << 0)
        //       = (2           << 6) | (2          << 4) | (1        << 2) | (3       << 0)
        //       = $80                | $20               | $04             | $03
        //       = $A7
        // If this is on the right tile

        // Use Coarse X from RegisterV (bits 4-0)
        if ((registerV & 3) >= 2){
            attributeColor = (attributeColor >> 2);
        }
        // If this is on the bottom tile
        // Use Coarse Y from RegisterV (bits 9-5)
        if ((((registerV & 0x3E0) >> 5) & 3) >= 2)
        {
            attributeColor = (attributeColor >> 4);
        }
        
        // we only save 2 bits
        attributeColor = (attributeColor & 3);
        // if((debugAttribute != attributeColor) && debugPalattechange > 0x1F){
        //     printf("pixel x,y: %d, %d\n",scanlineDot,scanlineRow);
        //     printf("nametable pixelRegister LB: 0x%x\n",shiftRegisterLowBytes);
        //     printf("nametable pixelRegister HB: 0x%x\n",shiftRegisterHighBytes);
        //     printf("attributeColor: 0x%x\n",attributeColor);
        // }
        debugAttribute = attributeColor;
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


    // if(scanlineRow<=33 || scanlineRow == 261){
    // if(scanlineRow>=31 || scanlineRow == 261){
    
    //     printf("fetch pixel x,y: %d, %d\n",scanlineDot,scanlineRow);
    //     printf("nametable pixelRegister LB: 0x%x\n",shiftRegisterLowBytes);
    //     printf("nametable pixelRegister HB: 0x%x\n",shiftRegisterHighBytes);
    //     printf("\n");
    // }
    // if(scanlineRow == 32){
    //     printf("\n");
    // }

    // 5)Turn the attribute data and the pattern table data into palette indices, and combine them with data from sprite data using priority.
    // https://www.nesdev.org/wiki/PPU_sprite_evaluati
    //First, it clears the list of sprites to draw.
    //Second, it reads through OAM, checking which sprites will be on this scanline. It chooses the first eight it finds that do.
    //Third, if eight sprites were found, it checks (in a wrongly-implemented fashion) for further sprites on the scanline to see if the sprite overflow flag should be set.
    //Fourth, using the details for the eight (or fewer) sprites chosen, it determines which pixels each has on the scanline and where to draw the
}

void PPU::SpriteFetch() {
    if((ppumask & PPU_FLAGS::SPRITE_ENABLE_BIT_4) == SPRITE_ENABLE_BIT_4){

        if(scanlineDot == 257){
            oamaddr = 0;
            oam2Address = 0;
        }
        
        // Cycles 1-64: Secondary OAM (32-byte buffer for current sprites on scanline) 
        // is initialized to $FF attempting to read $2004 will return $FF.
        // Internally, the clear operation is implemented
        // by reading from the OAM and writing into the secondary OAM as usual,
        // only a signal is active that makes the read always return $FF.
    
        // https://github.com/emu-russia/breaks/blob/master/BreakingNESWiki_DeepL/PPU/fifo.md
        // https://www.nesdev.org/2C02%20technical%20reference.TXT
        // Cycles 257-320: Sprite fetches (8 sprites total, 8 cycles per sprite)
        // 1-4: Read the Y-coordinate, tile number, attributes, and X-coordinate of the selected sprite from secondary OAM
        // 5-8: Read the X-coordinate of the selected sprite from secondary OAM 4 times (while the PPU fetches the sprite tile data)
        // For the first empty sprite slot, this will consist of sprite #63's Y-coordinate followed by 3 $FF bytes; for subsequent empty sprite slots, this will be four $FF bytes
                
        unsigned char actualCycle = (unsigned char) ((scanlineDot-1) & 0x7);
        // printf("tile fetch cycle: %d\n",actualCycle);
        //Each memory fetch takes 2 dots:
        // dot 1: the full address is placed onto the PPU's address bus
        //        and the low 8 bits are stored into an external address latch
        // dot 2: the read is performed.

        if(actualCycle == 0){
            spriteY = oam2[oam2Address]; // Save Y
            oam2Address++;
        }else if(actualCycle == 1){
            ppuAddress = oam2[oam2Address]; // Save Tile
            spriteTile = ppuAddress;
            oam2Address++;
        }else if(actualCycle == 2){
            if(((oam2[oam2Address] >> 7) & 1) == 0){
                VINV = false;
            }else{
                VINV = true;
            }
            if(((oam2[oam2Address] >> 6) & 1) == 0){
                HINV = false;
            }else{
                HINV = true;
            }
            // Save Attribute pallete (bit 0-1) and priority (bit 5)
            spriteBuffer[oam2Address/4].latchAttribute = ((oam2[oam2Address] & 0x20)>>3) | (oam2[oam2Address] & 3);
            oam2Address++;
        }else if(actualCycle == 3){
            spriteBuffer[oam2Address/4].posX = oam2[oam2Address]; // Save X
        }else if(actualCycle == 4){
            // oam2[oam2Address]; // Read X again
            GetSpriteAddress(); // update ppuAddress
        }else if(actualCycle == 5){
            // oam2[oam2Address]; // Read X again
            spriteBuffer[oam2Address/4].SRLowByte = FetchPPU(ppuAddress);
            if(HINV == true){
                spriteBuffer[oam2Address/4].SRLowByte = FlipSpriteHorizontal(spriteBuffer[oam2Address/4].SRLowByte);
            }

            // in-range check. (The pre-render line ends up checking scanline 5 due to the `& 0xFF`.
            if (!((scanlineRow & 0xFF) - spriteY >= 0 && (scanlineRow & 0xFF) - spriteY < ((ppuctrl & PPUCTRL) == PPUCTRL ? 16 : 8)))
            {
                spriteBuffer[oam2Address/4].SRLowByte = 0; // clear the value in this shift register if this object isn't in range.
            }
        }else if(actualCycle == 6){
            // oam2[oam2Address]; // Read X again
            GetSpriteAddress(); // update ppuAddress
            ppuAddress+=8;
        }else if(actualCycle == 7){
            // oam2[oam2Address]; // Read X again
            spriteBuffer[oam2Address/4].SRHighByte = FetchPPU(ppuAddress);

            if(HINV == true){
                spriteBuffer[oam2Address/4].SRHighByte = FlipSpriteHorizontal(spriteBuffer[oam2Address/4].SRHighByte);          
            }

            // in-range check. (The pre-render line ends up checking scanline 5 due to the `& 0xFF`.
            if (!((scanlineRow & 0xFF) - spriteY >= 0 && (scanlineRow & 0xFF) - spriteY < ((ppuctrl & PPUCTRL) == PPUCTRL ? 16 : 8)))
            {
                spriteBuffer[oam2Address/4].SRHighByte = 0; // clear the value in this shift register if this object isn't in range.
            }
            oam2Address++;
        }
    }
}

void PPU::GetSpriteAddress(){
    // More info: Details of object pattern fetch & render
    // 8x8 sprites
    if((ppuctrl & PPU_FLAGS::SPRITE_SIZE_FLAG) == 0){
        // The address is $0000 or $1000 depending on the nametable.
        // plus the pattern value from OAM * 16
        // plus the number of scanlines from the top of the object.
        // if the attributes are set to flip Y, it's 7 - the number of scanlines from the top of the object.
        if(VINV == false){
            ppuAddress = (unsigned short)(ppuctrl & PPU_FLAGS::SPRITE_PATTERN_FLAG ? 0x1000 : 0);
            ppuAddress += (spriteTile << 4) + ((scanlineRow & 0xFF) - spriteY);
        }else{
            ppuAddress = (unsigned short)(ppuctrl & PPU_FLAGS::SPRITE_PATTERN_FLAG ? 0x1000 : 0);
            ppuAddress += (spriteTile << 4) + ((7 - ((scanlineRow & 0xFF) - spriteY)) & 7);
        }
    }else{
        // 8x16 sprites
    }
}

unsigned char PPU::FlipSpriteHorizontal(unsigned char b){
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; // Swap nibbles
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2; // Swap pairs of bits
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1; // Swap adjacent bits
    return b;
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
        // printf("set pallete 0x%x: 0x%x\n",Address&0x1f,value);
        // printf("pc: 0x%x\n",emulator.cpu.pc);
        debugPalattechange++;
        paletteRAM[Address & 0x1F] = value;
    }
    else // if this is not pointing to CHR RAM or palettes
    {
        VRAM[Address & 0x7FF] = value;

    }
}

void PPU::StoreInPPU(unsigned char value, unsigned short address) {
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
        // printf("ppu data write: 0x%X\n",data);
    }else if(address == SECTIONS6502::OAMADDR){ // 0x2003
        ppuBus = value;
        oamaddr = value;
    }else if(address == SECTIONS6502::OAMDATA){ // 0x2004
        ppuBus = value;
        oamdata = value;
        oam1[oamaddr] = value;
        oamaddr++;

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
        // printf("2007 pc: 0x%X, V: 0x%X, Value: 0x%X\n",emulator.cpu.pc,registerV,value);
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
    }else if(address == SECTIONS6502::OAMDMA){ // 0x4014
        emulator.cpu.DMAPage = value;
        emulator.cpu.DMAAddress = 0; // always start with zero
    }
}
