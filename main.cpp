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

int nes_Button_A = KEY_L;
int nes_Button_B = KEY_K;
int nes_Button_Select = KEY_Q;
int nes_Button_Start = KEY_E;
int nes_Button_Up = KEY_W;
int nes_Button_Down = KEY_S;
int nes_Button_Left = KEY_A;
int nes_Button_Right = KEY_D;

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
    NOP_04 = 0X04,
    NOP_ABSOLUTE_0C = 0X0C,
    NOP_ZEROPAGEX_14 = 0X14,
    NOP_1A = 0X1A,
    NOP_ABSOLUTEX_1C = 0X1C,
    NOP_ZEROPAGEX_34 = 0X34,
    NOP_3A = 0X3A,
    NOP_ABSOLUTEX_3C = 0X3C,
    NOP_44 = 0X44,
    NOP_ZEROPAGEX_54 = 0X54,
    NOP_5A = 0X5A,
    NOP_ABSOLUTEX_5C = 0X5C,
    NOP_64 = 0X64,
    NOP_74 = 0X74,
    NOP_7A = 0X7A,
    NOP_ZEROPAGEX_74 = 0X74,
    NOP_ABSOLUTEX_7C = 0X7C,
    NOP_IMMEDIATE_80 = 0X80,
    NOP_IMMEDIATE_82 = 0X82,
    NOP_IMMEDIATE_89 = 0X89,
    NOP_IMMEDIATE_C2 = 0XC2,
    NOP_IMMEDIATE_E2 = 0XE2,
    NOP_DA = 0XDA,
    NOP_ZEROPAGEX_D4 = 0XD4,
    NOP_ABSOLUTEX_DC = 0XDC,
    NOP_EA = 0XEA, // only one official
    NOP_FA = 0XFA,
    NOP_ZEROPAGEX_F4 = 0XF4,
    NOP_ABSOLUTEX_FC = 0XFC,

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

    // Unofficial
    // SLO 
    SLO_INDIRECTX_03 = 0X03,
    SLO_ZEROPAGE_07 = 0X07,
    SLO_ABSOLUTE_0F = 0X0F,
    SLO_INDIRECTY_13 = 0X13,
    SLO_ZEROPAGEX_17 = 0X17,
    SLO_ABSOLUTEY_1B = 0X1B,
    SLO_ABSOLUTEX_1F = 0X1F,
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
    unsigned short address = (unsigned short) (emulator.Fetch(SECTIONS6502::RESET) | emulator.Fetch(SECTIONS6502::RESET+1) << 8); //(unsigned short) (Fetch(SECTIONS6502::RESET) | Fetch(SECTIONS6502::RESET) << 8);
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
    }else if (address >= SECTIONS6502::PPU_SECTION && address < SECTIONS6502::APUREGISTERS){
        cpu.cycleCounter++;
        address = (address & 0x2007);
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
            if((ppu.registerV & 0x3FF) >= 0x3F00){
                return ppu.FetchPPU((unsigned short)(ppu.ppuAddress & 0x3FFF));
            }
            // https://www.nesdev.org/wiki/PPU_registers#PPUDATA
            // The PPUDATA read buffer
            unsigned char delayReadBuffer;
            delayReadBuffer = ppu.PPU_readBuffer;
            ppu.PPU_readBuffer = ppu.FetchPPU((unsigned short)(ppu.registerV & 0x3FFF));
            ppu.ppuBus = delayReadBuffer;

            ppu.IncrementVramAddress();

            return delayReadBuffer;
        }
    }else if(address >= 0x4000 && address <= 0x401F){
        if(address == 0x4014){

            printf("4014\n");
            while (true)
            {
            }
        }else if (address == 0x4016){
            // Unconnected data lines and open bus
            unsigned char inputData = ((controller.controllerShiftRegister1 & 0x80)>>7) | (cpu.dataBus & 0xE0);
            controller.shiftCounter1 = 2;
            controller.controllerStopRead = false;
            emulator.cpu.dataBus = inputData;
            cpu.cycleCounter++;
            return inputData;
        }else if(address == 0x4017){
        }else{
            // printf("APU, DMC\n");
            // printf("not more, PC: 0x%X\n",emulator.cpu.pc);
            // while (true)
            // {
            // }
        }
        cpu.cycleCounter++;
    }else {
        data = MapperFetch(address, game->mapper);
        cpu.cycleCounter++;
        return data;
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
    }else if((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017){
        emulator.apu.Store(value, address); // APU registers
    }else if(address == 0x4014){ // OAMDMA WRITE
        emulator.ppu.StoreInPPU(value, address);
        DMAactive = true;
    }else if(address == 0x4016){
        controller.controllerStrobe = ((value & 1) != 0); // Activate/Desactivate read of controller
        if (!controller.controllerStrobe){
            controller.controllerStopRead = false;
        }
    }
}

// (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT, LAX, NOP)
void CPU6502::AbsoluteReadIns(){
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = dataBus;
        dataBus = emulator.Fetch(pc);
        dataAddress |= (dataBus<<8);
        // dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        pc++;
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }
}

// (ASL, LSR, ROL, ROR, INC, DEC, SLO, SRE, RLA, RRA, ISB, DCP)
void CPU6502::AbsoluteReadModWriteIns(){
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        pc++;
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 4){
        StoreInCPUMemory(dataBus, dataAddress); // Dummy write
        if(opcode == ASL_ABSOLUTE_0E){
            Ope_ASL();
        }else if(opcode == SLO_ABSOLUTE_0F){
            Ope_ASL();
            Ope_ORA();
        }else if(opcode == LSR_ABSOLUTE_4E){
            if((dataBus & CPUFLAGS::CARRY) == 0x1){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }

            dataBus = dataBus >> 1;

            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == ROL_ABSOLUTE_2E){
            char carryFuture = 0;
            if (((dataBus) & 0x80) == 0x80){
                carryFuture = 1;
            }
            dataBus = dataBus << 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 1;
            }
            if(carryFuture == 1){
                SetFlag(CARRY);
            }else{
                ClearFlag(CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == ROR_ABSOLUTE_6E){
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
        }else if(opcode == INC_ABSOLUTE_EE){
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == DEC_ABSOLUTE_CE){
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
        }
        cycleCounter++;
    }else if(cycleCounter == 5){
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

// (STA, STX, STY, SAX)
void CPU6502::AbsoluteWriteIns(){
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        pc++;
    }else if(cycleCounter == 3){
        if(opcode == STA_ABSOLUTE_8D){
           StoreInCPUMemory(accumulator,dataAddress);
        }else if(opcode == STX_ABSOLUTE_8E){
           StoreInCPUMemory(registerX,dataAddress);
        }else if(opcode == STY_ABSOLUTE_8C){
           StoreInCPUMemory(registerY,dataAddress);
        }/*else if(opcode == SAX){
           StoreInCPUMemory(accumulator,dataAddress);
        }*/
        cycleCounter++;
    }
}

// (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT, LAX, NOP)
void CPU6502::ZeroPageReadIns(){
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress);
    }
}

