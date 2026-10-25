#include "cpu.hpp"
#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
static int waveFrequency = 440;
static int newWaveFrequency = 440;
static int waveIndex = 0;
static void SineCallback(void *framesOut, unsigned int frameCount)
{
    int wavelength = SAMPLE_RATE/waveFrequency;

    // Synthesize the sine wave
    for (int i = 0; i < frameCount; i++)
    {
        ((float *)framesOut)[i] = sin(2*PI*waveIndex/wavelength);

        waveIndex++;

        if (waveIndex >= wavelength)
        {
            waveFrequency = newWaveFrequency;
            waveIndex = 0;
        }
    }

}

static void SquareCallback(void *framesOut, unsigned int frameCount)
{
    int wavelength = SAMPLE_RATE/waveFrequency;

    float c;
    float a;
    float b;
    // Synthesize the square wave
    for (int i = 0; i < frameCount; i++)
    {

        // c = i * 
        // a = sin(c) / i;
        // ((float *)framesOut)[i] = (waveIndex < wavelength/2)? 1 : -1;
        // waveIndex++;

        // if (waveIndex >= wavelength)
        // {
        //     waveFrequency = newWaveFrequency;
        //     waveIndex = 0;
        // }
    }

}

static double globalTime = 0;
unsigned int NTSCFrequency = 1789773;
static void Pulse1(void *framesOut, unsigned int frameCount){
    // float f = (SAMPLE_RATE/44100) / (16 * (emulator.apu.timerPulse1 + 1));

    double phase = emulator.apu.frequencyPulse1 * 2.0 * PI;
    double c = 0;
    double a = 0;
    double b = 0;
    // Synthesize the sine wave
    int frame = 0;
    // double j =0;

    auto aproxSin = [](float t){
      float j= t *0.15915;
      j = j - (int)j;
      return 20.785 * j * (j-0.5) * (j -1.0f);
    };
    // for (int pas = 0; pas < frameCount; pas++ /*(float)frameCount*/){
    //   j += (double) (0.3333333333/1789773);
    for (double j = 1.0f; j < 2.0f; j += 1.0f/(float)frameCount){
    // for (float j = 0.3333333333/1789773; j < 2.0f; j += 1.0f/128.0f /*(float)frameCount*/){
      for (double i = 1; i < emulator.apu.harmonics; i++)
      {
        // c = i * emulator.apu.fFrequency * 2 * PI * emulator.apu.t;
        // c = i * emulator.apu.frequency * 2 * PI * j;
        c = i * emulator.apu.frequency * 2 * PI * j;
        // a += -sin(c)/i ;
        // b += -sin(c-phase*i)/i;
        a += -aproxSin(c)/i ;
        b += -aproxSin(c-phase*i)/i;
      }
    // for (float i = 1; i < frameCount; i++){
      ((float *)framesOut)[frame] = ((2.0f * emulator.apu.amplitud / PI) * (a-b));
      frame++;
    }
}
static void SampleSquareWave(void* framesOut, unsigned int frameCount){
    float phase = emulator.apu.fDutyCycle * 2.0 * PI;
    float c = 0;
    float a = 0;
    float b = 0;
    float f = emulator.apu.fFrequency /440;
    // Synthesize the sine wave
    int frame = 0;
    for (float j = 1.0f; j < 2.0f; j += 1.0f/440.0f /*(float)frameCount*/){
      for (float i = 1; i < emulator.apu.fHarmonics; i++)
      {
        // c = i * emulator.apu.fFrequency * 2 * PI * emulator.apu.t;
        c = i * f * 2 * PI * j;
        a += sin(c)/i ;
        b += sin(c-phase*i)/i;
      }
      ((float *)framesOut)[frame] = ((2.0f / PI) * (a-b)) * 0.5f;
      frame++;
    }
    // printf("final frame: %d\n",frame);
    // for(int i= 0; i<frameCount; i++){
      
    //   ((float *)framesOut)[i] = (2.0f / PI) * (a-b);
    // }
}

AudioStream stream;

