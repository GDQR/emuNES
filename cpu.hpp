#pragma once
#include "cartridge.hpp"
#include <raylib.h>

const int memorySize = 65536; // 64kb
const int RAM_SIZE = 2048; // 2KB = 0x800 RAM
const int PPU_SIZE = 2048; // 2KB = 0x800 VRAM
const int startMemory = 256; // 0x100

enum SECTIONS6502{
    RAM = 0x0800,
    PPU_SECTION = 0x2000, // 0x2000
    PPUCTRL = 0x2000, // 0x2000
    PPUMASK = 0x2001, // 0x2001
    PPUSTATUS = 0x2002, // 0x2002
    OAMADDR = 0x2003, // 0x2003
    OAMDATA = 0x2004, // 0x2004
    PPUSCROLL = 0x2005, // 0x2005
    PPUADDR = 0x2006, // 0x2006
    PPUDATA = 0x2007, // 0x2007
    APU = 0x4000, // 0x4000
    OAMDMA = 0x4014, // 0x4014
    WRAM = 0x6000,
    ROM = 0x8000, // 0x8000
    NMI = 0xFFFA,
    RESET = 0xFFFC, //0xFFFC
    IRQ = 0xFFFE
};

enum CPUFLAGS
{
    CARRY = 1 << 0,
    ZERO = 1 << 1,
    INTERRUPT_DISABLE = 1 << 2,
    DECIMAL_MODE = 1 << 3,
    BREAK = 1 << 4,
    OVERFLOW = 1 << 6,
    NEGATIVE = 1 << 7
};

enum PPU_FLAGS
{
    // PPUCTRL FLAGS
    BASE_NAMETABLE_BIT1 = 1 << 0,
    BASE_NAMETABLE_BIT2 = 1 << 1,
    VRAM_ADDRESS_FLAG = 1 << 2,
    SPRITE_PATTERN_FLAG = 1 << 3,
    BACKGROUND_PATTERN_FLAG = 1 << 4,
    SPRITE_SIZE_FLAG = 1 << 5,
    PPU_MASTER_SLAVE = 1 << 6,
    NMI_FLAG = 1 << 7,

    //PPUMASK FLAGS
    GREYSCALE_BIT_0 = 1 << 0,
    BACKGROUND_LEFT_COLUMN_BIT_1 = 1 << 1,
    SPRITE_LEFT_COLUMN_BIT_2 = 1 << 2,
    BACKGROUND_ENABLE_BIT_3 = 1 << 3,
    SPRITE_ENABLE_BIT_4 = 1 << 4,
    COLOR_R_BIT_5 = 1 << 5,
    COLOR_G_BIT_6 = 1 << 6,
    COLOR_B_BIT_7 = 1 << 7,

 
    // PPUSTATUS FLAGS
    PPU_OVERFLOW = 1 << 4,
    PPU_SPRITEHIT = 1 << 6,
    PPU_VBLANK = 1 << 7
};

struct CPU6502
{
    unsigned int totalCycles = 0;
    unsigned short dataAddress;
    unsigned short pcdebug; // debug only
    unsigned short pc; // Program Counter
    unsigned char sp;  // Stack Pointer
    unsigned char dataBus;
    unsigned char PPUBus;
    unsigned char opcode;
    unsigned char debugCycle;
    unsigned char cycleCounter;
    unsigned char cycleCounterMax = 0;
    unsigned char accumulator;
    unsigned char registerX;
    unsigned char registerY;
    unsigned char statusFlag;
    bool beforeNMI = false;
    bool nmi;
    bool irq;
    // unsigned char memory[memorySize];
    unsigned char RAM[RAM_SIZE];
    void Initialize();
    void SetFlag(CPUFLAGS flag);
    void ClearFlag(CPUFLAGS flag);
    void Reset();
    void CheckPageCrossing(unsigned short pc);
    void isNegative(unsigned char value);
    void isZero(unsigned char value);
    unsigned short FetchAccumulator(unsigned short address); 
    unsigned char FetchImmediate(unsigned short address);
    // 1 Cycles
    unsigned short FetchZeroPage(unsigned short address);
    unsigned short FetchZeroPageX(unsigned short address);
    unsigned short FetchZeroPageY(unsigned short address);
    unsigned short FetchRelative(unsigned short address);
    // 2 Cycles
    unsigned short FetchAbsolute(unsigned short address);
    // 2 Cycles
    unsigned short FetchAbsoluteX(unsigned short address);
    // 2 Cycles
    unsigned short FetchAbsoluteY(unsigned short address);
    // 4 Cycles
    unsigned short FetchIndirect(unsigned short address);
    // Indexed Indirect. 3 Cycles
    unsigned short FetchIndirectX(unsigned short address);
    // Indirect Indexed. 3 Cycles
    unsigned short FetchIndirectY(unsigned short address);
    void StoreInCPUMemory(unsigned char value, unsigned short address);
    void Ins_ADC();
    void Ins_AND();
    void Ins_ASL();
    void Ins_BCC();
    void Ins_BCS();
    void Ins_BEQ();
    void Ins_BIT();
    void Ins_BMI();
    void Ins_BNE();
    void Ins_BPL();
    void Ins_BRK();
    void Ins_BVC();
    void Ins_BVS();
    void Ins_CLC();
    void Ins_CLD();
    void Ins_CLI();
    void Ins_CLV();
    void Ins_CMP();
    void Ins_CPX();
    void Ins_CPY();
    void Ins_DEC();
    void Ins_DEX();
    void Ins_DEY();
    void Ins_EOR();
    void Ins_INC();
    void Ins_INX();
    void Ins_INY();
    void Ins_JMP();
    void Ins_JSR();
    void Ins_LDA();
    void Ins_LDX();
    void Ins_LDY();
    void Ins_LSR();
    void Ins_NOP();
    void Ins_ORA();
    void Ins_PHA();
    void Ins_PHP();
    void Ins_PLA();
    void Ins_PLP();
    void Ins_ROL();
    void Ins_ROR();
    void Ins_RTI();
    void Ins_RTS();
    void Ins_SBC();
    void Ins_SEC();
    void Ins_SED();
    void Ins_SEI();
    void Ins_STA();
    void Ins_STX();
    void Ins_STY();
    void Ins_TAX();
    void Ins_TAY();
    void Ins_TSX();
    void Ins_TXA();
    void Ins_TXS();
    void Ins_TYA();
    void EmulateCycle();
};