// (ASL, LSR, ROL, ROR, INC, DEC, SLO, SRE, RLA, RRA, ISB, DCP)
void CPU6502::ZeroPageReadModWriteIns(){
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 3){
        StoreInCPUMemory(dataBus, dataAddress); // Dummy write
        if(opcode == ASL_ZEROPAGE_06){
            Ope_ASL();
        }else if(opcode == SLO_ZEROPAGE_07){
            Ope_ASL();
            Ope_ORA();
        }else if(opcode == LSR_ZEROPAGE_46){
            if((dataBus & CPUFLAGS::CARRY) == 0x1){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }

            dataBus = dataBus >> 1;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == ROL_ZEROPAGE_26){
            char carryFuture = 0;
            if (((dataBus) & 0x80) == 0x80){
                carryFuture = 1;
            }
            dataBus = dataBus << 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 1;
            }
            if(carryFuture == 1){
                SetFlag(CARRY);
            }else{
                ClearFlag(CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == ROR_ZEROPAGE_66){
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
        }else if(opcode == INC_ZEROPAGE_E6){
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == DEC_ZEROPAGE_C6){
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
        }
        cycleCounter++;
    }else if(cycleCounter == 4){
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

// (STA, STX, STY, SAX)
void CPU6502::ZeroPageWriteIns(){
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        if(opcode == STA_ZEROPAGE_85){
            StoreInCPUMemory(accumulator,dataAddress);
        }else if(opcode == STX_ZEROPAGE_86){
            StoreInCPUMemory(registerX,dataAddress);
        }else if(opcode == STY_ZEROPAGE_84){
            StoreInCPUMemory(registerY,dataAddress);
        }
        cycleCounter++;
    }
}

void CPU6502::ZeroPageXReadIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerX);
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }
}

void CPU6502::ZeroPageXReadModWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerX);
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 4){
        StoreInCPUMemory(dataBus, dataAddress); // Dummy Write
        if(opcode == ASL_ZEROPAGEX_16){
            Ope_ASL();
        }else if(opcode == SLO_ZEROPAGEX_17){
            Ope_ASL();
            Ope_ORA();
        }
        else if(opcode == DEC_ZEROPAGEX_D6){
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == INC_ZEROPAGEX_F6){
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == LSR_ZEROPAGEX_56){
            if((dataBus & CPUFLAGS::CARRY) == 0x1){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            dataBus = dataBus >> 1;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == ROL_ZEROPAGEX_36){
            char carryFuture = 0;
            if (((dataBus) & 0x80) == 0x80){
                carryFuture = 1;
            }
            dataBus = dataBus << 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 1;
            }
            if(carryFuture == 1){
                SetFlag(CARRY);
            }else{
                ClearFlag(CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == ROR_ZEROPAGEX_76){
            char carryFuture = 0; //value bit 0
            if (((dataBus) & 0x01) == 0x01){
                carryFuture = 1;
            }

            dataBus = dataBus >> 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 0x80;
            }
            if(carryFuture == 1){
                SetFlag(CARRY);
            }else{
                ClearFlag(CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
        }
        cycleCounter++;
    }else if(cycleCounter == 5){
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

void CPU6502::ZeroPageXWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerX);
    }else if(cycleCounter == 3){
        if(opcode == STA_ZEROPAGEX_95){
            StoreInCPUMemory(accumulator,dataAddress);
        }else if(opcode == STY_ZEROPAGEX_94){
            StoreInCPUMemory(registerY,dataAddress);
        }
        cycleCounter++;
    }
}

void CPU6502::ZeroPageYReadIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerY);
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }
}

void CPU6502::ZeroPageYReadModWriteIns() {}

void CPU6502::ZeroPageYWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerY);
    }else if(cycleCounter == 3){
        if(opcode == STX_ZEROPAGEY_96){
            StoreInCPUMemory(registerX,dataAddress);
        }
        cycleCounter++;
    }
}

// (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT, LAX, LAE, SHS, NOP)
// 4 or 5 Max cycles
void CPU6502::AbsoluteXReadIns() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = dataBus;
        dataBus = emulator.Fetch(pc);
        dataAddress |= dataBus<<8;
        dataAddress += registerX;
        pc++;
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress);
    }
}

// (ASL, LSR, ROL, ROR, INC, DEC, SLO, SRE, RLA, RRA, ISB, DCP)
void CPU6502::AbsoluteXReadModWriteIns() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        dataAddress += registerX;
        pc++;
    }else if(cycleCounter == 3){
        // add 1 cycle by default because
        // the processor cannot undo a write to an invalid
        // address, it always reads from the address first.
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress-registerX) & 0xFF00) != (dataAddress & 0xFF00) ){
            // dataAddress += 0x100; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 5){
        StoreInCPUMemory(dataBus, dataAddress); // Dummy Write

        if(opcode == ASL_ABSOLUTEX_1E){
            Ope_ASL();
        }else if(opcode == SLO_ABSOLUTEX_1F){
            Ope_ASL();
            Ope_ORA();
        }else if(opcode == ROL_ABSOLUTEX_3E){
            char carryFuture = 0;
            if (((dataBus) & 0x80) == 0x80){
                carryFuture = 1;
            }
            dataBus = dataBus << 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 1;
            }
            if(carryFuture == 1){
                SetFlag(CARRY);
            }else{
                ClearFlag(CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == LSR_ABSOLUTEX_5E){
            if((dataBus & CPUFLAGS::CARRY) == 0x1){
                SetFlag(CPUFLAGS::CARRY);
            }else{
                ClearFlag(CPUFLAGS::CARRY);
            }
            dataBus = dataBus >> 1;
            isZero(dataBus);
            isNegative(dataBus);
        }
         else if(opcode == ROR_ABSOLUTEX_7E){
            char carryFuture = 0; //value bit 0
            if (((dataBus) & 0x01) == 0x01){
                carryFuture = 1;
            }

            dataBus = dataBus >> 1;
            if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
                dataBus |= 0x80;
            }
            if(carryFuture == 1){
                SetFlag(CARRY);
            }else{
                ClearFlag(CARRY);
            }
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == DEC_ABSOLUTEX_DE){
            dataBus--;
            isZero(dataBus);
            isNegative(dataBus);
        }else if(opcode == INC_ABSOLUTEX_FE){
            dataBus++;
            isZero(dataBus);
            isNegative(dataBus);
        }
        cycleCounter++;
    }else if(cycleCounter == 6){
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

// (STA, STX, STY, SHA, SHX, SHY)
void CPU6502::AbsoluteXWriteIns() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        dataAddress += registerX;
        pc++;
    }else if(cycleCounter == 3){
        // add 1 cycle by default because
        // the processor cannot undo a write to an invalid
        // address, it always reads from the address first.
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress-registerX) & 0xFF00) != (dataAddress & 0xFF00) ){
            // dataAddress += 0x100; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        if(opcode == STA_ABSOLUTEX_9D){
            StoreInCPUMemory(accumulator,dataAddress);
        }
        cycleCounter++;
    }
}