void APU::Initialize(){
 
  fDutyCycle = 0.5f;
  fHarmonics = 20;
  fFrequency = 440.0f; 
  InitAudioDevice();
  
  static AudioCallback waveCallbacks[] = {SineCallback, Pulse1, SampleSquareWave};

  // Set the number of samples the stream will keep in memory at a time to BUFFER_SIZE
  SetAudioStreamBufferSizeDefault(4096);
  
  // Init raw audio stream (sample rate: 44100, sample size: 32bit-float, channels: 1-mono)
  stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
  PlayAudioStream(stream);

  // Configure it so that waveCallbacks[waveType] is called whenever stream is out of samples
  // WaveType waveType = SINE;
  // SetAudioStreamCallback(stream, waveCallbacks[0]);
  SetAudioStreamCallback(stream, waveCallbacks[1]);
}

void APU::EmulateCycle(){
  // Every 2 CPU cycles, 1 APU
  if(CPUclock == 1){
    if(controller.controllerStrobe == true){
        if(!controller.controllerStopRead ){
            controller.controllerStopRead = true;
        
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
    }else{
        controller.controllerStopRead = false;
    }
    
    // timerPulse1--;
    // unsigned char timer = pulse1[3] & 0x7;
    // timer--;
    // pulse1[3] = ((pulse1[3] & 0xF8) | timer);
    globalTime += 0.3333333333/1789773; 

    
    if(clock == 3728){
      // if Sweep is enable
      if((pulse1[1] & 0x80) == 1){
        // if Divider's period is zero
        if((pulse1[1] & 0x70) == 0){
          // if Shift count is not zero
          if((pulse1[1] & 7) != 0){

          }
        }
      }
    }else if(clock == 7456){
      
      // Length counter halt is clear
      // if((pulse1[0] & 0x20) == 0){
      //   unsigned char length = (pulse1[3] & 0xF8);
      //   if(length > 0){
      //     length--;
      //     pulse1[3] = (length & 0xF8) | (pulse1[3] & 7);
      //   }
      // }
    }else if(clock == 11185){
    }else if(clock == 14914){
      // Length counter halt is clear
      // if((pulse1[0] & 0x20) == 0){
      //   unsigned char length = (pulse1[3] & 0xF8);
      //   if(length > 0){
      //     length--;
      //     pulse1[3] = (length & 0xF8) | (pulse1[3] & 7);
      //   }
      // }
    }else if(clock == 18640){
    }

    frequency = 1789733.0 / (16 * (double) (emulator.apu.timerPulse1 + 1));
    
    clock++;
    
    if((frameCounter & 0x80) == 0){
      if(clock == 14915){
        clock = 0;
      } 
    }else{
      if(clock == 18641){
        clock = 0;
      }
    }
  }else if(CPUclock == 2){
    CPUclock = 0;
  }
  CPUclock++;
}

void APU::Store(unsigned char value, unsigned short address){
  switch (address){
  case 0x4000:
      pulse1[0] = value;
      
      // Duty
      switch ((pulse1[0] & 0xC0)){
      case 0:
        sequencePulse1 = 0x1;
        frequencyPulse1 = 0.125f;
        break;
      case 1:
        sequencePulse1 = 0x3;
        frequencyPulse1 = 0.250f;
        break;
      case 2:
        sequencePulse1 = 0x3;
        frequencyPulse1 = 0.500f;
        break;
      case 3:
        sequencePulse1 = 0xFC;
        frequencyPulse1 = 0.750f;
        break;
      default:
        break;
      }
      
      printf("pulse[0]: 0x%X\n",value);
    break;
  case 0x4001:
      pulse1[1] = value;
    break;
  case 0x4002:
      pulse1[2] = value;
    break;
  case 0x4003:
      pulse1[3] = value;
      timerPulse1 = ((pulse1[3] & 7) << 8) | pulse1[2];  
    break;
  case 0x4010:
      DMC[0] = value;
      if((DMC[0] & 0x80) == 0x80){
        emulator.cpu.irqLine = true;
      }else{
        emulator.cpu.irqLine = false;
      }
    break;
  case 0x4015:
    status = value & 0x1F;
    emulator.cpu.irqLine = false;
    break;
  case 0x4017:
    frameCounter = value & 0xC0;
    if((frameCounter & 0x40) != 0){
      emulator.cpu.irqLine = false;
    }else{
      emulator.cpu.irqLine = true;
    }
  break;
  default:
    break;
  }
}

void APU::Destroy(){
    UnloadAudioStream(stream);  // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
}