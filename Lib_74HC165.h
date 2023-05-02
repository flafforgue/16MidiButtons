// ----------------------------------------------------------------------------
//                       74 HC 165 Library
// ----------------------------------------------------------------------------
//
// V 0.01 
//
// (C) 2023 under GNU Licence  - F.Lafforgue
// ----------------------------------------------------------------------------

#ifndef SN74HC165_H

#define SN74HC165_H
#include "Arduino.h"

#ifndef CLK165
  #define CLK165        10
  #define LATCH165       6
  #define DIN165         5
#endif

#define MSBFIRST165
//#define LSBFIRST165


void L165Init() {
  pinMode(CLK165       , OUTPUT );
  pinMode(LATCH165     , OUTPUT );	
  pinMode(DIN165       , INPUT );

  digitalWrite(CLK165  , LOW );
  digitalWrite(LATCH165, LOW );  
}

byte L165ReadOneByte() {
  byte temp=0;
  
  digitalWrite(LATCH165,HIGH);
  for ( byte i=0; i<8; i++ ) {
	// MSBFIRST
	temp = temp | ( digitalRead(DIN165) << ( 7-i) );
    // LSBFIRST
    // temp = temp | ( digitalRead(DIN165) << i );

	digitalWrite(CLK165  , HIGH ); // Pulse
	digitalWrite(CLK165  , LOW );
  }
  digitalWrite(LATCH165,LOW);
  return temp;
}

unsigned int L165ReadOneWord( ) {
  byte temp0=0;
  byte temp1=0;
  unsigned int temp=0;
  
  digitalWrite(LATCH165,HIGH);
  for ( byte i=0; i<8; i++ ) {
    // MSBFIRST
    temp0 = temp0 | ( digitalRead(DIN165) << ( 7-i) );
    digitalWrite(CLK165  , HIGH ); // Pulse
    digitalWrite(CLK165  , LOW );
  }
  for ( byte i=0; i<8; i++ ) {
    // MSBFIRST
    temp1 = temp1 | ( digitalRead(DIN165) << ( 7-i) );
    digitalWrite(CLK165  , HIGH ); // Pulse
    digitalWrite(CLK165  , LOW );
  }
  
  digitalWrite(LATCH165,LOW);
  return temp0 | ( temp1 << 8 );
}

#endif
