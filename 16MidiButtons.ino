// ----------------------------------------------------------------------------
//                      16 Buttons Midi Controler
// ----------------------------------------------------------------------------
//
// V 0.03 test 74HC165 Lib - ok
// V 0.02 test 74HC595 Lib - ok
//
// (C) 2023 under GNU Licence  - F.Lafforgue
// ----------------------------------------------------------------------------


// D0       I2  Rx 
// D1       I3  Tx
// D2       I1  SDA       SSD1306
// D3~      I0  SCL       SSD1306
// D4   A6                RotB
// D5~                    DIN165
// D6~  A7                LATCH165
// D7       I6            RotA
// D8   A8                DOUT595
// D9~  A9                LATCH595
// D10~ A10               CLK595 / CLK165
// D14          MISO      RotBTN
// D15          SCLK
// D16          MOSI
// D18  A0                Pot0 
// D19  A1                Pot1
// D20  A2                Pot2
// D21  A3                Pot3

#define CLK595            10
#define LATCH595          9
#define DOUT595           8

#define CLK165            10
#define LATCH165          6
#define DIN165            5

#define SDA               2
#define SCL               3

#define ROTA              7
#define ROTB              4
#define ROTBTN            14

// ----------------------------------------------------------------------------

#include "Lib_74HC595.h"
#include "Lib_74HC165.h"

// ----------------------------------------------------------------------------

void setup() {
  L595Init();
  L165Init();
}

// ----------------------------------------------------------------------------

unsigned int BtnStatus =1;
unsigned int oBtnStatus=0;
  
void loop() {

  BtnStatus=L165ReadOneWord( );
  if ( BtnStatus != oBtnStatus ) {
    L595SendOneWord(BtnStatus);
    oBtnStatus=BtnStatus;
  }

}
