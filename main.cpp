#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.hpp"
#include "rayWindows.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "sharedDataMemory.hpp"

using namespace boost::interprocess;

bool rayWindow_Main = true;
bool rayWindow_Debugger = false;
unsigned int debugCol = 0;

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

// clang-format off
enum Opcode
{
    // ADD WITH CARRY
    ADC_IMMEDIATE_69 = 0X69,
    ADC_ZEROPAGE_65  = 0X65,
    ADC_ZEROPAGEX_75 = 0X75,
    ADC_ABSOLUTE_6D  = 0X6D,
    ADC_ABSOLUTEX_7D = 0X7D,
    ADC_ABSOLUTEY_79 = 0X79,
    ADC_INDIRECTX_61 = 0X61,
    ADC_INDIRECTY_71 = 0X71,

    // LOGICAL AND
    AND_IMMEDIATE_29 = 0X29,
    AND_ZEROPAGE_25  = 0X25,
    AND_ZEROPAGEX_35 = 0X35,
    AND_ABSOLUTE_2D  = 0X2D,
    AND_ABSOLUTEX_3D = 0X3D,
    AND_ABSOLUTEY_39 = 0X39,
    AND_INDIRECTX_21 = 0X21,
    AND_INDIRECTY_31 = 0X31,

    // ARITHMETIC SHIFT LEFT
    ASL_ACCUMULATOR_0A = 0X0A,
    ASL_ZEROPAGE_06    = 0X06,
    ASL_ZEROPAGEX_16   = 0X16,
    ASL_ABSOLUTE_0E    = 0X0E,
    ASL_ABSOLUTEX_1E   = 0X1E,

    // BRANCH IF CARRY CLEAR
    BCC_RELATIVE_90 = 0X90,

    // BRANCH IF CARRY SET
    BCS_RELATIVE_B0 = 0XB0,

    // BRANCH IF CARRY EQUAL
    BEQ_RELATIVE_F0 = 0XF0,

    // BIT TEST
    BIT_ZEROPAGE_24 = 0X24,
    BIT_ABSOLUTE_2C = 0X2C,

    // BRANCH IF MINUS
    BMI_RELATIVE_30 = 0X30,

    // BRANCH IF NOT EQUAL
    BNE_RELATIVE_D0 = 0XD0,

    // BRANCH IF POSITIVE
    BPL_RELATIVE_10 = 0X10,

    // BREAK: FORCE INTERRUPT
    BRK_00 = 0X00,

    // BRANCH IF OVERFLOW CLEAR
    BVC_RELATIVE_50 = 0X50,

    // BRANCH IF OVERFLOW SET
    BVS_RELATIVE_70 = 0X70,

    // CLEAR CARRY FLAG
    CLC_18 = 0X18,

    // CLEAR DECIMAL MODE
    CLD_D8 = 0XD8,

    // CLEAR INTERRUPT DISABLE
    CLI_58 = 0X58,

    // CLEAR OVERFLOW FLAG
    CLV_B8 = 0XB8,

    // COMPARE
    CMP_IMMEDIATE_C9 = 0XC9,
    CMP_ZEROPAGE_C5  = 0XC5,
    CMP_ZEROPAGEX_D5 = 0XD5,
    CMP_ABSOLUTE_CD  = 0XCD,
    CMP_ABSOLUTEX_DD = 0XDD,
    CMP_ABSOLUTEY_D9 = 0XD9,
    CMP_INDIRECTX_C1 = 0XC1,
    CMP_INDIRECTY_D1 = 0XD1,

    // COMPARE X REGISTER
    CPX_IMMEDIATE_E0 = 0XE0,
    CPX_ZEROPAGE_E4  = 0XE4,
    CPX_ABSOLUTE_EC  = 0XEC,

    // COMPARE Y REGISTER
    CPY_IMMEDIATE_C0 = 0XC0,
    CPY_ZEROPAGE_C4  = 0XC4,
    CPY_ABSOLUTE_CC  = 0XCC,

    // DECREMENT MEMORY
    DEC_ZEROPAGE_C6  = 0XC6,
    DEC_ZEROPAGEX_D6 = 0XD6,
    DEC_ABSOLUTE_CE  = 0XCE,
    DEC_ABSOLUTEX_DE = 0XDE,

    // DECREMENT X REGISTER
    DEX_CA = 0XCA,

    // DECREMENT Y REGISTER
    DEY_88 = 0X88,

    // EXCLUSIVE ORA
    EOR_IMMEDIATE_49 = 0X49,
    EOR_ZEROPAGE_45  = 0X45,
    EOR_ZEROPAGEX_55 = 0X55,
    EOR_ABSOLUTE_4D  = 0X4D,
    EOR_ABSOLUTEX_5D = 0X5D,
    EOR_ABSOLUTEY_59 = 0X59,
    EOR_INDIRECTX_41 = 0X41,
    EOR_INDIRECTY_51 = 0X51,

    // INCREMENT MEMORY
    INC_ZEROPAGE_E6  = 0XE6,
    INC_ZEROPAGEX_F6 = 0XF6,
    INC_ABSOLUTE_EE  = 0XEE,
    INC_ABSOLUTEX_FE = 0XFE,

    // INCREMENT X REGISTER
    INX_E8 = 0XE8,

    // INCREMENT Y REGISTER
    INY_C8 = 0XC8,

    // JUMP
    JMP_ABSOLUTE_4C = 0X4C,
    JMP_INDIRECT_6C = 0X6C,

    // JUMP TO SUBROUTINE
    JSR_ABSOLUTE_20 = 0X20,

    // LOAD ACCUMULATOR
    LDA_IMMEDIATE_A9 = 0XA9,
    LDA_ZEROPAGE_A5  = 0XA5,
    LDA_ZEROPAGEX_B5 = 0XB5,
    LDA_ABSOLUTE_AD  = 0XAD,
    LDA_ABSOLUTEX_BD = 0XBD,
    LDA_ABSOLUTEY_B9 = 0XB9,
    LDA_INDIRECTX_A1 = 0XA1,
    LDA_INDIRECTY_B1 = 0XB1,

    // LOAD X REGISTER
    LDX_IMMEDIATE_A2 = 0XA2,
    LDX_ZEROPAGE_A6  = 0XA6,
    LDX_ZEROPAGEY_B6 = 0XB6,
    LDX_ABSOLUTE_AE  = 0XAE,
    LDX_ABSOLUTEY_BE = 0XBE,

    // LOAD Y REGISTER
    LDY_IMMEDIATE_A0 = 0XA0,
    LDY_ZEROPAGE_A4  = 0XA4,
    LDY_ZEROPAGEX_B4 = 0XB4,
    LDY_ABSOLUTE_AC  = 0XAC,
    LDY_ABSOLUTEX_BC = 0XBC,

    // LOGICAL SHIFT RIGHT
    LSR_ACCUMULATOR_4A = 0X4A,
    LSR_ZEROPAGE_46    = 0X46,
    LSR_ZEROPAGEX_56   = 0X56,
    LSR_ABSOLUTE_4E    = 0X4E,
    LSR_ABSOLUTEX_5E   = 0X5E,

    // NO OPERATION
    NOP_EA = 0XEA,

    // LOGICAL INCLUSIVE OR
    ORA_IMMEDIATE_09 = 0X09,
    ORA_ZEROPAGE_05 = 0X05,
    ORA_ZEROPAGEX_15 = 0X15,
    ORA_ABSOLUTE_0D = 0X0D,
    ORA_ABSOLUTEX_1D = 0X1D,
    ORA_ABSOLUTEY_19 = 0X19,
    ORA_INDIRECTX_01 = 0X01,
    ORA_INDIRECTY_11 = 0X11,

    // PUSH ACCUMULATOR
    PHA_48 = 0X48,

    // PUSH PROCESSOR STATUS
    PHP_08 = 0X08,

    // PULL ACCUMULATOR
    PLA_68 = 0X68,

    // PULL PROCESSOR STATUS
    PLP_28 = 0X28,

    // ROTATE LEFT
    ROL_ACCUMULATOR_2A = 0X2A,
    ROL_ZEROPAGE_26 = 0X26,
    ROL_ZEROPAGEX_36 = 0X36,
    ROL_ABSOLUTE_2E = 0X2E,
    ROL_ABSOLUTEX_3E = 0X3E,

    // ROTATE RIGHT
    ROR_ACCUMULATOR_6A = 0X6A,
    ROR_ZEROPAGE_66 = 0X66,
    ROR_ZEROPAGEX_76 = 0X76,
    ROR_ABSOLUTE_6E = 0X6E,
    ROR_ABSOLUTEX_7E = 0X7E,

    // ROTATE FROM INTERRUPT
    RTI_40 = 0X40,

    // ROTATE FROM SUBROUTINE
    RTS_60 = 0X60,

    // SUBTRACT WITH CARRY
    SBC_IMMEDIATE_E9 = 0XE9,
    SBC_ZEROPAGE_E5  = 0XE5,
    SBC_ZEROPAGEX_F5 = 0XF5,
    SBC_ABSOLUTE_ED  = 0XED,
    SBC_ABSOLUTEX_FD = 0XFD,
    SBC_ABSOLUTEY_F9 = 0XF9,
    SBC_INDIRECTX_E1 = 0XE1,
    SBC_INDIRECTY_F1 = 0XF1,

    // SET CARRY FLAG
    SEC_38 = 0X38,

    // SET DECIMAL FLAG
    SED_F8 = 0XF8,

    // SET INTERRUPT DISABLE
    SEI_78 = 0X78,

    // STORE ACCUMULATOR
    STA_ZEROPAGE_85  = 0X85,
    STA_ZEROPAGEX_95 = 0X95,
    STA_ABSOLUTE_8D  = 0X8D,
    STA_ABSOLUTEX_9D = 0X9D,
    STA_ABSOLUTEY_99 = 0X99,
    STA_INDIRECTX_81 = 0X81,
    STA_INDIRECTY_91 = 0X91,

    // STORE X REGISTER
    STX_ZEROPAGE_86  = 0X86,
    STX_ZEROPAGEY_96 = 0X96,
    STX_ABSOLUTE_8E  = 0X8E,

    // STORE Y REGISTER
    STY_ZEROPAGE_84  = 0X84,
    STY_ZEROPAGEX_94 = 0X94,
    STY_ABSOLUTE_8C  = 0X8C,

    // TRANSFER ACCUMULATOR TO X
    TAX_AA = 0XAA,

    // TRANSFER ACCUMULATOR TO Y
    TAY_A8 = 0XA8,

    // TRANSFER STACK POINTER TO X
    TSX_BA = 0XBA,

    // TRANSFER X TO ACCUMULATOR
    TXA_8A = 0X8A,

    // TRANSFER X TO STACK POINTER
    TXS_9A = 0X9A,

    // TRANSFER Y TO ACCUMULATOR
    TYA_98 = 0X98,
};
// clang-format on


Emulator emulator;


void CPU6502::Initialize()
{
    // RESERVED MEMORY
    // $0000 - $00FF : ZERO PAGE
    // $0100 - $01FF : SYSTEM STACK
    // $FFFA - $FFFB : NON-MASKABLE INTERRUPT HANDLER
    // $FFFC - $FFFD : RESET
    // $FFFE - $FFFF : BREAK/INTERRUPT REQUEST HANDLER
    // pc = Fetch(0xFFFD) | (Fetch(0xFFFC) << 8);
    cycleCounter = 0;
    pc = 0xFFFF;
    opcode = 0;
    sp = (unsigned char) 0x01FD;
    accumulator = 0;
    registerX = 0;
    registerY = 0;
    ClearFlag(CPUFLAGS::CARRY);
    ClearFlag(CPUFLAGS::ZERO);
    SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
    ClearFlag(CPUFLAGS::DECIMAL_MODE);
    ClearFlag(CPUFLAGS::OVERFLOW);
    ClearFlag(CPUFLAGS::NEGATIVE);

    // for(size_t i=0; i < memorySize; i++){
    //     memory[i] = 0;
    // }
    Reset();
}

void CPU6502::Reset(){
    unsigned short address = (unsigned short) (FetchImmediate(SECTIONS6502::RESET) | FetchImmediate(SECTIONS6502::RESET+1) << 8); //(unsigned short) (Fetch(SECTIONS6502::RESET) | Fetch(SECTIONS6502::RESET) << 8);
    accumulator = registerX = registerY = 0;
    SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
    printf("RESET ADDRESS: 0x%X\n",address);
    pc = address;
    sp = (unsigned char) 0x1FD;
    cycleCounter = 0;
    cycleCounterMax = 0;
}

unsigned char Emulator::MapperFetch(unsigned short address, unsigned char mapperType){
    switch (mapperType)
    {
    case 0:
        if(address >= 0x8000){
            return game->PRGROM[address & (game->PRG_Size-1)];
        }
    default:
        break;
    }
    return 0;
}