// (LDA, LDX, LDY, EOR, AND, ORA, ADC, SBC, CMP, BIT, LAX, LAE, SHS, NOP)
void CPU6502::AbsoluteYReadIns() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        dataAddress += registerY;
        pc++;
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);

        if(((dataAddress - registerY) & 0xFF00) != (dataAddress & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        cycleCounter++;
    }
}

// Unoffical Instructions
// (ASL, LSR, ROL, ROR, INC, DEC, SLO, SRE, RLA, RRA, ISB, DCP)
void CPU6502::AbsoluteYReadModWriteIns() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        dataAddress += registerY;
        pc++;
    }else if(cycleCounter == 3){
        // add 1 cycle by default because
        // the processor cannot undo a write to an invalid
        // address, it always reads from the address first.
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00) ){
            // dataAddress += 0x100; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 5){
        StoreInCPUMemory(dataBus, dataAddress); // Dummy Write
        if(opcode == SLO_ABSOLUTEY_1B){
            Ope_ASL();
            Ope_ORA();
        }
        cycleCounter++;
    }else if(cycleCounter == 6){
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

void CPU6502::AbsoluteYWriteIns() {
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        dataAddress += registerY;
        pc++;
    }else if(cycleCounter == 3){
        // add 1 cycle by default because
        // the processor cannot undo a write to an invalid
        // address, it always reads from the address first.
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00) ){
            // dataAddress += 0x100; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        if(opcode == STA_ABSOLUTEY_99){
            StoreInCPUMemory(accumulator,dataAddress);
        }
        cycleCounter++;
    }
}

// (LDA, ORA, EOR, AND, ADC, CMP, SBC, LAX)
void CPU6502::IndirectXReadIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // 00bb
        pc++;
    }else if(cycleCounter == 2){
        // https://www.nesdev.org/6502_cpu.txt: Indexed indirect addressing
        // Note: The effective address is always fetched from zero page,
        // i.e. the zero page boundary crossing is not handled.
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerX); // 00bb + X
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 4){
        dataAddress = (emulator.Fetch((unsigned char) (dataAddress+1))<<8) | dataBus;
    }else if(cycleCounter == 5){
        dataBus = emulator.Fetch(dataAddress);
    }
}

// Unoficial Instructions
// (SLO, SRE, RLA, RRA, ISB, DCP)
// https://www.masswerk.at/6502/6502_instruction_set.html#SLO
void CPU6502::IndirectXReadModWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // 00bb
        pc++;
    }else if(cycleCounter == 2){
        // https://www.nesdev.org/6502_cpu.txt: Indexed indirect addressing
        // Note: The effective address is always fetched from zero page,
        // i.e. the zero page boundary crossing is not handled.
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerX); // 00bb + X
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 4){
        dataAddress = (emulator.Fetch((unsigned char) (dataAddress+1))<<8) | dataBus;
    }else if(cycleCounter == 5){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 6){
        StoreInCPUMemory(dataBus, dataAddress); // dummy write
        if(opcode == SLO_INDIRECTX_03){
            Ope_ASL();
            Ope_ORA();
        }
        cycleCounter++;
    }else if(cycleCounter == 7){
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

// (STA, SAX)
void CPU6502::IndirectXWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // 00bb
        pc++;
    }else if(cycleCounter == 2){
        // https://www.nesdev.org/6502_cpu.txt: Indexed indirect addressing
        // Note: The effective address is always fetched from zero page,
        // i.e. the zero page boundary crossing is not handled.
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress = (unsigned char) (dataAddress + registerX); // 00bb + X
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 4){
        dataAddress = (emulator.Fetch((unsigned char) (dataAddress + 1))<<8) | dataBus;
    }else if(cycleCounter == 5){
        if(opcode == STA_INDIRECTX_81){
            StoreInCPUMemory(accumulator, dataAddress);
        }
        cycleCounter++;
    }
}

void CPU6502::IndirectYReadIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // 00bb
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // xx
    }else if(cycleCounter == 3){
        // https://www.nesdev.org/6502_cpu.txt: Indirect indexed addressing
        // Note: The effective address is always fetched from zero page,
        // i.e. the zero page boundary crossing is not handled.
        dataAddress = (emulator.Fetch((unsigned char) (dataAddress+1))<<8) | dataBus; // yyxx
        dataAddress += registerY; // yyxx + Y
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 5){
        dataBus = emulator.Fetch(dataAddress);
    }
}
// Unofficial instructions
// (SLO, SRE, RLA, RRA, ISB, DCP)
void CPU6502::IndirectYReadModWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // 00bb
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // xx
    }else if(cycleCounter == 3){
        // https://www.nesdev.org/6502_cpu.txt: Indirect indexed addressing
        // Note: The effective address is always fetched from zero page,
        // i.e. the zero page boundary crossing is not handled.
        dataAddress = (emulator.Fetch((unsigned char) (dataAddress+1))<<8) | dataBus; // yyxx
        dataAddress += registerY; // yyxx + Y
    }else if(cycleCounter == 4){
        dataBus = emulator.Fetch(dataAddress);
        // if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00)){
        //     cycleCounterMax++; // page crossing add 1 cycle
        // }
    }else if(cycleCounter == 5){ 
        dataBus = emulator.Fetch(dataAddress);
    }else if(cycleCounter == 6){ 
        StoreInCPUMemory(dataBus, dataAddress);// dummy write
        if(opcode == SLO_INDIRECTY_13){
            Ope_ASL();
            Ope_ORA();
        }
        cycleCounter++;
    }else if(cycleCounter == 7){ 
        StoreInCPUMemory(dataBus, dataAddress);
        cycleCounter++;
    }
}