// clang-format off
const char cycles[256] = {
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 2, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    // clang-format on
};

enum PPURENDER_STATE{
    PRE_RENDER, // scanline 261
    NORMAL_RENDER, // scanline 0 to scanline 239
    POST_RENDER // scanline 240 to scanline 260
};

// NTSC- MAX CYCLE 340
enum PPU_SPECIALCYCLES{
    MAX_VISIBLE_SCANLINES = 256, // no se que es,  supongo que la pantalla
    FINAL_SCANLINE = 340
};

// OAM DATA for rendering
struct SPRITE_MEMORY{
    unsigned char posY; // sprite Y position
    unsigned char tileIndex;
    unsigned char attribute;
    unsigned char posX; // sprite X position
};

struct PPU
{
    PPURENDER_STATE renderState;
    unsigned short cycleCounter;
    unsigned short scanlineDot; // X from screen
    unsigned short scanlineRow; // Y from screen
    // Registers
    unsigned short ppuscroll;
    unsigned char ppuctrl; // 0x2000
    unsigned char ppumask; // 0x2001 Rendering settings
    unsigned char ppustatus;
    unsigned char oamaddr;
    unsigned char oamdata;
    unsigned char ppudata;
    unsigned char oamdma;
    unsigned char ppuBus;
    unsigned char renderValue;
    unsigned char attributeValue;
    unsigned short ppuAddress;
    // Internal Registers. More info:
    // https://www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
    // https://www.nesdev.org/wiki/PPU_registers#Internal_registers

    unsigned short registerV; // Current VRAM Address/read-write-address

    // During rendering, specifies the starting coarse-x scroll for the next scanline and the starting y scroll for the screen. 
    // Outside of rendering, holds the scroll or VRAM address before transferring it to v.
    // Temporary VRAM Address
    unsigned short registerT; 
    unsigned char registerX; // Fine X scroll
    bool registerW; // First or second write toggle
    bool useCHRRAM;
    unsigned short shiftRegisterLowBytes; // Fetch pixels
    unsigned short shiftRegisterHighBytes; // Fetch pixels
    unsigned char latchRegisterAttribute; // Fetch 2 bits
    unsigned char shiftRegisterAttribute1;
    unsigned char shiftRegisterAttribute2;
    unsigned char tileLSB;
    unsigned char tileHSB;
    unsigned char tileValue;
    unsigned char nametableByte; //only debug
    unsigned char attributeColor; // Save the Palette Color and is set in latchRegisterAttribute
    unsigned char paletteRAM[0x1F];
    SPRITE_MEMORY oamSprite[64]; // 64*4 = 256 bytes
    unsigned char VRAM[PPU_SIZE];// maybe useless, instead use the ram from cartridge
    bool isEvenFrame =true;
    void Initialize();
    void Power();
    void Reset();
    void EmulateCycle();
    void ShiftRegisterMoveBit();
    unsigned char FetchPPU(unsigned short Address);
    unsigned short MirroringAddress(unsigned short Address);
    void TileFetch();
    void GetPallete();
    void StorePPUData(unsigned char value, unsigned short Address);
    void StoreInPPU(unsigned char value, unsigned short address);
    void SetFlag(SECTIONS6502 typeRegister, PPU_FLAGS insertFlag);
    void ClearFlag(SECTIONS6502 typeRegister, PPU_FLAGS insertFlag);
};

struct RGBstruct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

class Emulator{
    public:
    CPU6502 cpu;
    PPU ppu;
    Cartridge* game;
    // RGBstruct screenData[256*240]; // this gives error in memory
    // RGBstruct screenData[256][256];
    // Image screen;
    void Initialize(const char* file);
    void Cycle();
    unsigned char Fetch(unsigned short address);
    unsigned char MapperFetch(unsigned short address, unsigned char mapperType);
};

extern Emulator emulator;
extern RenderTexture2D target;
extern int gameScreenWidth;
extern int gameScreenHeight;
extern float scale;