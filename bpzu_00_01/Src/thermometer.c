#include "thermometer.h"

//   systick 5 uSec!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   // Function reset
   inline void reset(uint16_t GPIO_Pin)
   {
      ////////////////// RESET /////////////////////
      // wait 500 uSec
      HAL_Delay(100);
      // pin low
      GPIOA->BRR=GPIO_Pin;
      // wait 500 uSec
      HAL_Delay(100);
      // pin high
      GPIOA->BSRR=GPIO_Pin;
      // wait 500 uSec
      HAL_Delay(100);
      //********************************************
      
   }//end reset
   
   // Function write0
   inline void write0(uint16_t GPIO_Pin)
   {
      ///////////////// WRITE 0 ///////////////////
      // wait 10 uSec
      HAL_Delay(2);
      // pin low
      GPIOA->BRR=GPIO_Pin;
      // wait 60 uSec
      HAL_Delay(12);
      // pin high
      GPIOA->BSRR=GPIO_Pin;
      //********************************************
      
   }//end write0
   
   // Function write1
   inline void write1(uint16_t GPIO_Pin)
   {
      ///////////////// WRITE 1 ///////////////////
      // wait 10 uSec
      HAL_Delay(2);
      // pin low
      GPIOA->BRR=GPIO_Pin;
      // wait 10 uSec
      HAL_Delay(2);
      // pin high
      GPIOA->BSRR=GPIO_Pin;
      // wait 50 uSec
      HAL_Delay(10);
      //********************************************
      
   }//end write1
   
   // Function writeByte
   inline void writeByte(uint8_t data, uint16_t GPIO_Pin)
   {
      uint8_t localData = data;
      ///////////////// WRITE BYTE ///////////////////
      int i;
      for(i=0; i<8; i++)
      {
         if((localData & (uint8_t)0x01) == (uint8_t)0x01)
            write1(GPIO_Pin);
         else
            write0(GPIO_Pin);
         localData >>=1;
      }
      //********************************************
      
   }//end writeByte
   
   // Function read1bit
   inline uint16_t read1bit(uint16_t GPIO_Pin)
   {
      uint16_t LSB = 0x0000;
      
      ///////////////// READ 1 bit ///////////////////
      // wait 10 uSec
      HAL_Delay(2);
      // pin low
      GPIOA->BRR=GPIO_Pin;
      // wait 5 uSec
      HAL_Delay(1);
      // pin high
      GPIOA->BSRR=GPIO_Pin;
      // wait 5 uSec
      HAL_Delay(1);
      // read pin condition
      if((GPIOA->IDR & GPIO_Pin) != (uint32_t)0)
         LSB = 0x0001;
      // wait 60 uSec
      HAL_Delay(12);
      //********************************************
      
      return LSB;
      
   }//end read1bit
   
   // Function readByte
   inline uint8_t readByte(uint16_t GPIO_Pin)
   {
      uint8_t data = 0;
      
      ///////////////// READ 1 byte ///////////////////
      int i;
      for(i=0; i<8; i++)
      {
         data += (read1bit(GPIO_Pin)) << i;
      }
      //********************************************
      
      return data;
      
   }//end readByte
   
   // Function readCode
   inline void readCode(uint8_t *code, uint16_t GPIO_Pin)
   {
	  int i;
      for(i=0; i<8; i++)
      {
         code[i] = readByte(GPIO_Pin);
      }
      
   }//end readCode
   
   // Function writeCode
   inline void writeCode(uint8_t *code, uint16_t GPIO_Pin)
   {
	  int i;
      for(i=0; i<8; i++)
      {
         writeByte(code[i], GPIO_Pin);
      }
      
   }//end writeCode
   
   // Function read2bytes
   inline uint16_t read2bytes(uint16_t GPIO_Pin)
   {
      uint16_t data = 0;
      
      ///////////////// READ 2 bytes ///////////////////
      int i;
      for(i=0; i<16; i++)
      {
         data += (read1bit(GPIO_Pin)) << i;
      }
      //********************************************
      
      return data;
      
   }//end read2bytes
   
   // Function skipRom
   inline void skipRom(uint16_t GPIO_Pin)
   {
      writeByte(0xcc, GPIO_Pin);
      
   }//end skipRom
   
   inline void readRom(uint16_t GPIO_Pin)
   {
      writeByte(0x33, GPIO_Pin);
      
   }//end readRom
   
   inline void matchRom(uint16_t GPIO_Pin)
   {
      writeByte(0x55, GPIO_Pin);
      
   }//end readRom

   // Function convertT
   inline void convertT(uint16_t GPIO_Pin)
   {
      writeByte(0x44, GPIO_Pin);
      
   }//end convertT
   
   // Function readT
   inline uint16_t readT(uint16_t GPIO_Pin)
   {
      writeByte(0xbe, GPIO_Pin);
      return read2bytes(GPIO_Pin);
      
   }//end readT
   
   // Function configure9bit
   inline void configure9bit(uint16_t GPIO_Pin)
   {
      writeByte(0x4e, GPIO_Pin);  // write scratchpad
      writeByte(0x00, GPIO_Pin);  // alarm
      writeByte(0x00, GPIO_Pin);  // alarm
      writeByte(0x10, GPIO_Pin);  // configuration
      
   }//end configure9bit

   int16_t readThemperature()
   {
      float themperature;
      uint16_t GPIO_Pin = GPIO_PIN_4;
      
      reset(GPIO_Pin);
      skipRom(GPIO_Pin);
      //*
      convertT(GPIO_Pin);
      ///////////// wait 1 sec ///////////////////
      HAL_Delay(200000);
      //******************************************
      //*/
      reset(GPIO_Pin);
      //matchRom();
      skipRom(GPIO_Pin);
      //writeCode(code);
      uint16_t tAux = readT(GPIO_Pin);
      reset(GPIO_Pin);
      
      themperature = (float)((int16_t)tAux)/16.0;
      
      
      return (int16_t)tAux;
      
   }//end readThemperature