// (STA)
void CPU6502::IndirectYWriteIns() {
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // 00bb
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // xx
    }else if(cycleCounter == 3){
        // https://www.nesdev.org/6502_cpu.txt: Indirect indexed addressing
        // Note: The effective address is always fetched from zero page,
        // i.e. the zero page boundary crossing is not handled.
        dataAddress = (emulator.Fetch((unsigned char) (dataAddress+1))<<8) | dataBus; // yyxx
        dataAddress += registerY; // yyxx + Y
    }else if(cycleCounter == 4){
        // add 1 cycle by default because
        // the processor cannot undo a write to an invalid
        // address, it always reads from the address first.
        dataBus = emulator.Fetch(dataAddress);
        if(((dataAddress-registerY) & 0xFF00) != (dataAddress & 0xFF00) ){
            // dataAddress += 0x100; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 5){
        if(opcode == STA_INDIRECTY_91){
            StoreInCPUMemory(accumulator, dataAddress);
        }
        cycleCounter++;
    }
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
        ZeroPageReadIns();
    }else if(opcode == ADC_ZEROPAGEX_75){
        // max 4 cycles
        ZeroPageXReadIns();
    }else if(opcode == ADC_ABSOLUTE_6D){
        // max 4 cycles
        AbsoluteReadIns();
    }else if(opcode == ADC_ABSOLUTEX_7D){
        // max 4 or 5 cycles
        AbsoluteXReadIns();
    }else if(opcode == ADC_ABSOLUTEY_79){
        // max 4 or 5cycles
        AbsoluteYReadIns();
    }else if(opcode == ADC_INDIRECTX_61){
        // max 6 cycles
        IndirectXReadIns();
    }else if(opcode == ADC_INDIRECTY_71){
        // max 5 cycles
        IndirectYReadIns();
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

void CPU6502::Ins_AND()
{
    if(opcode == AND_IMMEDIATE_29){
        // cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == AND_ZEROPAGE_25){
        // max 3 cycles
        ZeroPageReadIns();
    }else if(opcode == AND_ZEROPAGEX_35){
        // max 4 cycles
        ZeroPageXReadIns();
    }else if(opcode == AND_ABSOLUTE_2D){
        // max 4 cycles
        AbsoluteReadIns();
    }else if(opcode == AND_ABSOLUTEX_3D){
        // max 4 or 5 cycles
        AbsoluteXReadIns();
    }else if(opcode == AND_ABSOLUTEY_39){
        // max 4 or 5 cycles
        AbsoluteYReadIns();
    }else if(opcode == AND_INDIRECTX_21){
        // max 6 cycles
        IndirectXReadIns();
    }else if(opcode == AND_INDIRECTY_31){
        // max 5 or 6 cycles
        IndirectYReadIns();
    }

    if(cycleCounter == cycleCounterMax){
        accumulator = accumulator & dataBus;
        isZero(accumulator);
        isNegative(accumulator);
    }
}

// CYCLE ACCURACY: ALL
void CPU6502::Ins_ASL()
{
    if(opcode == ASL_ZEROPAGE_06){
        ZeroPageReadModWriteIns();
    }else if(opcode == ASL_ACCUMULATOR_0A){    
        if(cycleCounter == 1){            
            //cycle 2
            Ope_ASL();
            cycleCounter++;
        }
    }else if(opcode == ASL_ABSOLUTE_0E){
        AbsoluteReadModWriteIns();
    }else if(opcode == ASL_ZEROPAGEX_16){
        ZeroPageXReadModWriteIns();
    }else if(opcode == ASL_ABSOLUTEX_1E){
        AbsoluteXReadModWriteIns();
    }
}

void CPU6502::Ope_ASL(){
    if(opcode != ASL_ACCUMULATOR_0A){
        if (dataBus >= 0x80){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }

        dataBus = dataBus << 1;

        isZero(dataBus);
        isNegative(dataBus);
    }else{
        if (accumulator >= 0x80){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }

        accumulator = accumulator << 1;
        isZero(accumulator);
        isNegative(accumulator);
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

//CYCLE ACCURACY: ALL
void CPU6502::Ins_BIT() {
  if(opcode == BIT_ZEROPAGE_24){
    ZeroPageReadIns();
    if(cycleCounter == cycleCounterMax){
        dataBus = accumulator & dataBus;

        isZero(dataBus);

        if ((dataBus & 0x40) != 0) {
            SetFlag(CPUFLAGS::OVERFLOW);
        } else {
            ClearFlag(CPUFLAGS::OVERFLOW);
        }
        isNegative(dataBus);
    }
  }else if(opcode == BIT_ABSOLUTE_2C){
    AbsoluteReadIns();
    if(cycleCounter == cycleCounterMax){
        isZero(dataBus);

        if ((dataBus & 0x40) != 0) {
            SetFlag(CPUFLAGS::OVERFLOW);
        } else {
            ClearFlag(CPUFLAGS::OVERFLOW);
        }
        isNegative(dataBus);
    }
  }
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

    if(nmi == true){
        //NMI
        if(cycleCounter == 1){
            dataBus = emulator.Fetch(pc); // dummy read
        }else if(cycleCounter == 2){
            StoreInCPUMemory((unsigned char)(pc>>8),(unsigned short)(0x100 + sp));
            sp--;
            cycleCounter++;
        }else if(cycleCounter == 3){
            StoreInCPUMemory((unsigned char)(pc),(unsigned short)(0x100 + sp));
            sp--;
            cycleCounter++;
        }else if(cycleCounter == 4){
            ClearFlag(CPUFLAGS::B_FLAG); // clear only in NMI
            StoreInCPUMemory(statusFlag,(unsigned short)(0x100 + sp));
            sp--;
            cycleCounter++;
        }else if(cycleCounter == 5){
            pc = (unsigned short)((pc & 0xFF00) | (emulator.Fetch(0xFFFA)));            
        }else if(cycleCounter == 6){
            pc = (unsigned short)((pc & 0x00FF) | (emulator.Fetch(0xFFFB) << 8));
            nmi = false;
            SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
        }
    }else if(nmi == false){
        // BRK
        if(cycleCounter == 1){
            dataBus = emulator.Fetch(pc); // dummy read
            pc++;
        }else if(cycleCounter == 2){
            StoreInCPUMemory((unsigned char)(pc>>8),(unsigned short)(0x100 + sp));
            sp--;
            cycleCounter++;
        }else if(cycleCounter == 3){
            StoreInCPUMemory((unsigned char)(pc),(unsigned short)(0x100 + sp));
            sp--;
            cycleCounter++;
        }else if(cycleCounter == 4){
            SetFlag(CPUFLAGS::B_FLAG); // Set in BRK
            StoreInCPUMemory(statusFlag,(unsigned short)(0x100 + sp));
            sp--;
            cycleCounter++;
        }else if(cycleCounter == 5){
            pc = (unsigned short)((pc & 0xFF00) | (emulator.Fetch(0xFFFE)));
        }else if(cycleCounter == 6){
            pc = (unsigned short)((pc & 0x00FF) | (emulator.Fetch(0xFFFF) << 8));
            SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
        }
    }
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
    emulator.Fetch(pc); // dummy read
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

void CPU6502::Ins_CMP() {
    if(opcode == CMP_IMMEDIATE_C9){
        // max cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == CMP_ZEROPAGE_C5){
        // max cycle 3
        ZeroPageReadIns();
    }else if(opcode == CMP_ZEROPAGEX_D5){
        // max cycle 4
        ZeroPageXReadIns();
    }else if(opcode == CMP_ABSOLUTE_CD){
        // max cycle 4
        AbsoluteReadIns();
    }else if(opcode == CMP_ABSOLUTEX_DD){
        // max cycle 4 or 5
        AbsoluteXReadIns();
    }else if(opcode == CMP_ABSOLUTEY_D9){
        // max cycle 4 or 5
        AbsoluteYReadIns();
    }else if(opcode == CMP_INDIRECTX_C1){
        // max cycle 6
        IndirectXReadIns();
    }else if(opcode == CMP_INDIRECTY_D1){
        // max cycle 5 or 6
        IndirectYReadIns();
    }

    if(cycleCounter == cycleCounterMax){
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
}

void CPU6502::Ins_CPX() {
    if(opcode == CPX_IMMEDIATE_E0){
        // max cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == CPX_ZEROPAGE_E4){
        // max cycle 3
        ZeroPageReadIns();
    }else if(opcode == CPX_ABSOLUTE_EC){
        // max cycle 4
        AbsoluteReadIns();
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
}

void CPU6502::Ins_CPY() {
    if(opcode == CPY_IMMEDIATE_C0){
        // max cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == CPY_ZEROPAGE_C4){
        // max cycle 3
        ZeroPageReadIns();
    }else if(opcode == CPY_ABSOLUTE_CC){
        // max cycle 4
        AbsoluteReadIns();
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
}

//CYCLE ACCURACY: ALL
void CPU6502::Ins_DEC() {
    if(opcode == DEC_ZEROPAGE_C6){
        // max cycle 5
        ZeroPageReadModWriteIns();
    }else if(opcode == DEC_ZEROPAGEX_D6){
        // max cycle 6
        ZeroPageXReadModWriteIns();
    }else if(opcode == DEC_ABSOLUTE_CE){
        // max cycle 6
        AbsoluteReadModWriteIns();
    }else if(opcode == DEC_ABSOLUTEX_DE){
        // max cycle 7
        AbsoluteXReadModWriteIns();
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

void CPU6502::Ins_EOR() {
    if(opcode == EOR_IMMEDIATE_49){
        // max cycles 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == EOR_ZEROPAGE_45){
        // max cycles 3
        ZeroPageReadIns();
    }else if(opcode == EOR_ZEROPAGEX_55){
        // max cycles 4
        ZeroPageXReadIns();
    }else if(opcode == EOR_ABSOLUTE_4D){
        // max cycles 4
        AbsoluteReadIns();
    }else if(opcode == EOR_ABSOLUTEX_5D){
        // max cycles 4 or 5
        AbsoluteXReadIns();
    }else if(opcode == EOR_ABSOLUTEY_59){
        // max cycles 4 or 5
        AbsoluteYReadIns();
    }else if(opcode == EOR_INDIRECTX_41){
        // max cycles 6
        IndirectXReadIns();
    }else if(opcode == EOR_INDIRECTY_51){
        // max cycles 5 or 6
        IndirectYReadIns();
    }

    if(cycleCounter == cycleCounterMax){
        accumulator ^= dataBus;
        isZero(accumulator);
        isNegative(accumulator);
    }
}

void CPU6502::Ins_INC() {
    if(opcode == INC_ZEROPAGE_E6){
        // max cycles 5
        ZeroPageReadModWriteIns();
    } else if(opcode == INC_ZEROPAGEX_F6){
        // max cycles 6
        ZeroPageXReadModWriteIns();
    } else if(opcode == INC_ABSOLUTE_EE){
        // max cycles 6
        AbsoluteReadModWriteIns();
    } else if(opcode == INC_ABSOLUTEX_FE){
        // max cycles 7
        AbsoluteXReadModWriteIns();
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

//CYCLE ACCURATE: ALL
void CPU6502::Ins_JMP() {
    if(opcode == JMP_ABSOLUTE_4C){
        // max 3 cycles
        if(cycleCounter == 1){
            dataBus = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (unsigned short) ((emulator.Fetch(pc) << 8) | dataBus);
            pc++;
            pc = dataAddress;
        }
    }else if(opcode == JMP_INDIRECT_6C){
        // max 5 cycles
        if(cycleCounter == 1){
            dataBus = emulator.Fetch(pc);
            pc++;
        }else if(cycleCounter == 2){
            dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
            pc++;
        }else if(cycleCounter == 3){
            dataBus = emulator.Fetch(dataAddress); // Get the PCL
        }else if(cycleCounter == 4){
            pc = dataBus;
            // https://www.nesdev.org/6502_cpu.txt: Absolute indirect addressing
            // Get the PCH
            // The PCH will always be fetched from the same page
            // than PCL, i.e. page boundary crossing is not handled.
            // in other words: 0xZZ00, the "ZZ" from address not change
            dataBus = (emulator.Fetch( (unsigned short) ((dataAddress & 0xFF00) | (unsigned char)(dataAddress+1)))) ; 
            pc |= dataBus << 8;
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
        dataBus = emulator.Fetch(pc);
        pc = (unsigned short)((dataBus << 8) | sp);
        sp = (unsigned char) dataAddress;
    }
}

void CPU6502::Ins_LDA(){
    if(opcode == LDA_IMMEDIATE_A9){
        // cycle 2
        accumulator = emulator.Fetch(pc);
        pc++;
    }else if(opcode == LDA_ZEROPAGE_A5){
        ZeroPageReadIns();
    }else if(opcode == LDA_ZEROPAGEX_B5){
        ZeroPageXReadIns();
    }else if(opcode == LDA_ABSOLUTE_AD){
        AbsoluteReadIns();
    }else if(opcode == LDA_ABSOLUTEX_BD){
        AbsoluteXReadIns();
    }else if(opcode == LDA_ABSOLUTEY_B9){
        AbsoluteYReadIns();
    }else if(opcode == LDA_INDIRECTX_A1){
        IndirectXReadIns();
    }else if(opcode == LDA_INDIRECTY_B1){
        IndirectYReadIns();
    }

    if(cycleCounter == cycleCounterMax){
        if(opcode != LDA_IMMEDIATE_A9){
            accumulator = dataBus;
        }
        isZero(accumulator);
        isNegative(accumulator);
    }
}

void CPU6502::Ins_LDX(){
    if(opcode == LDX_IMMEDIATE_A2){
        //cycle 2
        registerX = emulator.Fetch(pc);
        pc++;
    }else if(opcode == LDX_ZEROPAGE_A6){
        // max cycle 3
        ZeroPageReadIns();
    }else if (opcode == LDX_ZEROPAGEY_B6){
        ZeroPageYReadIns();
    }else if(opcode == LDX_ABSOLUTE_AE){
        AbsoluteReadIns();
    }else if(opcode == LDX_ABSOLUTEY_BE){
        AbsoluteYReadIns();
    }

    if(cycleCounter == cycleCounterMax){
        if(opcode != LDX_IMMEDIATE_A2){
            registerX = dataBus;
        }
        isZero(registerX);
        isNegative(registerX);
    }
}

void CPU6502::Ins_LDY(){
    if(opcode == LDY_IMMEDIATE_A0){
        //cycle 2
        registerY = emulator.Fetch(pc);
        pc++;
    }else if(opcode == LDY_ZEROPAGE_A4){
        // max cycle 3
        ZeroPageReadIns();
    }else if (opcode == LDY_ZEROPAGEX_B4){
        ZeroPageXReadIns();
    }else if(opcode == LDY_ABSOLUTE_AC){
        AbsoluteReadIns();
    }else if(opcode == LDY_ABSOLUTEX_BC){
        // Max 4 or 5 cycles
        AbsoluteXReadIns();
    }


    if(cycleCounter == cycleCounterMax){
        if(opcode != LDY_IMMEDIATE_A0){
            registerY = dataBus;
        }
        isZero(registerY);
        isNegative(registerY);
    }
}

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
        AbsoluteReadModWriteIns();
    }else if(opcode == LSR_ZEROPAGE_46){
        ZeroPageReadModWriteIns();
    }else if(opcode == LSR_ZEROPAGEX_56){
        ZeroPageXReadModWriteIns();
    }else if(opcode == LSR_ABSOLUTEX_5E){
        AbsoluteXReadModWriteIns();
    }
}

void CPU6502::Ins_NOP() {
    cycleCounter++;
}

void CPU6502::Ins_NOP_IMMEDIATE(){
    dataBus = emulator.Fetch(pc);
    pc++;
}

void CPU6502::Ins_NOP_ABSOLUTE(){
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = dataBus;
        dataBus = emulator.Fetch(pc);
        dataAddress |= (dataBus<<8);
        pc++;
    }else if(cycleCounter == 3){
        dataBus = emulator.Fetch(dataAddress);
    }
}

void CPU6502::Ins_NOP_ABSOLUTEX(){
    if(cycleCounter == 1){
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataAddress = (emulator.Fetch(pc) << 8) | dataBus;
        dataAddress += registerX;
        pc++;
    }else if(cycleCounter == 3){
        emulator.Fetch(dataAddress);
        if(((dataAddress - registerX) & 0xFF00) != (dataAddress & 0xFF00)){
            cycleCounterMax++; // page crossing add 1 cycle
        }
    }else if(cycleCounter == 4){
        cycleCounter++;
    }
}

void CPU6502::Ins_NOP_ZEROPAGE(){
  // Max 3 cycles
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc); // dummy read
        pc++;
    }else if(cycleCounter == 2){
        emulator.Fetch(dataAddress); // dummy read
    }
}


void CPU6502::Ins_NOP_ZEROPAGEX(){
    if(cycleCounter == 1){
        dataAddress = emulator.Fetch(pc);
        pc++;
    }else if(cycleCounter == 2){
        dataBus = emulator.Fetch(dataAddress); // Dummy read
        dataAddress += registerX;
    }else if(cycleCounter == 3){
        emulator.Fetch(dataAddress);
    }
}

void CPU6502::Ins_ORA(){
    if(opcode == ORA_INDIRECTX_01){
        // cycle 2
        IndirectXReadIns();
    }else if(opcode == ORA_ZEROPAGE_05){
        ZeroPageReadIns();
    }else if(opcode == ORA_IMMEDIATE_09){
        // cycle 2
        dataBus = emulator.Fetch(pc);
        pc++;
    }else if(opcode == ORA_ABSOLUTE_0D){
        AbsoluteReadIns();
    }else if(opcode == ORA_INDIRECTY_11){
        IndirectYReadIns();
    }else if(opcode == ORA_ZEROPAGEX_15){
        ZeroPageXReadIns();
    }else if(opcode == ORA_ABSOLUTEY_19){
        AbsoluteYReadIns();
    }else if(opcode == ORA_ABSOLUTEX_1D){
        AbsoluteXReadIns();
    }

    if(cycleCounter == cycleCounterMax){
        //cycle 3
        Ope_ORA();
    }
}

void CPU6502::Ope_ORA(){
    accumulator |= dataBus;
    isZero(accumulator);
    isNegative(accumulator);
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
        statusFlag |= CPUFLAGS::B_FLAG;
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

void CPU6502::Ins_ROL(){
    if(opcode == ROL_ACCUMULATOR_2A){
        char carryFuture = 0;
        if (((accumulator) & 0x80) == 0x80){
            carryFuture = 1;
        }
        accumulator = accumulator << 1;
        if((statusFlag & CPUFLAGS::CARRY) == CPUFLAGS::CARRY){
            accumulator |= 1;
        }
        if(carryFuture == 1){
            SetFlag(CARRY);
        }else{
            ClearFlag(CARRY);
        }
        isZero(accumulator);
        isNegative(accumulator);
        cycleCounter++;
    }else if(opcode == ROL_ZEROPAGE_26){
        // max 5 cycles
        ZeroPageReadModWriteIns();
    }else if(opcode == ROL_ZEROPAGEX_36){
        // max 6 cycles
        ZeroPageXReadModWriteIns();
    }else if(opcode == ROL_ABSOLUTE_2E){
        // max 6 cycles
        AbsoluteReadModWriteIns();
    }else if(opcode == ROL_ABSOLUTEX_3E){
        // max 7 cycles
        AbsoluteXReadModWriteIns();
    }
}

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
        ZeroPageReadModWriteIns();
    }else if(opcode == ROR_ZEROPAGEX_76){
        // max 6 cycles
        ZeroPageXReadModWriteIns();
    }else if(opcode == ROR_ABSOLUTE_6E){
        // max 6 cycles
        AbsoluteReadModWriteIns();
    }else if(opcode == ROR_ABSOLUTEX_7E){
        // max 7 cycles
        AbsoluteXReadModWriteIns();
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
    }else if(opcode == SBC_ZEROPAGE_E5){
        // max 3 cycles
        ZeroPageReadIns();
    }else if(opcode == SBC_ZEROPAGEX_F5){
        // max 4 cycles
        ZeroPageXReadIns();
    }else if(opcode == SBC_ABSOLUTE_ED){
        // max 4 cycles
        AbsoluteReadIns();
    }else if(opcode == SBC_ABSOLUTEX_FD){
        // max 4 or 5 cycles
        AbsoluteXReadIns();
    }else if(opcode == SBC_ABSOLUTEY_F9){
        // max 4 or 5 cycles
        AbsoluteYReadIns();
    }else if(opcode == SBC_INDIRECTX_E1){
        // max 6 cycles
        IndirectXReadIns();
    }else if(opcode == SBC_INDIRECTY_F1){
        IndirectYReadIns();
    }

    if(cycleCounter == cycleCounterMax){
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
        if((result & 0x80) != 0x80){
            SetFlag(CPUFLAGS::CARRY);
        }else{
            ClearFlag(CPUFLAGS::CARRY);
        }
        isZero(result);
        isNegative(result);
        accumulator = result;
    }
}

void CPU6502::Ins_SEC() {
    // max 2 cycles
    SetFlag(CPUFLAGS::CARRY);
    cycleCounter++;
}

void CPU6502::Ins_SED() {
    // max 2 cycles
    // https://www.nesdev.org/wiki/Programming_with_unofficial_opcodes
    // NES doesn't have it,they do almost nothing; 
    // their effect is visible only after a PHP or BRK
    SetFlag(CPUFLAGS::DECIMAL_MODE);
    dataBus = emulator.Fetch(pc); // Dummy read
    // cycleCounter++;
}

void CPU6502::Ins_SEI() {
    // max 2 cycles
    // this is the 2 cycle
    SetFlag(CPUFLAGS::INTERRUPT_DISABLE);
    emulator.Fetch(pc); // Dummy read
    // DO IRQ
}

void CPU6502::Ins_SLO(){
    if(opcode == SLO_INDIRECTX_03){
        IndirectXReadModWriteIns();
    }else if(opcode == SLO_ZEROPAGE_07){
        ZeroPageReadModWriteIns();
    }else if(opcode == SLO_ABSOLUTE_0F){
        AbsoluteReadModWriteIns();
    }else if(opcode == SLO_INDIRECTY_13){
        IndirectYReadModWriteIns();
    }else if(opcode == SLO_ZEROPAGEX_17){
        ZeroPageXReadModWriteIns();
    }else if(opcode == SLO_ABSOLUTEY_1B){
        AbsoluteYReadModWriteIns();
    }else if(opcode == SLO_ABSOLUTEX_1F){
        AbsoluteXReadModWriteIns();
    }
}

void CPU6502::Ins_STA(){
    if(opcode == STA_ZEROPAGE_85){
        // max 3 cycles
        ZeroPageWriteIns();
    }else if(opcode == STA_ZEROPAGEX_95){
        // max 4 cycles
        ZeroPageXWriteIns();
    }else if(opcode == STA_ABSOLUTE_8D){
        AbsoluteWriteIns();
    }else if(opcode == STA_ABSOLUTEX_9D){
        // max 5 cycles
        AbsoluteXWriteIns();
    }else if(opcode == STA_ABSOLUTEY_99){
        // max 5 cycles
        AbsoluteYWriteIns();
    }else if(opcode == STA_INDIRECTX_81){
        // max 6 cycles
        IndirectXWriteIns();
    }else if(opcode == STA_INDIRECTY_91){
        IndirectYWriteIns();
    }
}

void CPU6502::Ins_STX()
{
    if(opcode == STX_ZEROPAGE_86){
        // max 3 cycles
        ZeroPageWriteIns();
    }else if(opcode == STX_ZEROPAGEY_96){
        // max 4 cycles
        ZeroPageYWriteIns();
    }else if(opcode == STX_ABSOLUTE_8E){
        AbsoluteWriteIns();
    }
}

void CPU6502::Ins_STY(){
    if(opcode == STY_ZEROPAGE_84){
        //max 3 cycles
        ZeroPageWriteIns();
    }else if(opcode == STY_ZEROPAGEX_94){
        //max 4 cycles
        ZeroPageXWriteIns();
    }else if(opcode == STY_ABSOLUTE_8C){
        //max 4 cycles
        AbsoluteWriteIns();
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
        ((emulator.ppu.ppustatus & PPU_FLAGS::PPU_VBLANK) == PPU_FLAGS::PPU_VBLANK) && stopNMIinstruction == false){
            nmi = true;
            stopNMIinstruction = true;
            opcode = BRK_00;
            cycleCounter++;
            // printf("interruput opcode: 0x%X\n",opcode);
        }else{
            if(cycleCounterMax != cycleCounter && cycleCounter != 0){
                printf("no es cycle perfer\n");
                while (true){}
            }

            opcode = emulator.Fetch(pc);
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
            case SLO_INDIRECTX_03:
                Ins_SLO();
                break;
            case NOP_04:
                Ins_NOP_ZEROPAGE();
                break;
            case ORA_ZEROPAGE_05: // 0x05
                Ins_ORA();
                break;
            case ASL_ZEROPAGE_06:
                Ins_ASL();
                break;
            case SLO_ZEROPAGE_07:
                Ins_SLO();
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
            case NOP_ABSOLUTE_0C:
                Ins_NOP_ABSOLUTE();
                break;
            case ORA_ABSOLUTE_0D:
                Ins_ORA();
                break;
            case ASL_ABSOLUTE_0E:
                Ins_ASL();
                break;
            case SLO_ABSOLUTE_0F:
                Ins_SLO();
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
            case SLO_INDIRECTY_13:
                Ins_SLO();
                break;
            case NOP_ZEROPAGEX_14:
                Ins_NOP_ZEROPAGEX();
                break;
            case ORA_ZEROPAGEX_15:
                Ins_ORA();
                break;
            case ASL_ZEROPAGEX_16:
                Ins_ASL();
                break;
            case SLO_ZEROPAGEX_17:
                Ins_SLO();
                break;
            case CLC_18:
                Ins_CLC();
                break;
            case ORA_ABSOLUTEY_19:
                Ins_ORA();
                break;
            case NOP_1A:
                Ins_NOP();
                break;
            case SLO_ABSOLUTEY_1B:
                Ins_SLO();
                break;
            case NOP_ABSOLUTEX_1C:
                Ins_NOP_ABSOLUTEX();
                break;
            case ORA_ABSOLUTEX_1D:
                Ins_ORA();
                break;
            case ASL_ABSOLUTEX_1E:
                Ins_ASL();
                break;
            case SLO_ABSOLUTEX_1F:
                Ins_SLO();
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
            case NOP_ZEROPAGEX_34:
                Ins_NOP_ZEROPAGEX();
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
            case NOP_3A:
                Ins_NOP();
                break;
            case NOP_ABSOLUTEX_3C:
                Ins_NOP_ABSOLUTEX();
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
            case NOP_44:
                Ins_NOP_ZEROPAGE();
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
            case NOP_ZEROPAGEX_54:
                Ins_NOP_ZEROPAGEX();
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
            case NOP_5A:
                Ins_NOP();
                break;
            case NOP_ABSOLUTEX_5C:
                Ins_NOP_ABSOLUTEX();
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
            case NOP_64:
                Ins_NOP_ZEROPAGE();
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
            case NOP_ZEROPAGEX_74:
                Ins_NOP_ZEROPAGEX();
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
            case NOP_7A:
                Ins_NOP();
                break;
            case ADC_ABSOLUTEX_7D:
                Ins_ADC();
                break;
            case NOP_ABSOLUTEX_7C:
                Ins_NOP_ABSOLUTEX();
                break;
            case ROR_ABSOLUTEX_7E:
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
            case NOP_IMMEDIATE_80:
                Ins_NOP_IMMEDIATE();
                break;
            case STA_INDIRECTX_81:
                Ins_STA();
                break;
            case NOP_IMMEDIATE_82:
                Ins_NOP_IMMEDIATE();
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
            case NOP_IMMEDIATE_89:
                Ins_NOP_IMMEDIATE();
                break;
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
            case NOP_IMMEDIATE_C2:
                Ins_NOP_IMMEDIATE();
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
            case NOP_ZEROPAGEX_D4:
                Ins_NOP_ZEROPAGEX();
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
            case NOP_DA:
                Ins_NOP();
                break;
            case NOP_ABSOLUTEX_DC:
                Ins_NOP_ABSOLUTEX();
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
            case NOP_IMMEDIATE_E2:
                Ins_NOP_IMMEDIATE();
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
            case NOP_ZEROPAGEX_F4:
                Ins_NOP_ZEROPAGEX();
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
            case NOP_FA:
                Ins_NOP();
                break;
            case NOP_ABSOLUTEX_FC:
                Ins_NOP_ABSOLUTEX();
                break;
            case SBC_ABSOLUTEX_FD:
                Ins_SBC();
                break;
            case INC_ABSOLUTEX_FE:
                Ins_INC();
                break;
            default:
                printf("Unknown opcode [0xF0]: 0x%X\n", opcode);
                break;
            }
            break;
        default:
            printf("Unknown opcode 0x%X\n", opcode);
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


void Emulator::Cycle(){
    cpu.EmulateCycle();
    ppu.EmulateCycle();
    ppu.EmulateCycle();
    ppu.EmulateCycle();
    if(!controller.controllerStrobe){
        if (controller.shiftCounter1 > 0){
            controller.shiftCounter1--;
            if (controller.shiftCounter1 == 0)
            {
                controller.controllerShiftRegister1 <<= 1;
                controller.controllerShiftRegister1 |= 1;
            }
        }
    }else{
        controller.shiftCounter1 = 0;
    }
    // 60hz or 48hz
    emulator.apu.EmulateCycle();
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
    // emulator.Initialize("Super_mario_brothers.nes");
    // emulator.apu.Initialize();
    
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

        // if(rayWindow_Debugger == false){
        //     rayWindow_Debugger = true;

        //     data->nempty.wait();
        //     data->mutex.wait();

        //     printf("main\n");
        //     OpenWindow();
        //     data->mutex.post();
        //     data->nstored.post();
        // }

        // if(data->commandMain == 1){

        //     data->nempty.wait();
        //     data->mutex.wait();
        //     data->debugger.pc = emulator.cpu.pc;
        //     data->debugger.x = emulator.cpu.registerX;
        //     data->debugger.y = emulator.cpu.registerY;
        //     data->debugger.a = emulator.cpu.accumulator;
        //     data->debugger.status = emulator.cpu.statusFlag;
        //     data->debugger.ppuDot = emulator.ppu.scanlineDot;
        //     data->debugger.ppuScanline = emulator.ppu.scanlineRow;

        //     data->commandMain = 0;
        //     data->mutex.post();
        //     data->nstored.post();
        // }else if(data->commandMain == 2){

        //     data->nempty.wait();
        //     data->mutex.wait();

        //     data->debugger.pc = emulator.cpu.pc;
        //     data->debugger.x = emulator.cpu.registerX;
        //     data->debugger.y = emulator.cpu.registerY;
        //     data->debugger.a = emulator.cpu.accumulator;
        //     data->debugger.status = emulator.cpu.statusFlag;
        //     data->debugger.ppuDot = emulator.ppu.scanlineDot;
        //     data->debugger.ppuScanline = emulator.ppu.scanlineRow;
        //     data->mutex.post();
        //     data->nstored.post();

        //     printf("termine\n");
        //     while (data->commandMain == 2){}
        //     printf("sali\n");
        // } else if(data->commandMain == 3 || data->commandMain == 4){

        //     if (data->debugger.pc != emulator.cpu.pc){
        //         data->nempty.wait();
        //         data->mutex.wait();

        //         data->debugger.pc = emulator.cpu.pc;
        //         data->debugger.x = emulator.cpu.registerX;
        //         data->debugger.y = emulator.cpu.registerY;
        //         data->debugger.a = emulator.cpu.accumulator;
        //         data->debugger.status = emulator.cpu.statusFlag;
        //         data->debugger.ppuDot = emulator.ppu.scanlineDot;
        //         data->debugger.ppuScanline = emulator.ppu.scanlineRow;
        //         data->mutex.post();
        //         data->nstored.post();

        //         printf("termine\n");
        //         int stopValue = data->commandMain;
        //         while (data->commandMain == stopValue){}
        //         printf("sali\n");
        //     }
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
    emulator.apu.Destroy();
    CloseMainWindow();
    CloseWindow();          // Close window and OpenGL context
    if(rayWindow_Debugger == true){
        // closePipeClient();
        closeDebuggerWindow();
    }
    //--------------------------------------------------------------------------------------

    return 0;
}