unsigned char Emulator::Fetch(unsigned short address){
    unsigned char data = 0;
    if(address >= SECTIONS6502::ROM){
        data = MapperFetch(address, game->mapper);
        cpu.cycleCounter++;
        return data;
    }else if(address < SECTIONS6502::PPU_SECTION){
        cpu.cycleCounter++;
        return cpu.RAM[address & 0x7ff];
    }else if (address >= SECTIONS6502::PPU_SECTION && address < SECTIONS6502::APU){
        cpu.cycleCounter++;
        if(address == SECTIONS6502::PPUCTRL){ // 0x2000 Write only
            return ppu.ppuBus;
        }else if(address == SECTIONS6502::PPUMASK){ // 0x2001 Write only
            return ppu.ppuBus;
        }else if(address == SECTIONS6502::PPUSTATUS){ // 0x2002
            data = ppu.ppustatus + (ppu.ppuBus & 0x1F) ; // only need bit 0 to bit 5 of PPUBus
            ppu.ClearFlag(SECTIONS6502::PPUSTATUS, PPU_FLAGS::PPU_VBLANK);
            ppu.registerW = false; // clear register
            return data;
        }else if(address == SECTIONS6502::OAMADDR){ // 0x2003 Write only
            return ppu.ppuBus;
        }else if(address == SECTIONS6502::OAMDATA){ // 0x2004
            // data = ReadOAM();
            printf("readOAMDATA\n");
            while (true){}
            
            return data;
        }else if(address == SECTIONS6502::PPUSCROLL){ // 0x2005 Write only
            return ppu.ppuBus;
        }else if(address == SECTIONS6502::PPUADDR){ // 0x2006 Write only
            return ppu.ppuBus;
        }else if(address == SECTIONS6502::PPUDATA){ // 0x2007
            if(ppu.registerV >= 0x3F00){
                printf("cpu registerV\n");
                ppu.ppuAddress = ppu.registerV;
                if((ppu.ppuctrl & PPU_FLAGS::VRAM_ADDRESS_FLAG) == PPU_FLAGS::VRAM_ADDRESS_FLAG){
                    // if bit is one (1)
                    ppu.registerV++;
                }else{
                    ppu.registerV += 32;
                    // if bit is zero (0)
                }
                return ppu.FetchPPU((unsigned short)(ppu.ppuAddress & 0x3FFF));
            }

            return ppu.registerV;
        }
        cpu.cycleCounter++;
    }else if(address >= 0x4000 && address <= 0x401F){
        if (address == 0x4016){
            unsigned char inputData = ((controller.controllerShiftRegister1 & 0x80)>>7) | (cpu.dataBus & 0xE0);
            printf("4016: 0x%X\n",inputData);
            controller.shiftCounter1 = 2;
            emulator.cpu.dataBus = inputData;
            cpu.cycleCounter++;
            return inputData;
        }else if(address == 0x4017){
        }else if(address == 0x4014){
            
            printf("4014\n");
            while (true)
            {
            }
        }else{
            // printf("APU, DMC\n");
            // printf("not more, PC: 0x%X\n",emulator.cpu.pc);
            // while (true)
            // {
            // }
        }
        cpu.cycleCounter++;
    }
    // pc++;
    // cycleCounter++;
    return 0;
}

void CPU6502::StoreInCPUMemory(unsigned char value, unsigned short address)
{
    if(address < 0x2000){
        RAM[address & 0x7ff] = value;
    }else if (address < 0x4000){
        // $2000 through $3FFF writes to the PPU registers
        emulator.ppu.StoreInPPU(value, address);
    }else if(address == 0x4000){
        
    }else if(address == 0x4014){ // OAMDMA WRITE
        emulator.ppu.StoreInPPU(value, address);
        DMAactive = true;
    }else if(address == 0x4016){
        controller.controllerStrobe = value; // Activate/Desactivate read of controller
    }
}


unsigned short CPU6502::FetchAccumulator(unsigned short address){
    unsigned short newAddress = 0;
    return newAddress;
}

unsigned char CPU6502::FetchImmediate(unsigned short address){
    unsigned char newAddress = emulator.Fetch(address);
    return newAddress;
}

unsigned short CPU6502::FetchZeroPage(unsigned short address){
    unsigned short newAddress = (unsigned short) (0x0000 | emulator.Fetch(address));
    return newAddress;
}

unsigned short CPU6502::FetchZeroPageX(unsigned short address){
    unsigned short newAddress = (unsigned short) (0x0000 | emulator.Fetch(address)) + registerX;
    return newAddress;
}
unsigned short CPU6502::FetchZeroPageY(unsigned short address){
    unsigned short newAddress = (unsigned short) (0x0000 | emulator.Fetch(address)) + registerY;
    return newAddress;
}
unsigned short CPU6502::FetchRelative(unsigned short address){
    unsigned short newAddress = 0;
    return newAddress;
}
unsigned short CPU6502::FetchAbsolute(unsigned short address){ // 2 Cycles
    unsigned short newAddress = (unsigned short) (emulator.Fetch(address) | emulator.Fetch(address+1) << 8);
    return newAddress; 
}
unsigned short CPU6502::FetchAbsoluteX(unsigned short address){ 
    unsigned short newAddress = (unsigned short) (emulator.Fetch(address) | emulator.Fetch(address+1) << 8);
    newAddress += registerX;
    return newAddress;
}
unsigned short CPU6502::FetchAbsoluteY(unsigned short address){
    unsigned short newAddress = (unsigned short) (emulator.Fetch(address) | emulator.Fetch(address+1) << 8);
    newAddress += registerY;
    return newAddress;
}
unsigned short CPU6502::FetchIndirect(unsigned short address){ // 4 Cycles
    unsigned short Address1 = (unsigned short) (emulator.Fetch(address) | emulator.Fetch(address)); //ccbb = xx
    unsigned short Address2 = Address1 + 1; //ccbb + 1 = yy
    unsigned short newAddress = ((unsigned short) (emulator.Fetch(Address1) | emulator.Fetch(Address2)<<8)); //yyxx
    return newAddress;
}
unsigned short CPU6502::FetchIndirectX(unsigned short address){ // 3 Cycles
    unsigned char bb = emulator.Fetch(address); //bb
    unsigned short address1 = (unsigned short) (0x0000 | bb) + registerX; //00bb + RegisterX = xx
    unsigned short address2 = address1 + 1; //00bb + RegisterX + 1 = yy
    unsigned short newAddress = ((unsigned short) (emulator.Fetch(address1) | emulator.Fetch(address2)<<8)); //yyxx
    return newAddress;
}

unsigned short CPU6502::FetchIndirectY(unsigned short address){ // 3 Cycles
    unsigned char bb = emulator.Fetch(address); //bb
    unsigned short zeroAddress1 = (unsigned short) (0x0000 | bb); //00bb = xx
    unsigned short zeroAddress2 = zeroAddress1 + 1; //00bb + 1 = yy
    unsigned char res1 = emulator.Fetch(zeroAddress1);
    unsigned char res2 = emulator.Fetch(zeroAddress2);
    unsigned short newAddress = ((unsigned short) (res1 | res2<<8)) + registerY; //yyxx + RegisterY
    return newAddress;
}

void CPU6502::SetFlag(CPUFLAGS insertFlag)
{
    statusFlag |= insertFlag;
    // if(insertFlag == CPUFLAGS::INTERRUPT_DISABLE){ } // only when this was before zero
}

void CPU6502::ClearFlag(CPUFLAGS insertFlag)
{
    statusFlag &= ~((unsigned char)insertFlag);
}

void CPU6502::CheckPageCrossing(unsigned short pc){
    if(((pc+1)>>8) != (pc>>8)){
        cycleCounterMax++;
    }
}

// if value have the 7 bit set is negative
void CPU6502::isNegative(unsigned char value){
    if (value >= 0x80) // 0x80 = 1000-0000
    {
        SetFlag(CPUFLAGS::NEGATIVE);
    }else{
        ClearFlag(CPUFLAGS::NEGATIVE);
    }
}

void CPU6502::isZero(unsigned char value){
    if (value == 0){
        SetFlag(CPUFLAGS::ZERO);
    }else{
        ClearFlag(CPUFLAGS::ZERO);
    }
}

