#pragma once

enum INES{
    PRG_SIZE = 4,
    CHR_SIZE = 5
};

struct Cartridge
{
    unsigned char* PRGROM; // Program ROM
    unsigned char* CHRROM; // Character ROM
    unsigned char* PRGRAM;
    unsigned char* CHRRAM;
    unsigned int PRG_Size;
    unsigned int CHR_Size;
    unsigned char mapper;
    unsigned char mirroring;

    void LoadGame(const char* file);
};
