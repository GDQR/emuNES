#pragma once
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

struct DebuggerData
{
   unsigned short pc;
   unsigned char x;
   unsigned char y;
   unsigned char a;
   unsigned short ppuDot;
   unsigned short ppuScanline;
};


struct shared_memory_buffer
{
   // enum { NumItems = 10 };

   shared_memory_buffer()
      : mutex(1), nempty(1/*NumItems*/), nstored(0)
   {}

   //Semaphores to protect and synchronize access
   boost::interprocess::interprocess_semaphore
      mutex, nempty, nstored;

   //Items to fill
   // int items[NumItems];
   DebuggerData debugger;
   int commandMain;
   int commandDebugger;
};