//CYCLE ACCURACY:
// 69,65,6D
void CPU6502::Ins_ADC(){
    if(opcode == ADC_IMMEDIATE_69){
        // max 2 cycles
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == ADC_ZEROPAGE_65){
        // max 3 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
        }
    }else if(opcode == ADC_ZEROPAGEX_75){
        // max 4 cycles
        dataAddress = FetchZeroPageX(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == ADC_ABSOLUTE_6D){
        // max 4 cycles
        if(cycleCounter == 1){
            dataBus = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) (dataBus | (emulator.Fetch(pc)<<8));
            pc++;
        }else if(cycleCounter == 3){            
            dataBus = emulator.Fetch(dataAddress);
        }
    }else if(opcode == ADC_ABSOLUTEX_7D){
        // max 4 or 5 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);

            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }

        }else if(cycleCounter == 4){
            cycleCounter++;
        }
    }else if(opcode == ADC_ABSOLUTEY_79){
        // max 4 or 5cycles
        dataAddress = FetchAbsoluteY(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == ADC_INDIRECTX_61){
        // max 6 cycles
        dataAddress = FetchIndirectX(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == ADC_INDIRECTY_71){
        // max 5 cycles
        dataAddress = FetchIndirectY(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }

    if(opcode != ADC_IMMEDIATE_69 && opcode != ADC_ZEROPAGE_65 && opcode != ADC_ABSOLUTE_6D && opcode != ADC_ABSOLUTEX_7D){
        printf("ADC NOT WORK: 0x%X\n",opcode);
        printf("pc: 0x%x\n",pc);
        while (true)
        {
            /* code */
        }
    }

    if(cycleCounter == cycleCounterMax){
        short value = accumulator + dataBus;
        if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
            value += 1;
        }
        
        if((~(accumulator ^ dataBus) & (accumulator ^ value) & 0x80) != 0){
            SetFlag(CPUFLAGS::OVERFLOW);
        }else{
            ClearFlag(CPUFLAGS::OVERFLOW);
        }
        if(value > 0xFF){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        accumulator = (unsigned char) value;
        isZero(accumulator);
        isNegative(accumulator);
    }
}

//CYCLE ACCURACY
// 29, 25, 35
void CPU6502::Ins_AND()
{
    if(opcode == AND_IMMEDIATE_29){
        // cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
        accumulator = accumulator & dataBus;
        isZero(accumulator);
        isNegative(accumulator);
    }else if(opcode == AND_ZEROPAGE_25){
        // max 3 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);

            accumulator = accumulator & dataBus;
            isZero(accumulator);
            isNegative(accumulator);
        }
    }else if(opcode == AND_ZEROPAGEX_35){
        // max 4 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress); // dummy read
            dataAddress += registerX;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);

            accumulator = accumulator & dataBus;
            isZero(accumulator);
            isNegative(accumulator);
        }
    }else if(opcode == AND_ABSOLUTE_2D){
        // max 4 cycles
        dataAddress = FetchAbsolute(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == AND_ABSOLUTEX_3D){
        // max 4 or 5 cycles
        dataAddress = FetchAbsoluteX(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == AND_ABSOLUTEY_39){
        // max 4 or 5 cycles
        dataAddress = FetchAbsoluteY(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == AND_INDIRECTX_21){
        // max 6 cycles
        dataAddress = FetchIndirectX(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == AND_INDIRECTY_31){
        // max 5 or 6 cycles
        dataAddress = FetchIndirectY(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }

    if(opcode != AND_ZEROPAGEX_35 && opcode != AND_IMMEDIATE_29 && opcode != AND_ZEROPAGE_25){       
        printf("AND not work: 0x%X\n",opcode);
        while (true){}
        
    }
}

// CYCLE ACCURACY:
// 06, 0A
void CPU6502::Ins_ASL()
{
    if(opcode == ASL_ZEROPAGE_06){
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc);//FetchZeroPage(pc);
            pc++;
        }else if (cycleCounter == 2){
            //cycle 3
            dataBus = emulator.Fetch(dataAddress);
        } else if(cycleCounter == 3){
            //cycle 4
            StoreInCPUMemory(dataBus, dataAddress); // dummy write
            cycleCounter++;
            
            if (dataBus >= 0x80){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }

            dataBus = dataBus << 1;
            
            isZero(dataBus);
            isNegative(dataBus);
        }else if(cycleCounter == 4){
            // cycle 5
            StoreInCPUMemory(dataBus,dataAddress);
            cycleCounter++;
        }
    }else if(opcode == ASL_ACCUMULATOR_0A){
        if(cycleCounter == 1){
            //cycle 2
            if (accumulator >= 0x80){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }

            accumulator = accumulator << 1;
            isZero(accumulator);
            isNegative(accumulator);
            cycleCounter++;
        }
    }else if(opcode == ASL_ABSOLUTE_0E){
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = FetchAbsolute(pc);
            pc++;
            pc++;
        } else if( cycleCounter == 3 || cycleCounter == 4){
            //cycle 4 and cycle 5
            cycleCounter++;
        }else if(cycleCounter == 5){
            //cycle 6
            // value = emulator.Fetch(dataAddress);
        }
    }else if(opcode == ASL_ZEROPAGEX_16){
        switch (cycleCounter){
        case 1:
            dataAddress = FetchZeroPageX(pc);
            pc++;
            break;
        case 2:
        case 3:
            //cycle 3 and 4
            cycleCounter++;
            break;
        case 4:
            //cycle 5
            // value = emulator.Fetch(dataAddress);
            break;
        default:
            break;
        }
    }else if(opcode == ASL_ABSOLUTEX_1E){
        switch (cycleCounter)
        {
        // case 0:
        //     // operator    
        //     break;
        case 1:
            dataAddress = FetchAbsoluteX(pc);
            pc++;
            pc++;
            break;

        case 2:
        case 3:
        case 4:
        case 5:
            cycleCounter++;
            break;
        case 6:
            //cycle 7
            // value = emulator.Fetch(dataAddress);
            break;
        default:
            printf("ASL_ABSOLUTEX_1E ERROR\n");
            while (true){}
            break;
        }
    }

    if(opcode != ASL_ZEROPAGE_06 && opcode != ASL_ACCUMULATOR_0A){
        printf("ASL NOT WORK, OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}

void CPU6502::Ins_BCC() {
    if(cycleCounter == 1){
        // cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
        if((statusFlag & CPUFLAGS::CARRY) == 0){
            cycleCounterMax += 1; // Carry flag is clear
        }
    }else if(cycleCounter == 2){
        // cycle 3
        unsigned short pcBranch;
        
        if(dataBus >= 0x80){
            // is negative
            pcBranch = pc - (256-dataBus);
        }else{
            pcBranch = pc + dataBus;
        }
        
        if((pc & 0xFF00) != (pcBranch & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }

        pc = pcBranch;
        cycleCounter++;
    }else if(cycleCounter == 3){
        //cycle 4
        // DO IRQ
        emulator.Fetch(pc); // dummy read
    }
}

void CPU6502::Ins_BCS() {
    if(cycleCounter == 1){
        // cycle 2        
        dataBus = emulator.Fetch(pc);
        pc++;
        if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
            cycleCounterMax += 1; // Carry flag is set
        }
    }else if(cycleCounter == 2){
        // cycle 3
        unsigned short pcBranch;
        
        if(dataBus >= 0x80){
            // is negative
            pcBranch = pc - (256-dataBus);
        }else{
            pcBranch = pc + dataBus;
        }
        
        if((pc & 0xFF00) != (pcBranch & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }

        pc = pcBranch;
        cycleCounter++;
    }else if(cycleCounter == 3){
        //cycle 4
        // DO IRQ
        emulator.Fetch(pc); // dummy read
    }
}

void CPU6502::Ins_BEQ() {
    if (cycleCounter == 1){
        // cycle 2        
        dataBus = emulator.Fetch(pc);
        pc++;
        if((statusFlag & CPUFLAGS::ZERO) == CPUFLAGS::ZERO){
            cycleCounterMax += 1; // Zero flag is set
        }
    } else if(cycleCounter == 2){
        // cycle 3
        unsigned short pcBranch;
        
        if(dataBus >= 0x80){
            // is negative
            pcBranch = pc - (256-dataBus);
        }else{
            pcBranch = pc + dataBus;
        }
        
        if((pc & 0xFF00) != (pcBranch & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }

        pc = pcBranch;
        cycleCounter++;
    }else if(cycleCounter == 3){
        //cycle 4
        // DO IRQ
        emulator.Fetch(pc); // dummy read
    }
}

//CYCLE ACCURACY:
//24
void CPU6502::Ins_BIT() {
  if(opcode == BIT_ZEROPAGE_24){
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = accumulator & emulator.Fetch(dataAddress);

        isZero(dataBus);

        if ((dataBus & 0x40) != 0) {
            SetFlag(CPUFLAGS::OVERFLOW);
        } else {
            ClearFlag(CPUFLAGS::OVERFLOW);
        }
        isNegative(dataBus);
    }
  }else if(opcode == BIT_ABSOLUTE_2C){
    dataAddress = FetchAbsolute(pc);
    pc++;
    pc++;
  }

  if(opcode != BIT_ZEROPAGE_24){
    printf("BIT NOT WORK: 0x%X\n",opcode);
    while (true)
    {
        /* code */
    }
    
  }

//   if(cycleCounter == cycleCounterMax){
//     dataBus = accumulator & emulator.Fetch(dataAddress);

//     isZero(dataBus);

//     if ((dataBus & 0x40) != 0) {
//         SetFlag(CPUFLAGS::OVERFLOW);
//     } else {
//         ClearFlag(CPUFLAGS::OVERFLOW);
//     }
//     isNegative(dataBus);
//   }
}

void CPU6502::Ins_BMI(){
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
        if((statusFlag & CPUFLAGS::NEGATIVE) == NEGATIVE){
            cycleCounterMax++; // NEGATIVE flag is set
        }
    }else if(cycleCounter == 2){
        // cycle 3
        unsigned short pcBranch;
        
        if(dataBus >= 0x80){
            // is negative
            pcBranch = pc - (256-dataBus);
        }else{
            pcBranch = pc + dataBus;
        }
        
        if((pc & 0xFF00) != (pcBranch & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }

        pc = pcBranch;
        cycleCounter++;
    }else if(cycleCounter == 3){
        //cycle 4
        // DO IRQ
        emulator.Fetch(pc); // dummy read
    }
}

void CPU6502::Ins_BNE() {
    if(cycleCounter == 1){  
        dataBus = emulator.Fetch(pc);
        pc++;
        if((statusFlag & CPUFLAGS::ZERO) == 0){
            cycleCounterMax++; // Zero flag is clear
        }
    }else if(cycleCounter == 2){
        unsigned short pcBranch;
        if(dataBus >= 0x80){
            pcBranch = pc - (256-dataBus);
        }else{
            pcBranch = pc + dataBus;
        }
        
        if((pc & 0xFF00) != (pcBranch & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }

        pc = pcBranch;
        cycleCounter++;
    }else if (cycleCounter == 3){
        //cycle 4
        // DO IRQ
        emulator.Fetch(pc); // dummy read
    }
}

void CPU6502::Ins_BPL() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
        // If the negative flag is clear GO TO ADDRESS
        if((statusFlag & CPUFLAGS::NEGATIVE) == 0){
            cycleCounterMax++;
        }
    }else if(cycleCounter == 2){
        unsigned short pcBranch;
        if(dataBus >= 0x80){
            // is negative
            pcBranch = pc - (256-dataBus);
        }else{
            pcBranch = pc + dataBus;
        }
        
        if((pc & 0xFF00) != (pcBranch & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }

        pc = pcBranch;
        cycleCounter++;
    }else if(cycleCounter == 3){
        // DO IRQ
        emulator.Fetch(pc); // dummy read
    }
}

void CPU6502::Ins_BRK() {
    // TODO: 7 cycles
    
    // hack
    if(nmi == true){
        //NMI
        StoreInCPUMemory((unsigned char)(pc>>8),(unsigned short)(0x100 + sp));
        sp--;
        StoreInCPUMemory((unsigned char)(pc),(unsigned short)(0x100 + sp));
        sp--;
        //TODO: set flags
        ClearFlag(CPUFLAGS::BREAK); // clear only in NMI
        StoreInCPUMemory(statusFlag,(unsigned short)(0x100 + sp));
        sp--;
        pc = (unsigned short)((pc & 0xFF00) | (emulator.Fetch(0xFFFA)));
        pc = (unsigned short)((pc & 0xFF) | (emulator.Fetch(0xFFFB) << 8));
        SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
        nmi = false;
    }else if(nmi == false){
        StoreInCPUMemory((unsigned char)(pc>>8),(unsigned short)(0x100 + sp));
        sp--;
        StoreInCPUMemory((unsigned char)(pc),(unsigned short)(0x100 + sp));
        sp--;
        SetFlag(CPUFLAGS::BREAK);
        StoreInCPUMemory(statusFlag,(unsigned short)(0x100 + sp));
        sp--;
        pc = (unsigned short)((pc & 0xFF00) | (emulator.Fetch(0xFFFE)));
        pc = (unsigned short)((pc & 0xFF) | (emulator.Fetch(0xFFFF) << 8));
        SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
    }
    cycleCounter = cycleCounterMax;
}

void CPU6502::Ins_BVC() {
    if((statusFlag & CPUFLAGS::OVERFLOW) == 0){
        unsigned char goToAddress = emulator.Fetch(pc);
        pc++;
        if(goToAddress >= 0x80){
            // is negative
            pc -= (256-goToAddress);
        }else{
            pc += goToAddress;
        }
        cycleCounterMax++;
    }else{
        pc++;
        cycleCounterMax += 2;
    }
    
    printf("NOT WORK OPCODE: 0x%X\n",opcode);
    while (true)
    {
        /* code */
    }
}

void CPU6502::Ins_BVS() {
    if((statusFlag & CPUFLAGS::OVERFLOW) == CPUFLAGS::OVERFLOW){
        unsigned char goToAddress = emulator.Fetch(pc);
        pc++;
        if(goToAddress >= 0x80){
            // is negative
            pc -= (256-goToAddress);
        }else{
            pc += goToAddress;
        }
        cycleCounterMax++;
    }else{
        pc++;
        cycleCounterMax += 2;
    }

    printf("NOT WORK OPCODE: 0x%X\n",opcode);
    while (true)
    {
        /* code */
    }
    
}

void CPU6502::Ins_CLC(){
    // cycle 2
    ClearFlag(CPUFLAGS::CARRY);
    cycleCounter++;
}
void CPU6502::Ins_CLD() {
    // cycle 2
    // is always disabled
    ClearFlag(CPUFLAGS::DECIMAL_MODE);
    FetchImmediate(pc); // dummy read
}
void CPU6502::Ins_CLI(){
    // cycle 2
    ClearFlag(CPUFLAGS::INTERRUPT_DISABLE);
    cycleCounter++;
}

void CPU6502::Ins_CLV() {
    // cycle 2
    ClearFlag(CPUFLAGS::OVERFLOW);
    cycleCounter++;
}

//CYCLE ACCURACY:
// C9
void CPU6502::Ins_CMP() {
    if(opcode == CMP_IMMEDIATE_C9){
        // max cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
        if(accumulator >= dataBus){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        if(accumulator == dataBus){
            SetFlag(CPUFLAGS::ZERO);
        }else{
            ClearFlag(CPUFLAGS::ZERO);
        }
        isNegative((unsigned char)(accumulator-dataBus));
    }else if(opcode == CMP_ZEROPAGE_C5){
        // max cycle 3
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
            if(accumulator >= dataBus){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            if(accumulator == dataBus){
                SetFlag(CPUFLAGS::ZERO);
            }else{
                ClearFlag(CPUFLAGS::ZERO);
            }
            isNegative((unsigned char)(accumulator-dataBus));
        }
    }else if(opcode == CMP_ZEROPAGEX_D5){
        // max cycle 4
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            emulator.Fetch(dataAddress); // Dummy Read
            dataAddress += registerX; // 00bb + X
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
            if(accumulator >= dataBus){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            if(accumulator == dataBus){
                SetFlag(CPUFLAGS::ZERO);
            }else{
                ClearFlag(CPUFLAGS::ZERO);
            }
            isNegative((unsigned char)(accumulator-dataBus));
        }
    }else if(opcode == CMP_ABSOLUTE_CD){
        // max cycle 4
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) (emulator.Fetch(pc) << 8 | dataAddress);
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
            if(accumulator >= dataBus){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            if(accumulator == dataBus){
                SetFlag(CPUFLAGS::ZERO);
            }else{
                ClearFlag(CPUFLAGS::ZERO);
            }
            isNegative((unsigned char)(accumulator-dataBus));
        }
    }else if(opcode == CMP_ABSOLUTEX_DD){
        // max cycle 4 or 5
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);

            if(accumulator >= dataBus){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            if(accumulator == dataBus){
                SetFlag(CPUFLAGS::ZERO);
            }else{
                ClearFlag(CPUFLAGS::ZERO);
            }
            isNegative((unsigned char)(accumulator-dataBus));
            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }
        }else if(cycleCounter == 4){
            cycleCounter++;
        }
    }else if(opcode == CMP_ABSOLUTEY_D9){
        // max cycle 4 or 5
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerY;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);

            if(accumulator >= dataBus){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            if(accumulator == dataBus){
                SetFlag(CPUFLAGS::ZERO);
            }else{
                ClearFlag(CPUFLAGS::ZERO);
            }
            isNegative((unsigned char)(accumulator-dataBus));
            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }
        }else if(cycleCounter == 4){
            cycleCounter++;
        }
    }else if(opcode == CMP_INDIRECTX_C1){
        // max cycle 6
        dataAddress = FetchIndirectX(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == CMP_INDIRECTY_D1){
        // max cycle 5 or 6
        dataAddress = FetchIndirectY(pc);
        CheckPageCrossing(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }

    if(opcode != CMP_IMMEDIATE_C9 && opcode != CMP_ZEROPAGEX_D5 && opcode != CMP_ABSOLUTEY_D9 && opcode != CMP_ABSOLUTEX_DD && opcode != CMP_ZEROPAGE_C5 && opcode != CMP_ABSOLUTE_CD){
        printf("CMP NOT WORK opcode: 0x%X, PC: 0x%X\n",opcode,pc);
        while (true)
        {
            /* code */
        }
        
    }
}

void CPU6502::Ins_CPX() {
    if(opcode == CPX_IMMEDIATE_E0){
        // max cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == CPX_ZEROPAGE_E4){
        // max cycle 3
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
        }
    }else if(opcode == CPX_ABSOLUTE_EC){
        // max cycle 4
        dataAddress = FetchAbsolute(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }

    if(cycleCounter == cycleCounterMax){

        if(registerX >= dataBus){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        if(registerX == dataBus){
            SetFlag(CPUFLAGS::ZERO);
        }else{
            ClearFlag(CPUFLAGS::ZERO);
        }
        isNegative((unsigned char) (registerX - dataBus));
    }
    if(opcode != CPX_IMMEDIATE_E0 && opcode != CPX_ZEROPAGE_E4){
        printf("CPX NOT WORK, OPCODE: 0x%X, PC: 0x%X\n",opcode, pc);
        while (true)
        {
            /* code */
        }
        
    }
}

// Cycle Accuracy
// C0
void CPU6502::Ins_CPY() {
    if(opcode == CPY_IMMEDIATE_C0){
        // max cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == CPY_ZEROPAGE_C4){
        // max cycle 3
        dataAddress = FetchZeroPage(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == CPY_ABSOLUTE_CC){
        // max cycle 4
        dataAddress = FetchAbsolute(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }

    if(cycleCounter == cycleCounterMax){
        if(registerY >= dataBus){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        if(registerY == dataBus){
            SetFlag(CPUFLAGS::ZERO);
        }else{
            ClearFlag(CPUFLAGS::ZERO);
        }
        isNegative((unsigned char)(registerY - dataBus));
    }
    if(opcode != CPY_IMMEDIATE_C0){
        printf("CPY NOT WORK, OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}

//CYCLE ACCURACY
//C6, D6, CE
void CPU6502::Ins_DEC() {
    if(opcode == DEC_ZEROPAGE_C6){
        // max cycle 5
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); //FetchZeroPage(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 3){
            StoreInCPUMemory(dataBus, dataAddress); // dummy write
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
        // printf("databus dec: 0x%X\n",dataBus);
    }else if(opcode == DEC_ZEROPAGEX_D6){
        // max cycle 6
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress); // dummy read
            dataAddress += registerX;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus,dataAddress); // dummy store
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 5){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
    }else if(opcode == DEC_ABSOLUTE_CE){
        // max cycle 6
        if(cycleCounter == 1){
            // cycle 2
            dataBus = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) (dataBus | (emulator.Fetch(pc) << 8));
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus,dataAddress); // dummy store
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 5){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
    }else if(opcode == DEC_ABSOLUTEX_DE){
        // max cycle 7
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                // Here Fix high byte
            }
        }else if(cycleCounter == 4){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 5){
            StoreInCPUMemory(dataBus,dataAddress); // dummy store
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 6){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
    }

    if(opcode != DEC_ZEROPAGE_C6 && opcode != DEC_ZEROPAGEX_D6 && opcode != DEC_ABSOLUTEX_DE && opcode != DEC_ABSOLUTE_CE){
        printf("DEC not work: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
    }
}

void CPU6502::Ins_DEX() {
    // max 2 cycles
    registerX--;
    isZero(registerX);
    isNegative(registerX);
    cycleCounter++;
}

void CPU6502::Ins_DEY() {
    // max 2 cycles
    registerY--;
    isZero(registerY);
    isNegative(registerY);
    cycleCounter++;
}

// Cycle Accurate
// 45, 49, 4D
void CPU6502::Ins_EOR() {
    if(opcode == EOR_IMMEDIATE_49){
        // max cycles 2
        dataBus = emulator.Fetch(pc);
        pc++;
        accumulator ^= dataBus;
        isZero(accumulator);
        isNegative(accumulator);
    }else if(opcode == EOR_ZEROPAGE_45){
        // max cycles 3
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
            accumulator ^= dataBus;
            isZero(accumulator);
            isNegative(accumulator);
        }
    }else if(opcode == EOR_ZEROPAGEX_55){
        // max cycles 4
        dataAddress = FetchZeroPageX(pc);
        pc++;
    }else if(opcode == EOR_ABSOLUTE_4D){
        // max cycles 4
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc)<<8) | dataAddress);
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
            accumulator ^= dataBus;
            isZero(accumulator);
            isNegative(accumulator);
        }
    }else if(opcode == EOR_ABSOLUTEX_5D){
        // max cycles 4 or 5
        dataAddress = FetchAbsoluteX(pc);
        CheckPageCrossing(pc);
        pc++;
        CheckPageCrossing(pc);
        pc++;
    }else if(opcode == EOR_ABSOLUTEY_59){
        // max cycles 4 or 5
        dataAddress = FetchAbsoluteY(pc);
        CheckPageCrossing(pc);
        pc++;
        CheckPageCrossing(pc);
        pc++;
    }else if(opcode == EOR_INDIRECTX_41){
        // max cycles 6
        dataAddress = FetchIndirectX(pc);
        pc++;
    }else if(opcode == EOR_INDIRECTY_51){
        // max cycles 5 or 6
        dataAddress = FetchIndirectY(pc);
        CheckPageCrossing(pc);
        pc++;
    }

    if(opcode != EOR_ZEROPAGE_45 && opcode != EOR_ABSOLUTE_4D && opcode != EOR_IMMEDIATE_49){
        printf("EOR not work: 0x%x, PC: 0x%X\n",opcode,pc);
        while (true)
        {
            /* code */
        }
        
    }
}

void CPU6502::Ins_INC() {
    if(opcode == INC_ZEROPAGE_E6){
        // max cycles 5
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 3){
            StoreInCPUMemory(dataBus, dataAddress); // dummy write
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
    } else if(opcode == INC_ZEROPAGEX_F6){
        // max cycles 6
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress); // dummy read
            dataAddress += registerX;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus, dataAddress);
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 5){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
    } else if(opcode == INC_ABSOLUTE_EE){
        // max cycles 6
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc)<<8) | dataAddress);
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus, dataAddress); // dummy write
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 5){
            StoreInCPUMemory(dataBus, dataAddress);
            cycleCounter++;
        }
    } else if(opcode == INC_ABSOLUTEX_FE){
        // max cycles 7
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
            
            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                dataAddress += 0x100; // page crossing, Fix high byte
            }
        }else if(cycleCounter == 4){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 5){
            StoreInCPUMemory(dataBus,dataAddress);  // dummy write
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }else if(cycleCounter == 6){
            StoreInCPUMemory(dataBus,dataAddress);
            cycleCounter++;
        }
    }

    if(opcode != INC_ZEROPAGE_E6 && opcode != INC_ABSOLUTE_EE && opcode != INC_ZEROPAGEX_F6 && opcode != INC_ABSOLUTEX_FE){
        printf("INC NOT WORK, OPCODE: 0x%X, PC:0x%X\n",opcode,pc);
        while (true)
        {
            /* code */
        }       
    }
}

