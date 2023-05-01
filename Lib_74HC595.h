// ----------------------------------------------------------------------------
//                       74 HC 595 Library
// ----------------------------------------------------------------------------
//
// V 0.02 
//
// (C) 2023 under GNU Licence  - F.Lafforgue
// ----------------------------------------------------------------------------

#ifndef SN74HC595_H

#define SN74HC595_H
#include "Arduino.h"

#ifndef CLK595
  #define CLK595        10
  #define LATCH595       9
  #define DOUT595        8
#endif

void L595Init() {
  pinMode(CLK595       , OUTPUT );
  pinMode(DOUT595      , OUTPUT );
  pinMode(LATCH595     , OUTPUT );	
}

void L595SendOneByte(byte Data) {
   digitalWrite(LATCH595, LOW);
   shiftOut(DOUT595, CLK595, MSBFIRST, Data);
   digitalWrite(LATCH595, HIGH);
}

void L595SendOneWord(unsigned int  Data) {
   digitalWrite(LATCH595, LOW);
   shiftOut(DOUT595, CLK595, MSBFIRST, Data >> 8  );
   shiftOut(DOUT595, CLK595, MSBFIRST, Data);
   digitalWrite(LATCH595, HIGH);
}

#endif