void CPU6502::Ins_INX() {
    // max 2 cycles
    registerX++;
    isZero(registerX);
    isNegative(registerX); 
    cycleCounter++;
}

void CPU6502::Ins_INY() {
    // max 2 cycles
    registerY++;
    isZero(registerY);
    isNegative(registerY); 
    cycleCounter++;
}

//CYCLE ACCURATE:
// 4C
void CPU6502::Ins_JMP() {
    if(opcode == JMP_ABSOLUTE_4C){
        // max 3 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            pc++;
            pc = dataAddress;
        }
    }else if(opcode == JMP_INDIRECT_6C){
        // max 5 cycles
        dataAddress = FetchIndirect(pc);
        pc++;
        pc++;
    }

    if(opcode != JMP_ABSOLUTE_4C){
        printf("JMP NOT WORK: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}

int  debugsp = 0;
// CYCLE ACCURACY
void CPU6502::Ins_JSR()
{
    debugsp++;
    // max 6 cycles
    if(cycleCounter == 1){
        // fetch low address byte
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (unsigned short) (0x100 | sp);
        sp = dataBus; // stack pointer save PC Low byte
        cycleCounter++;
    }else if(cycleCounter == 3){
        StoreInCPUMemory((unsigned char)((pc & 0xFF00) >> 8), dataAddress);
        dataAddress = (unsigned short)((unsigned char)(dataAddress - 1) | 0x100);
        cycleCounter++;
    }else if(cycleCounter == 4){
        StoreInCPUMemory((unsigned char)(pc & 0x00FF), dataAddress);
        dataAddress = (unsigned short)((unsigned char)(dataAddress - 1) | 0x100);
        cycleCounter++;
    }else if(cycleCounter == 5){
        pc = (unsigned short)((emulator.Fetch(pc) << 8) | sp);
        sp = (unsigned char) dataAddress;
    }
}

//CYCLE ACCURACY:
//A5, A9, AD, B1, BD, B5
void CPU6502::Ins_LDA(){
    if(opcode == LDA_IMMEDIATE_A9){
        // cycle 2
        accumulator = emulator.Fetch(pc);
        pc++;
    }else if(opcode == LDA_ZEROPAGE_A5){
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            //cycle 3
            accumulator = emulator.Fetch(dataAddress);
        }
    }else if(opcode == LDA_ZEROPAGEX_B5){
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            //cycle 3
            emulator.Fetch(dataAddress); // DUMMY READ
            dataAddress += registerX;
        }else if(cycleCounter == 3){
            accumulator = emulator.Fetch(dataAddress);
        }
    }else if(opcode == LDA_ABSOLUTE_AD){
        //cycle 2        
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) (dataAddress | (emulator.Fetch(pc) << 8));
            pc++;
        }else if(cycleCounter == 3){
            accumulator = emulator.Fetch(dataAddress);
        }
    }else if(opcode == LDA_ABSOLUTEX_BD){
        switch (cycleCounter){
        case 1:
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
            break;
        case 2:
            //cycle 3
            dataAddress = (unsigned short) ((emulator.Fetch(pc)<<8) | dataAddress);
            dataAddress += registerX;
            pc++;
            break;
        case 3:
            //cycle 4
            accumulator = emulator.Fetch(dataAddress);
            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }
            break;
        case 4:
            //cycle 5 only if page is crossed
            cycleCounter++;
            break;
        default:
            printf("LDA_ABSOLUTEX_BD ERROR\n");
            while (true){}
            break;
        }
    }else if(opcode == LDA_ABSOLUTEY_B9){
        switch (cycleCounter)
        {
        case 1:
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
            break;
        case 2:
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerY;
            pc++;
            break;
        case 3:
            //cycle 4
            accumulator = emulator.Fetch(dataAddress);
            
            if(((dataAddress - registerY) & 0xFF00) !=(dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }
            break;
        case 4:
            //cycle 5 only if page is crossed
            cycleCounter++;
            break;
        default:
            printf("LDA_ABSOLUTEY_B9 ERROR\n");
            while (true){}
            break;
        }
    }else if(opcode == LDA_INDIRECTX_A1){
        switch (cycleCounter)
        {
        case 1:
            dataBus = emulator.Fetch(pc); // 00bb
            pc++;
            break;
        case 2:
            emulator.Fetch(dataBus); // dummy read
            dataBus += registerX; // 00bb + X
            break;
        case 3:
            //cycle 4
            dataAddress = emulator.Fetch(dataBus); // 00bb + X = xx = low byte
            dataBus++; // 00bb + X + 1 = yy = high byte
            break;
        case 4:
            //cycle 5
            dataAddress = (unsigned short) ((emulator.Fetch(dataBus)<<8) | dataAddress); // yyxx
            break;
        case 5:
            //cycle 6
            accumulator = emulator.Fetch(dataAddress);
            break;
        default:
            printf("LDA_INDIRECTX_A1 ERROR\n");
            while (true){}
            break;
        }
    }else if(opcode == LDA_INDIRECTY_B1){
        switch (cycleCounter){
        case 1:
            //cycle 2
            dataBus = emulator.Fetch(pc); // bb
            pc++;
            break;
        case 2:
            //cycle 3
            dataAddress = emulator.Fetch(dataBus); // 00bb = xx = low byte
            dataBus++; // 00bb+1 = yy = high byte
            break;
        case 3:
            //cycle 4
            dataAddress = (unsigned short) ((emulator.Fetch(dataBus)<<8) | dataAddress); // yyxx
            dataAddress += registerY; // yyxx + Register Y
            break;
        case 4:
            //cycle 5
            accumulator = emulator.Fetch(dataAddress);
            if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }
            break;
        case 5:
            //cycle 6
            cycleCounter++;
            break;
        default:
            printf("LDA_INDIRECTX_B1 ERROR\n");
            while (true){}
            break;
        }
    }
    if(opcode != LDA_ABSOLUTEY_B9 && opcode != LDA_ZEROPAGEX_B5 && opcode != LDA_ABSOLUTEX_BD &&opcode != LDA_ABSOLUTE_AD && opcode != LDA_ZEROPAGE_A5 && opcode != LDA_IMMEDIATE_A9 && opcode != LDA_INDIRECTY_B1){
        printf("LDA NOT WORK: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }

    if(cycleCounter == cycleCounterMax){
        isZero(accumulator);
        isNegative(accumulator);
    }
}

// CYCLE ACCURACY:
// A2, A6, AE
void CPU6502::Ins_LDX(){
    //TODO: needs to be based on cycles
    if(opcode == LDX_IMMEDIATE_A2){
        //cycle 2
        registerX = emulator.Fetch(pc);
        pc++;
        isZero(registerX);
        isNegative(registerX);
    }else if(opcode == LDX_ZEROPAGE_A6){
        // max cycle 3
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            registerX = emulator.Fetch(dataAddress);
            isZero(registerX);
            isNegative(registerX);
        }
    }else if (opcode == LDX_ZEROPAGEY_B6){
        dataAddress = FetchZeroPageY(pc);
        pc++;
        registerX = emulator.Fetch(dataAddress);
    }else if(opcode == LDX_ABSOLUTE_AE){
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            pc++;
        }else if(cycleCounter == 3){    
            registerX = emulator.Fetch(dataAddress);
            isZero(registerX);
            isNegative(registerX);
        }
    }else if(opcode == LDX_ABSOLUTEY_BE){
        dataAddress = FetchAbsolute(pc);
        CheckPageCrossing(pc);
        pc++;
        CheckPageCrossing(pc);
        pc++;
        registerX = emulator.Fetch(dataAddress);
    }


    if(opcode != LDX_IMMEDIATE_A2 && opcode != LDX_ZEROPAGE_A6 && opcode != LDX_ABSOLUTE_AE){
        printf("LDX not work: 0x%x\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}

//CYCLE ACCURACY
// A0, B4, A4, AC
void CPU6502::Ins_LDY(){
    if(opcode == LDY_IMMEDIATE_A0){
        //cycle 2
        registerY = emulator.Fetch(pc);
        pc++;
        
        isZero(registerY);
        isNegative(registerY);
    }else if(opcode == LDY_ZEROPAGE_A4){
        // max cycle 3 
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            registerY = emulator.Fetch(dataAddress);
            isZero(registerY);
            isNegative(registerY);
        }
    }else if (opcode == LDY_ZEROPAGEX_B4){
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            // cycle 3
            emulator.Fetch(dataAddress); //dummy read
            dataAddress += registerX; // 00bb + X
        }else if(cycleCounter == 3){
            // cycle 4
            registerY = emulator.Fetch(dataAddress);
            isZero(registerY);
            isNegative(registerY);
        }
    }else if(opcode == LDY_ABSOLUTE_AC){
        if(cycleCounter == 1){ // cycle 2
            dataBus = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) (dataBus | (emulator.Fetch(pc)<<8));
            pc++;
        }else if(cycleCounter == 3){
            registerY = emulator.Fetch(dataAddress);
        }
    }else if(opcode == LDY_ABSOLUTEX_BC){
        // Max 4 or 5 cycles  
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            registerY = emulator.Fetch(dataAddress);
            
            isZero(registerY);
            isNegative(registerY);
            
            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++; // page crossing add 1 cycle
            }
        }else if(cycleCounter == 4){
            cycleCounter++;
        }
    }

    if(opcode != LDY_IMMEDIATE_A0 && opcode != LDY_ZEROPAGEX_B4 && opcode != LDY_ZEROPAGE_A4 && opcode != LDY_ABSOLUTE_AC && opcode != LDY_ABSOLUTEX_BC){
        printf("LDY not work: 0x%X, PC: 0x%X\n",opcode,pc);
        while (true)
        {
            /* code */
        }
        
    }
    
    // isZero(registerY);
    // isNegative(registerY);
}

//CYCLE ACCURACY:
//4A, 46
void CPU6502::Ins_LSR(){
    if(opcode == LSR_ACCUMULATOR_4A){
        //Max 2 cycles
        if((accumulator & CPUFLAGS::CARRY) == 0x1){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        accumulator = accumulator >> 1;
        isZero(accumulator);
        isNegative(accumulator);
        cycleCounter++;
    }else if(opcode == LSR_ABSOLUTE_4E){
        dataAddress = FetchAbsolute(pc);
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == LSR_ZEROPAGE_46){
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 3){
            StoreInCPUMemory(dataBus,dataAddress);
            cycleCounter++;
        }else if(cycleCounter == 4){
            if((dataBus & CPUFLAGS::CARRY) == 0x1){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }

            dataBus = dataBus >> 1;
            StoreInCPUMemory(dataBus,dataAddress);
            isZero(dataBus);
            isNegative(dataBus);
            cycleCounter++;
        }
    }else if(opcode == LSR_ZEROPAGEX_56){
        dataAddress = FetchZeroPageX(pc);
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }else if(opcode == LSR_ABSOLUTEX_5E){
        dataAddress = FetchAbsoluteX(pc);
        pc++;
        pc++;
        dataBus = emulator.Fetch(dataAddress);
    }

    if(opcode != LSR_ACCUMULATOR_4A && opcode != LSR_ZEROPAGE_46){
        printf("LSR NOT WORK: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}

void CPU6502::Ins_NOP() {
    cycleCounter++;
}

// CYCLE ACCURACY
// 5, 9
void CPU6502::Ins_ORA()
{
    if(opcode == ORA_INDIRECTX_01){
        // cycle 2
        if(cycleCounter == 1){
            dataAddress = FetchIndirectX(pc);
            pc++;
        }else if(cycleCounter == 4 || cycleCounter == 5){
            //cycle 5 and 6
            cycleCounter++;
        }
    }else if(opcode == ORA_ZEROPAGE_05){
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = emulator.Fetch(pc); //FetchZeroPage(pc);
            pc++;
        }else if(cycleCounter == 2){
            //cycle 3
            dataBus = emulator.Fetch(dataAddress);
            accumulator |= dataBus;
            isZero(accumulator);
            isNegative(accumulator);
        }
    }else if(opcode == ORA_IMMEDIATE_09){
        // cycle 2
        dataBus = emulator.Fetch(pc); //FetchImmediate(pc);
        pc++;
        accumulator |= dataBus;
        isZero(accumulator);
        isNegative(accumulator);
    }else if(opcode == ORA_ABSOLUTE_0D){
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = FetchAbsolute(pc);
            pc++;
            pc++;
        }else if(cycleCounter == 3){
            // cycle 4
            cycleCounter++;
        }
    }else if(opcode == ORA_INDIRECTY_11){
        // cycle 2
        if(cycleCounter == 1){
            dataAddress = FetchIndirectY(pc);
            CheckPageCrossing(pc);
            pc++;
        }else if(cycleCounter == 4 || cycleCounter == 5){
            //cycle 5 or cycle 6
            cycleCounter++;
        }
    }else if(opcode == ORA_ZEROPAGEX_15){
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = FetchZeroPageX(pc);
            pc++;
        }else if(cycleCounter == 2 || cycleCounter == 3){
            // cycle 3 and cycle 4
            cycleCounter++;
        }
    }else if(opcode == ORA_ABSOLUTEY_19){
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = FetchAbsoluteY(pc);
            CheckPageCrossing(pc);
            pc++;
            CheckPageCrossing(pc);
            pc++;
        }else if(cycleCounter == 3){
            // cycle 4 or cycle  5
            cycleCounter++;
        }
    }else if(opcode == ORA_ABSOLUTEX_1D){
        if(cycleCounter == 1){
            // cycle 2
            dataAddress = FetchAbsoluteX(pc);
            CheckPageCrossing(pc);
            pc++;
            CheckPageCrossing(pc);
            pc++;
        }else if(cycleCounter == 3){
            // cycle 4 or cycle  5
            cycleCounter++;
        }
    }

    if(opcode != ORA_ZEROPAGE_05 && opcode != ORA_IMMEDIATE_09){
        printf("ORA NOT WORK, OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
    }
    
}

void CPU6502::Ins_PHA() {
    // max 3 cycles
    if(cycleCounter == 1){
        emulator.Fetch(pc); // dummy read
    }else if(cycleCounter == 2){
        StoreInCPUMemory(accumulator,(unsigned short)(0x100 + sp));
        sp--;
        cycleCounter++;
    }
}

// Pushing in the stack the status
void CPU6502::Ins_PHP(){
    // max 3 cycles
    if(cycleCounter == 1){
        emulator.Fetch(pc); // dummy read
    }else if(cycleCounter == 2){
        StoreInCPUMemory(statusFlag,(unsigned short)(0x100 + sp));
        sp--;
        cycleCounter++;
    }
}

void CPU6502::Ins_PLA() {
    // max 4 cycles
    if(cycleCounter == 1){
        emulator.Fetch(pc); // dummy read
    }else if(cycleCounter == 2){
        sp++;
        cycleCounter++;
    }else if(cycleCounter == 3){
        dataAddress = (unsigned short)(0x100 | sp);
        accumulator = emulator.Fetch(dataAddress);
        isZero(accumulator);
        isNegative(accumulator);
    }

}

// Pull processor status from stack
void CPU6502::Ins_PLP(){
    // max 4 cycles
    emulator.Fetch(pc); // dummy read
    sp++;
    dataAddress = (unsigned short)(0x100 | sp);
    statusFlag = emulator.Fetch(dataAddress);

    cycleCounter = cycleCounterMax;
}

// CYCLE ACCURACY:
// 2A
void CPU6502::Ins_ROL()
{
    if(opcode == ROL_ACCUMULATOR_2A){
        char futureBit = 0;
        if (((accumulator) & 0x80) == 0x80){
            futureBit = 1;
        }
        accumulator = accumulator << 1;
        if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
            accumulator |= 1;
        }
        if(futureBit == 1){
            SetFlag(CARRY);
        }else{
            ClearFlag(CARRY);
        }
        isZero(accumulator);
        isNegative(accumulator);
        cycleCounter++;
    }else if(opcode == ROL_ZEROPAGE_26){
        // max 5 cycles
        dataAddress = FetchZeroPage(pc);
        pc++;
    }else if(opcode == ROL_ZEROPAGEX_36){
        // max 6 cycles
        dataAddress = FetchZeroPageX(pc);
        pc++;
    }else if(opcode == ROL_ABSOLUTE_2E){
        // max 6 cycles
        dataAddress = FetchAbsolute(pc);
        pc++;
        pc++;
    }else if(opcode == ROL_ABSOLUTEX_3E){
        // max 7 cycles
        dataAddress = FetchAbsoluteX(pc);
        pc++;
        pc++;
    }

    if(opcode != ROL_ACCUMULATOR_2A){
        printf("ROL NOT WORK OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
    }
    
}

// CYCLE ACCURACY:
// 6A, 66
void CPU6502::Ins_ROR() {
    if(opcode == ROR_ACCUMULATOR_6A){
        // max 2 cycles
        char carryFuture = 0; //value bit 0
        if (((accumulator) & 0x01) == 0x01){
            carryFuture = 1;
        }

        accumulator = accumulator >> 1;
        if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
            accumulator |= 0x80;
        }
        if(carryFuture == 1){
            SetFlag(CARRY);
        }else{
            ClearFlag(CARRY);
        }
        isZero(accumulator);
        isNegative(accumulator);
        cycleCounter++;
    }else if(opcode == ROR_ZEROPAGE_66){
        // max 5 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataBus = emulator.Fetch(dataAddress);
        }else if(cycleCounter == 3){
            StoreInCPUMemory(dataBus,dataAddress); // dummy write
            char carryFuture = 0; //value bit 0

            if ((dataBus & 0x01) == 0x01){
                carryFuture = 1;
            }

            dataBus = dataBus >> 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 0x80;
            }
            
            if (carryFuture == 1){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
            
            cycleCounter++;
        }else if(cycleCounter == 4){
            StoreInCPUMemory(dataBus,dataAddress);
            cycleCounter++;
        }
    }else if(opcode == ROR_ZEROPAGEX_76){
        // max 6 cycles
        dataAddress = FetchZeroPageX(pc);
        pc++;
    }else if(opcode == ROR_ABSOLUTE_6E){
        // max 6 cycles
        dataAddress = FetchAbsolute(pc);
        pc++;
        pc++;
    }else if(opcode == ROR_ABSOLUTEX_7E){
        // max 7 cycles
        dataAddress = FetchAbsoluteX(pc);
        pc++;
        pc++;
    }
    if(opcode != ROR_ACCUMULATOR_6A && opcode != ROR_ZEROPAGE_66){
        printf("ROR NOT WORK OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
    }
}

// Cycle Accurate
void CPU6502::Ins_RTI() {
    // max 6 cycles
    if(cycleCounter == 1){
        emulator.Fetch(pc); // dummy read
    }else if(cycleCounter == 2){
        dataAddress = (unsigned short)(0x100 | sp);
        dataAddress = (unsigned short)((unsigned char)(dataAddress + 1) | 0x100);
        cycleCounter++;
    }else if(cycleCounter == 3){
        statusFlag = emulator.Fetch(dataAddress);
        dataAddress = (unsigned short)((unsigned char)(dataAddress + 1) | 0x100);
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress);
        pc = (unsigned short)((pc & 0xFF00) | dataBus); // Store Low byte
    }else if(cycleCounter == 5){
        dataAddress = (unsigned short)((unsigned char)(dataAddress + 1) | 0x100);
        dataBus = emulator.Fetch(dataAddress);
        pc = (unsigned short)((pc & 0xFF) | (dataBus << 8)); // Store High byte
        sp = (unsigned char) dataAddress;
    }
}

void CPU6502::Ins_RTS() {
    // max 6 cycles

    debugsp--;
    if(cycleCounter == 1){
        emulator.Fetch(pc); // dummy read
    }else if(cycleCounter == 2){
        dataAddress = (unsigned short)(0x100 | sp); // Get the stack pointer address to look
        emulator.Fetch(dataAddress); // dummy read?
        dataAddress++;
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress); // Get PC Low byte
        pc = dataBus;
        dataAddress++;
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress); // Get PC High byte
        pc = (unsigned short) (dataBus << 8) | pc;
    }else if(cycleCounter == 5){
        sp = (unsigned char)dataAddress;
        pc++;
        cycleCounter++;
    }
}

void CPU6502::Ins_SBC() {
    if(opcode == SBC_IMMEDIATE_E9){
        // max 2 cycles
        dataBus = emulator.Fetch(pc);
        pc++;
        unsigned char result = accumulator;
        result -= dataBus;
        if((statusFlag & CPUFLAGS::CARRY) != CPUFLAGS::CARRY){
            result -= 1;
        }
        if(((result ^ accumulator) & (result ^ (~dataBus)) & 0x80) != 0){
            SetFlag(CPUFLAGS::OVERFLOW);
        }else{
            ClearFlag(CPUFLAGS::OVERFLOW);
        }
        if(result >= 0){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        isZero(result);
        isNegative(result);
        accumulator = result;
    }else if(opcode == SBC_ZEROPAGE_E5){
        // max 3 cycles
        if(cycleCounter == 1){         
            dataAddress = emulator.Fetch(pc);
            
            pc++;
        }else if(cycleCounter == 2){
            unsigned char result = accumulator;
            dataBus = emulator.Fetch(dataAddress);
            result -= dataBus;
            if((statusFlag & CPUFLAGS::CARRY) != CPUFLAGS::CARRY){
                result -= 1;
            }
            if(((result ^ accumulator) & (result ^ (~dataBus)) & 0x80) != 0){
                SetFlag(CPUFLAGS::OVERFLOW);
            }else{
                ClearFlag(CPUFLAGS::OVERFLOW);
            }
            if((result & 0x80) != 0x80){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            isZero(result);
            isNegative(result);
            accumulator = result;
        }
    }else if(opcode == SBC_ZEROPAGEX_F5){
        // max 4 cycles
        dataAddress = FetchZeroPageX(pc);
        pc++;
    }else if(opcode == SBC_ABSOLUTE_ED){
        // max 4 cycles
        // if(cycleCounter == 1){
        //     dataAddress = emulator.Fetch(pc);
        //     pc++;
        // }else if(cycleCounter == 2){
        //     dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
        //     pc++;
        // }else if(cycleCounter == 3){
            
        // }
    }else if(opcode == SBC_ABSOLUTEX_FD){
        // max 4 or 5 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            unsigned char result = accumulator;
            dataBus = emulator.Fetch(dataAddress);
            result -= dataBus;
            if((statusFlag & CPUFLAGS::CARRY) != CPUFLAGS::CARRY){
                result -= 1;
            }
            if(((result ^ accumulator) & (result ^ (~dataBus)) & 0x80) != 0){
                SetFlag(CPUFLAGS::OVERFLOW);
                // printf("overflow set\n");
            }else{
                ClearFlag(CPUFLAGS::OVERFLOW);
                // printf("overflow clear\n");
            }
            if((result & 0x80) != 0x80){
                SetFlag(CPUFLAGS::CARRY);
                // printf("carry set\n");
            }else{
                ClearFlag(CPUFLAGS::CARRY);
                // printf("carry clear\n");
            }
            isZero(result);
            isNegative(result);
            accumulator = result;

            if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
                cycleCounterMax++;
            }
        }else if(cycleCounter == 4){
            cycleCounter++;
        }
        
    }else if(opcode == SBC_ABSOLUTEY_F9){
        // max 4 or 5 cycles
        dataAddress = FetchAbsoluteY(pc);
        CheckPageCrossing(pc);
        pc++;
        pc++;
    }else if(opcode == SBC_INDIRECTX_E1){
        // max 6 cycles
        dataAddress = FetchIndirectX(pc);
        pc++;
    }else if(opcode == SBC_INDIRECTY_F1){
        // max 5 or 6 cycles
        dataAddress = FetchIndirectY(pc);
        CheckPageCrossing(pc);
        pc++;
    }

    if(opcode != SBC_ZEROPAGE_E5 && opcode != SBC_IMMEDIATE_E9 && opcode != SBC_ABSOLUTEX_FD){
        printf("SBC NOT WORK, OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}

void CPU6502::Ins_SEC() {
    // max 2 cycles
    SetFlag(CPUFLAGS::CARRY);
    cycleCounter++;
}

void CPU6502::Ins_SED() {
    // max 2 cycles
    // SetFlag(CPUFLAGS::DECIMAL_MODE); // NES doesn't have it
    cycleCounter++;
}

void CPU6502::Ins_SEI() {
    // max 2 cycles
    // this is the 2 cycle
    SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
    FetchImmediate(pc); // Dummy read
    // DO IRQ 
}

// CYCLE ACCURACY:
// 85, 8D, 91, 95, 9D
void CPU6502::Ins_STA(){
    if(opcode == STA_ZEROPAGE_85){
        // max 3 cycles
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            //cycle 3
            StoreInCPUMemory(accumulator,dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STA_ZEROPAGEX_95){
        // max 4 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00BB
            pc++;
        }else if(cycleCounter == 2){
            emulator.Fetch(dataAddress); // DUMMY READ
            dataAddress += registerX;
        }else if(cycleCounter == 3){
            // cycle 4
            StoreInCPUMemory(accumulator, dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STA_ABSOLUTE_8D){
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            //cycle 3
            dataAddress = (unsigned short) (dataAddress | (emulator.Fetch(pc) << 8));
            pc++;
        }else if(cycleCounter == 3){
            //cycle 4
            StoreInCPUMemory(accumulator,dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STA_ABSOLUTEX_9D){
        // max 5 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc)<<8) | dataAddress);
            dataAddress += registerX;
            pc++;
        }else if(cycleCounter == 3){
            // add 1 cycle by default because
            // the processor cannot undo a write to an invalid
            // address, it always reads from the address first.
            dataBus = emulator.Fetch(dataAddress);
            if(((dataAddress-registerX) & 0xFF00) != (dataAddress & 0xFF00) ){
                // dataAddress += 0x100;
            }
        }else if(cycleCounter == 4){
            StoreInCPUMemory(accumulator, dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STA_ABSOLUTEY_99){
        // max 5 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc)<<8) | dataAddress);
            dataAddress += registerY;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress);
            
            // add 1 cycle by default because
            // the processor cannot undo a write to an invalid
            // address, it always reads from the address first.
            if(((dataAddress-registerX) & 0xFF00) != (dataAddress & 0xFF00) ){
                // dataAddress += 0x100;
            }
        }else if(cycleCounter == 4){
            StoreInCPUMemory(accumulator, dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STA_INDIRECTX_81){
        // max 6 cycles
        dataAddress = FetchIndirectX(pc);
        pc++;
    }else if(opcode == STA_INDIRECTY_91){
        if(cycleCounter == 1){
            // cycle 2
            dataBus = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            // cycle 3
            dataAddress = emulator.Fetch(dataBus); // xx
            dataBus++; // oobb + 1
        }else if(cycleCounter == 3){
            // cycle 4
            dataAddress = (dataAddress | (emulator.Fetch(dataBus) << 8)); // yyxx
            dataAddress += registerY; // yyxx + Y
        }else if(cycleCounter == 4){
            // cycle 5
            emulator.Fetch(dataAddress);
            // add 1 cycle by default because
            // the processor cannot undo a write to an invalid
            // address, it always reads from the address first.
            if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00) ){
                // dataAddress += 0x100;
            }
        }else if(cycleCounter == 5){
            // cycle 6
            StoreInCPUMemory(accumulator,dataAddress);
            cycleCounter++;
        }
    }

    if(opcode != STA_ABSOLUTEX_9D && opcode != STA_ZEROPAGE_85 && opcode != STA_ABSOLUTE_8D && opcode != STA_INDIRECTY_91 && opcode != STA_ZEROPAGEX_95 && opcode != STA_ABSOLUTEY_99){
        printf("sta doesn't work: 0x%x\n",opcode);
        while (true)
        {
            /* code */
        }

    }
}

//CYCLE ACCURACY:
// 86, 8E
void CPU6502::Ins_STX()
{
    if(opcode == STX_ZEROPAGE_86){
        // max 3 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            cycleCounter++;
        }
    }else if(opcode == STX_ZEROPAGEY_96){
        // max 4 cycles
        dataAddress = FetchZeroPageX(pc);
        pc++;
    }else if(opcode == STX_ABSOLUTE_8E){
        //cycle 2
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // FetchAbsolute(pc);
            pc++;
        }else if(cycleCounter == 2){
            //cycle 3
            dataAddress = (unsigned short) ((emulator.Fetch(pc)<<8) | dataAddress); // FetchAbsolute(pc);
            pc++;
        }else if(cycleCounter == 3){
            //cycle 4
            cycleCounter++;
        }
    }

    if(cycleCounter == cycleCounterMax){
        StoreInCPUMemory(registerX,dataAddress);
    }

    if(opcode != STX_ZEROPAGE_86 && opcode != STX_ABSOLUTE_8E){
        printf("STX NOT WORK, OPCODE: 0x%X\n",opcode);
        while (true)
        {
            /* code */
        }
        
    }
}
//CYCLE ACCURACY
// 84, 8C
void CPU6502::Ins_STY()
{
    if(opcode == STY_ZEROPAGE_84){
        //max 3 cycles
        if(cycleCounter == 1){
            //cycle 2
            dataAddress = emulator.Fetch(pc); //FetchZeroPage(pc);
            pc++;
        }else if(cycleCounter == 2){
            // cycle 3
            StoreInCPUMemory(registerY,dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STY_ZEROPAGEX_94){
        //max 4 cycles
        if(cycleCounter == 1){
            dataAddress = emulator.Fetch(pc); // 00bb
            pc++;
        }else if(cycleCounter == 2){
            emulator.Fetch(dataAddress); // dummy read
            dataAddress += registerX;
        } else if(cycleCounter == 3){
            StoreInCPUMemory(registerY,dataAddress);
            cycleCounter++;
        }
    }else if(opcode == STY_ABSOLUTE_8C){
        //max 4 cycles
        if(cycleCounter == 1){
            dataBus = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) (dataBus | (emulator.Fetch(pc)<<8));
            pc++;
        } else if(cycleCounter == 3){
            StoreInCPUMemory(registerY,dataAddress);
            cycleCounter++;
        }
    }

    if(opcode != STY_ZEROPAGE_84 && opcode != STY_ABSOLUTE_8C && opcode != STY_ZEROPAGEX_94){
        printf("sty not work, OPCODE: 0x%X, PC: 0x%X\n",opcode,pc);
        while (true)
        {
            /* code */
        }
        
    }
}

void CPU6502::Ins_TAX() {
    // max 2 cycles
    registerX = accumulator;
    isZero(registerX);
    isNegative(registerX);
    cycleCounter++;
}

void CPU6502::Ins_TAY() {
    // max 2 cycles
    registerY = accumulator;
    isZero(registerY);
    isNegative(registerY);
    cycleCounter++;
}

void CPU6502::Ins_TSX() {
    // max 2 cycles
    registerX = sp;
    isZero(registerX);
    isNegative(registerX);
    cycleCounter++;
}

void CPU6502::Ins_TXA(){
    //cycle 2
    accumulator = registerX;
    isZero(accumulator);
    isNegative(accumulator);
    
    emulator.Fetch(pc); // dummy read;
}

void CPU6502::Ins_TXS(){
    //cycle 2
    sp = registerX;
    emulator.Fetch(pc); // dummy read;
}

void CPU6502::Ins_TYA() {
    //cycle 2
    accumulator = registerY;
    isZero(accumulator);
    isNegative(accumulator);
    emulator.Fetch(pc); // dummy read;
}

void CPU6502::EmulateCycle()
{
    if(DMAactive == true){
        // 0x4014 is active for load sprites in OAM
        if(cycleCounter == 0){
            oamBus = emulator.Fetch((unsigned short) (DMAPage << 8 | DMAAddress));
            cycleCounterMax = 2;
        }else{
            StoreInCPUMemory(oamBus, 0x2004);
            DMAAddress++;
            if (DMAAddress == 0){
                DMAactive = false;
            }
            cycleCounter++;
        }
    }else if(nmi == false && cycleCounter == 0){
        // Fetch opcode
        // cycle 1, always is the start
        // #define debug_emu
        #if defined(debug_emu)
    
        printf("leyendo opcode\n");
        printf("PC: 0x%X\n",pc);

        #endif
        pcdebug = pc;

        if(((emulator.ppu.ppuctrl & PPU_FLAGS::NMI_FLAG) == PPU_FLAGS::NMI_FLAG) && 
        ((emulator.ppu.ppustatus & PPU_FLAGS::PPU_VBLANK) == PPU_FLAGS::PPU_VBLANK) && beforeNMI == false){
            nmi = true;
            beforeNMI = nmi;
            opcode = BRK_00;
            cycleCounter++;
            // printf("interruput opcode: 0x%X\n",opcode);
        }else{
            if(cycleCounterMax != cycleCounter && cycleCounter != 0){
                printf("no es cycle perfer\n");
                while (true){}
            }

            opcode = FetchImmediate(pc);
            pc++;
        }

        cycleCounterMax = cycles[opcode];
        
        totalCycles += cycleCounterMax;
    }else{
        // Get the first 4 bits
        switch (opcode & 0xF0)
        {
        case 0x00:
            switch (opcode)
            {
            case BRK_00: // 0x00: Break
                Ins_BRK();
                break;
            case ORA_INDIRECTX_01: // 0x01
                Ins_ORA();
                break;
            case ORA_ZEROPAGE_05: // 0x05
                Ins_ORA();
                break;
            case ASL_ZEROPAGE_06:
                Ins_ASL();
                break;
            case PHP_08:
                Ins_PHP();
                break;
            case ORA_IMMEDIATE_09:
                Ins_ORA();
                break;
            case ASL_ACCUMULATOR_0A:
                Ins_ASL();
                break;
            case ORA_ABSOLUTE_0D:
                Ins_ORA();
                break;
            case ASL_ABSOLUTE_0E:
                Ins_ASL();
                break;
            default:
                printf("Unknown opcode [0x00]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x10:
            switch (opcode)
            {
            case BPL_RELATIVE_10:
                Ins_BPL();
                break;
            case ORA_INDIRECTY_11:
                Ins_ORA();
                break;
            case ORA_ZEROPAGEX_15:
                Ins_ORA();
                break;
            case ASL_ZEROPAGEX_16:
                Ins_ASL();
                break;
            case CLC_18:
                Ins_CLC();
                break;
            case ORA_ABSOLUTEY_19:
                Ins_ORA();
                break;
            case ORA_ABSOLUTEX_1D:
                Ins_ORA();
                break;
            case ASL_ABSOLUTEX_1E:
                Ins_ASL();
                break;
            default:
                printf("Unknown opcode [0x10]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x20:
            switch (opcode)
            {
            case JSR_ABSOLUTE_20:
                Ins_JSR();
                break;
            case AND_INDIRECTX_21:
                Ins_AND();
                break;
            case BIT_ZEROPAGE_24:
                Ins_BIT();
                break;
            case AND_ZEROPAGE_25:
                Ins_AND();
                break;
            case ROL_ZEROPAGE_26:
                Ins_ROL();
                break;
            case PLP_28:
                Ins_PLP();
                break;
            case AND_IMMEDIATE_29:
                Ins_AND();
                break;
            case ROL_ACCUMULATOR_2A:
                Ins_ROL();
                break;
            case BIT_ABSOLUTE_2C:
                Ins_BIT();
                break;
            case AND_ABSOLUTE_2D:
                Ins_AND();
                break;
            case ROL_ABSOLUTE_2E:
                Ins_ROL();
                break;
            default:
                printf("Unknown opcode [0x20]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x30:
            switch (opcode)
            {
            case BMI_RELATIVE_30:
                Ins_BMI();
                break;
            case AND_INDIRECTY_31:
                Ins_AND();
                break;
            case AND_ZEROPAGEX_35:
                Ins_AND();
                break;
            case ROL_ZEROPAGEX_36:
                Ins_ROL();
                break;
            case SEC_38:
                Ins_SEC();
                break;
            case AND_ABSOLUTEY_39:
                Ins_AND();
                break;
            case AND_ABSOLUTEX_3D:
                Ins_AND();
                break;
            case ROL_ABSOLUTEX_3E:
                Ins_ROL();
                break;
            default:
                printf("Unknown opcode [0x30]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x40:
            switch (opcode)
            {
            case RTI_40:
                Ins_RTI();
                break;
            case EOR_INDIRECTX_41:
                Ins_EOR();
                break;
            case EOR_ZEROPAGE_45:
                Ins_EOR();
                break;
            case LSR_ZEROPAGE_46:
                Ins_LSR();
                break;
            case PHA_48:
                Ins_PHA();
                break;
            case EOR_IMMEDIATE_49:
                Ins_EOR();
                break;
            case LSR_ACCUMULATOR_4A:
                Ins_LSR();
                break;
            case JMP_ABSOLUTE_4C:
                Ins_JMP();
                break;
            case EOR_ABSOLUTE_4D:
                Ins_EOR();
                break;
            case LSR_ABSOLUTE_4E:
                Ins_LSR();
                break;
            default:
                printf("Unknown opcode [0x40]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x50:
            switch (opcode)
            {
            case BVC_RELATIVE_50:
                Ins_BVC();
                break;
            case EOR_INDIRECTY_51:
                Ins_EOR();
                break;
            case EOR_ZEROPAGEX_55:
                Ins_EOR();
                break;
            case LSR_ZEROPAGEX_56:
                Ins_LSR();
                break;
            case CLI_58:
                Ins_CLI();
                break;
            case EOR_ABSOLUTEY_59:
                Ins_EOR();
                break;
            case EOR_ABSOLUTEX_5D:
                Ins_EOR();
                break;
            case LSR_ABSOLUTEX_5E:
                Ins_LSR();
                break;
            default:
                printf("Unknown opcode [0x50]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x60:
            switch (opcode)
            {
            case RTS_60:
                Ins_RTS();
                break;
            case ADC_INDIRECTX_61:
                Ins_ADC();
                break;
            case ADC_ZEROPAGE_65:
                Ins_ADC();
                break;
            case ROR_ZEROPAGE_66:
                Ins_ROR();
                break;
            case PLA_68:
                Ins_PLA();
                break;
            case ADC_IMMEDIATE_69:
                Ins_ADC();
                break;
            case ROR_ACCUMULATOR_6A:
                Ins_ROR();
                break;
            case JMP_INDIRECT_6C:
                Ins_JMP();
                break;
            case ADC_ABSOLUTE_6D:
                Ins_ADC();
                break;
            case ROR_ABSOLUTE_6E:
                Ins_ROR();
                break;
            default:
                printf("Unknown opcode [0x60]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x70:
            switch (opcode)
            {
            case BVS_RELATIVE_70:
                Ins_BVS();
                break;
            case ADC_INDIRECTY_71:
                Ins_ADC();
                break;
            case ADC_ZEROPAGEX_75:
                Ins_ADC();
                break;
            case ROR_ZEROPAGEX_76:
                Ins_ROR();
                break;
            case SEI_78:
                Ins_SEI();
                break;
            case ADC_ABSOLUTEY_79:
                Ins_ADC();
                break;
            case ADC_ABSOLUTEX_7D:
                Ins_ADC();
                break;
            case ROR_ABSOLUTE_6E:
                Ins_ROR();
                break;
            default:
                printf("Unknown opcode [0x70]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x80:
            switch (opcode)
            {
            case STA_INDIRECTX_81:
                Ins_STA();
                break;
            case STY_ZEROPAGE_84:
                Ins_STY();
                break;
            case STA_ZEROPAGE_85:
                Ins_STA();
                break;
            case STX_ZEROPAGE_86:
                Ins_STX();
                break;
            case DEY_88:
                Ins_DEY();
                break;
            // case NOP: // unnoficial
            //     break;
            case TXA_8A:
                Ins_TXA();
                break;
            case STY_ABSOLUTE_8C:
                Ins_STY();
                break;
            case STA_ABSOLUTE_8D:
                Ins_STA(); 
                break;
            case STX_ABSOLUTE_8E:
                Ins_STX();
                break;
            default:
                printf("Unknown opcode [0x80]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0x90:
            switch (opcode)
            {
            case BCC_RELATIVE_90:
                Ins_BCC();
                break;
            case STA_INDIRECTY_91:
                Ins_STA();
                break;
            case STY_ZEROPAGEX_94:
                Ins_STY();
                break;
            case STA_ZEROPAGEX_95:
                Ins_STA();
                break;
            case STX_ZEROPAGEY_96:
                Ins_STX();
                break;
            case TYA_98:
                Ins_TYA();
                break;
            case STA_ABSOLUTEY_99:
                Ins_STA();
                break;
            case TXS_9A:
                Ins_TXS();
                break;
            case STA_ABSOLUTEX_9D:
                Ins_STA();
                break;
            default:
                printf("Unknown opcode [0x90]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0xA0:
            switch (opcode)
            {
            case LDY_IMMEDIATE_A0:
                Ins_LDY();
                break;
            case LDA_INDIRECTX_A1:
                Ins_LDA();
                break;
            case LDX_IMMEDIATE_A2:
                Ins_LDX();
                break;
            case LDY_ZEROPAGE_A4:
                Ins_LDY();
                break;
            case LDA_ZEROPAGE_A5:
                Ins_LDA();
                break;
            case LDX_ZEROPAGE_A6:
                Ins_LDX();
                break;
            case TAY_A8:
                Ins_TAY();
                break;
            case LDA_IMMEDIATE_A9:
                Ins_LDA();
                break;
            case TAX_AA:
                Ins_TAX();
                break;
            case LDY_ABSOLUTE_AC:
                Ins_LDY();
                break;
            case LDA_ABSOLUTE_AD:
                Ins_LDA();        
                break;
            case LDX_ABSOLUTE_AE:
                Ins_LDX();
                break;
            default:
                printf("Unknown opcode [0xA0]: 0x%X\n", opcode);
                exit(1);
                break;
            }
            break;
        case 0xB0:
            switch (opcode)
            {
            case BCS_RELATIVE_B0:
                Ins_BCS();
                break;
            case LDA_INDIRECTY_B1:
                Ins_LDA();
                break;
            case LDY_ZEROPAGEX_B4:
                Ins_LDY();
                break;
            case LDA_ZEROPAGEX_B5:
                Ins_LDA();
                break;
            case LDX_ZEROPAGEY_B6:
                Ins_LDX();
                break;
            case CLV_B8:
                Ins_CLV();
                break;
            case LDA_ABSOLUTEY_B9:
                Ins_LDA();
                break;
            case TSX_BA:
                Ins_TSX();
                break;
            case LDY_ABSOLUTEX_BC:
                Ins_LDY();
                break;
            case LDA_ABSOLUTEX_BD:
                Ins_LDA();
                break;
            case LDX_ABSOLUTEY_BE:
                Ins_LDX();
                break;
            default:
                printf("Unknown opcode [0xB0]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0xC0:
            switch (opcode)
            {
            case CPY_IMMEDIATE_C0:
                Ins_CPY();
                break;
            case CMP_INDIRECTX_C1:
                Ins_CMP();
                break;
            case CPY_ZEROPAGE_C4:
                Ins_CPY();
                break;
            case CMP_ZEROPAGE_C5:
                Ins_CMP();
                break;
            case DEC_ZEROPAGE_C6:
                Ins_DEC();
                break;
            case INY_C8:
                Ins_INY();
                break;
            case CMP_IMMEDIATE_C9:
                Ins_CMP();
                break;
            case DEX_CA:
                Ins_DEX();
                break;
            case CPY_ABSOLUTE_CC:
                Ins_CPY();
                break;
            case CMP_ABSOLUTE_CD:
                Ins_CMP();
                break;
            case DEC_ABSOLUTE_CE:
                Ins_DEC();
                break;
            default:
                printf("Unknown opcode [0xC0]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0xD0:
            switch (opcode)
            {
            case BNE_RELATIVE_D0:
                Ins_BNE();
                break;
            case CMP_INDIRECTY_D1:
                Ins_CMP();
                break;
            case CMP_ZEROPAGEX_D5:
                Ins_CMP();
                break;
            case DEC_ZEROPAGEX_D6:
                Ins_DEC();
                break;
            case CLD_D8:
                Ins_CLD();
                break;
            case CMP_ABSOLUTEY_D9:
                Ins_CMP();
                break;
            case CMP_ABSOLUTEX_DD:
                Ins_CMP();
                break;
            case DEC_ABSOLUTEX_DE:
                Ins_DEC();
                break;
            default:
                printf("Unknown opcode [0xD0]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0xE0:
            switch (opcode)
            {
            case CPX_IMMEDIATE_E0:
                Ins_CPX();
                break;
            case SBC_INDIRECTX_E1:
                Ins_SBC();
                break;
            case CPX_ZEROPAGE_E4:
                Ins_CPX();
                break;
            case SBC_ZEROPAGE_E5:
                Ins_SBC();
                break;
            case INC_ZEROPAGE_E6:
                Ins_INC();
                break;
            case INX_E8:
                Ins_INX();
                break;
            case SBC_IMMEDIATE_E9:
                Ins_SBC();
                break;
            case NOP_EA:
                Ins_NOP();
                break;
            case CPX_ABSOLUTE_EC:
                Ins_CPX();
                break;
            case SBC_ABSOLUTE_ED:
                Ins_SBC();
                break;
            case INC_ABSOLUTE_EE:
                Ins_INC();
                break;
            default:
                printf("Unknown opcode [0xE0]: 0x%X\n", opcode);
                break;
            }
            break;
        case 0xF0:
            switch (opcode)
            {
            case BEQ_RELATIVE_F0:
                Ins_BEQ();
                break;
            case SBC_INDIRECTY_F1:
                Ins_SBC();
                break;
            case SBC_ZEROPAGEX_F5:
                Ins_SBC();
                break;
            case INC_ZEROPAGEX_F6:
                Ins_INC();
                break;
            case SED_F8:
                Ins_SED();
                break;
            case SBC_ABSOLUTEY_F9:
                Ins_SBC();
                break;
            case SBC_ABSOLUTEX_FD:
                Ins_SBC();
                break;
            case INC_ABSOLUTEX_FE:
                Ins_INC();
                break;
            default:
                printf("Unknown opcode [0xF0]: 0x%X\n", opcode);
                exit(1);
                break;
            }
            break;
        default:
            printf("Unknown opcode 0x%X\n", opcode);
            exit(1);
            break;
        }
    }

    if(cycleCounter == 0){
        printf("cycle error\n");
        printf("opcode error: 0x%X\n",opcode);
        printf("pc: 0x%X\n",pc);
        while (true){}
    }

    if(cycleCounter == debugCycle){
        printf("cicle not updated\n");
        printf("opcode error: 0x%X\n",opcode);
        printf("cycle error: %d\n",cycleCounter);
        while (true){}
    }
    if(cycleCounter == cycleCounterMax){
        cycleCounter = 0;
    }
    if(cycleCounter > cycleCounterMax){
        printf("limite passed\n");
        printf("opcode: 0x%X\n",opcode);
        printf("pc: 0x%X\n",pc);
        printf("pc before: 0x%X\n",pcdebug);
        while (true){
        }
        
    }
}

void Emulator::Initialize(const char* file){
    game = new Cartridge;
    game->LoadGame(file);
    cpu.Initialize();
    ppu.Initialize();
    int i = 0;
    while (i < 0x800)
    {
        int j = i & 0x2;
        bool swap = (i & 0x1F) >= 0x10;
        if ((j < 0x2) == !swap)
        {
            ppu.VRAM[i] = 0xF0;
        }
        else
        {
            ppu.VRAM[i] = 0x0F;
        }
        i++;
    }
}

shared_memory_buffer * data;

int nes_Button_A = KEY_L;
int nes_Button_B = KEY_B;
int nes_Button_Select = KEY_Q;
int nes_Button_Start = KEY_E;
int nes_Button_Up = KEY_W;
int nes_Button_Down = KEY_S;
int nes_Button_Left = KEY_A;
int nes_Button_Right = KEY_D;

void Emulator::Cycle(){
    cpu.EmulateCycle();
    ppu.EmulateCycle();
    ppu.EmulateCycle();
    ppu.EmulateCycle();
    if (controller.shiftCounter1 > 0){
        controller.shiftCounter1--;
        if (controller.shiftCounter1 == 0)
        {
            controller.controllerShiftRegister1 <<= 1;
            controller.controllerShiftRegister1 |= 1;
        }
    }
    if(controller.controllerStrobe == true){
        controller.controllerData1 = 0;
        if(IsKeyDown(nes_Button_Right) == true){
            controller.controllerData1 = 1;
        }
        if(IsKeyDown(nes_Button_Left) == true){
            controller.controllerData1 |= 2;
        }
        if(IsKeyDown(nes_Button_Down) == true){
            controller.controllerData1 |= 4;
        }
        if(IsKeyDown(nes_Button_Up) == true){
            controller.controllerData1 |= 8;
        }
        if(IsKeyDown(nes_Button_Start) == true){
            controller.controllerData1 |= 16;
        }
        if(IsKeyDown(nes_Button_Select) == true){
            controller.controllerData1 |= 32;
        }
        if(IsKeyDown(nes_Button_B) == true){
            controller.controllerData1 |= 64;
        }
        if(IsKeyDown(nes_Button_A) == true){
            controller.controllerData1 |= 128;
        }
        controller.controllerShiftRegister1 = controller.controllerData1;
    }
}

Controller controller;
RenderTexture2D target;
int gameScreenWidth;
int gameScreenHeight;
float scale;
int main(int argc, char const *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if(CreateMainWindow() == 1){
        return 1;
    }
    const int screenWidth = 256*4;// 800;
    const int screenHeight = 240*4;//480*1.5f;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - drop files");
    SetWindowMinSize(64, 32);
    
    // int gameScreenWidth = 800;
    // int gameScreenHeight = 480;
    
    gameScreenWidth = 256;
    gameScreenHeight = 240;

    
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);  // Texture scale filter to use

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    emulator.Initialize("DonkeyK.nes");
    
    RenderTexture2D target = LoadRenderTexture(256, 240);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    int contador = 0;
    unsigned int command = 1;

    //Remove shared memory on construction and destruction
    struct shm_remove
    {
        shm_remove() { shared_memory_object::remove("MyName"); }
        ~shm_remove(){ shared_memory_object::remove("MyName"); }
    } remover;

    //Create a shared memory object.
    shared_memory_object shm
        ( create_only                 //only create
        , "MyName" //name
        , read_write                  //read-write mode
        );

    //Set size
    shm.truncate(sizeof(shared_memory_buffer));

    //Map the whole shared memory in this process
    mapped_region region
        (shm                       //What to map
        ,read_write //Map it as read-write
        );

    //Get the address of the mapped region
    void * addr       = region.get_address();

    //Construct the shared structure in memory
    data = new (addr) shared_memory_buffer;

    data->commandMain = 1;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // data->items[0]
        
        if(rayWindow_Debugger == false){
            rayWindow_Debugger = true;
           
            data->nempty.wait();
            data->mutex.wait();
            
            printf("main\n");
            OpenWindow();
            data->mutex.post();
            data->nstored.post();
        }

        if(data->commandMain == 1){
            
            data->nempty.wait();
            data->mutex.wait();
            data->debugger.pc = emulator.cpu.pc;
            data->debugger.x = emulator.cpu.registerX;
            data->debugger.y = emulator.cpu.registerY;
            data->debugger.a = emulator.cpu.accumulator;
            data->debugger.status = emulator.cpu.statusFlag;
            data->debugger.ppuDot = emulator.ppu.scanlineDot;
            data->debugger.ppuScanline = emulator.ppu.scanlineRow;
            
            data->commandMain = 0;
            data->mutex.post();
            data->nstored.post();
        }else if(data->commandMain == 2){
            
            data->nempty.wait();
            data->mutex.wait();
            
            data->debugger.pc = emulator.cpu.pc;
            data->debugger.x = emulator.cpu.registerX;
            data->debugger.y = emulator.cpu.registerY;
            data->debugger.a = emulator.cpu.accumulator;
            data->debugger.status = emulator.cpu.statusFlag;
            data->debugger.ppuDot = emulator.ppu.scanlineDot;
            data->debugger.ppuScanline = emulator.ppu.scanlineRow;
            data->mutex.post();
            data->nstored.post();
            
            printf("termine\n");
            while (data->commandMain == 2){}
            printf("sali\n");
        } else if(data->commandMain == 3 || data->commandMain == 4){
            
            if (data->debugger.pc != emulator.cpu.pc){
                data->nempty.wait();
                data->mutex.wait();
                
                data->debugger.pc = emulator.cpu.pc;
                data->debugger.x = emulator.cpu.registerX;
                data->debugger.y = emulator.cpu.registerY;
                data->debugger.a = emulator.cpu.accumulator;
                data->debugger.status = emulator.cpu.statusFlag;
                data->debugger.ppuDot = emulator.ppu.scanlineDot;
                data->debugger.ppuScanline = emulator.ppu.scanlineRow;
                data->mutex.post();
                data->nstored.post();
                
                printf("termine\n");
                int stopValue = data->commandMain;
                while (data->commandMain == stopValue){}
                printf("sali\n");
            }
        }
        // printf("post\n");
        // printf("pc: 0x%x\n",emulator.cpu.pc);
        // if(rayWindow_Debugger == false){
        //     rayWindow_Debugger = true;
        //     OpenWindow();
        //     int contdeb=0;
        //     while (ReceiveFromOthers(command) == 1)
        //     {
        //         if(contdeb < 10){
        //         printf("no abrio\n");
        //         contdeb++;                    
        //         }
        //     }
        //     command = 1;
        //     printf("sali de openwindo\n");
        // }
        // if(command == 0){
        //     // printf("recibiendo, pc: 0x%x\n",emulator.cpu.pc);
        //     ReceiveFromOthers(command);
        // }else if(command == 1){
        //     printf("pc: 0x%x\n",emulator.cpu.pc);
        //     size_t size = 24;
        //     std::string data(size,'\0');
        //     // data.resize(24);
            
        //     memcpy(&data[0], &emulator.cpu.pc, sizeof(emulator.cpu.pc));
        //     memcpy(&data[2], &emulator.cpu.registerX, sizeof(emulator.cpu.registerX));
        //     memcpy(&data[3], &emulator.cpu.registerY, sizeof(emulator.cpu.registerY));
        //     // printf("data size: %d\n",data.size());
        //     for(int i=0; i<data.size();i++){
        //         printf("[%d]: 0x%x\n",i,data[i]);
        //     }
        //     // printf("string : 0x%x\n",data);
        //     printf("string 0: 0x%x\n",data[0]);
        //     printf("string 1: 0x%x\n",data[1]);
        //     printf("string 2: 0x%x\n",data[2]);
        //     SendDebugger(data);
        //     while (ReceiveFromOthers(command) == 1)
        //     {
        //     }
            
        //     command = 0;
        // }else if(command == 2){
        //     size_t size = 24;
        //     std::string data(size,'\0');
        //     // data.resize(24);
            
        //     memcpy(&data[0], &emulator.cpu.pc, sizeof(emulator.cpu.pc));
        //     SendDebugger(data);
        //     command = 0;
        // }
        scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);
        // Update
        //----------------------------------------------------------------------------------
        emulator.Cycle();
        contador++;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseMainWindow();
    CloseWindow();          // Close window and OpenGL context
    if(rayWindow_Debugger == true){
        // closePipeClient();
        closeDebuggerWindow();
    }
    //--------------------------------------------------------------------------------------

    return 0;
}
