// ----------------------------------------------------------------------------
//                      16 Buttons Midi Controler
// ----------------------------------------------------------------------------
//
// V 0.12 add pots and visualization during live 
// V 0.11 Setup with Control switch mode
//                   Clrl "Normal" Mode send value when pressing
//                   CtrS "Switch" Mode first press send High value and switch light On 
//                                      second send Low value and Switch light Off
// V 0.10 setup
// V 0.06 Test menus       - ok
// V 0.05 test Encoder     - ok
// V 0.04 test SSD1306     - ok
// V 0.03 test 74HC165 Lib - ok
// V 0.02 test 74HC595 Lib - ok
//
// (C) 2023 under GNU Licence  - F.Lafforgue
// ----------------------------------------------------------------------------

// D0       I2  Rx 
// D1       I3  Tx        Midi out
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

#define CLK595             10
#define LATCH595            9
#define DOUT595             8

#define CLK165             10
#define LATCH165            6
#define DIN165              5

#define SDA                 2
#define SCL                 3

#define ROT_A               7
#define ROT_B               4
#define BTNEncoder         14

#define Pot0               A3
#define Pot1               A2
#define Pot2               A1
#define Pot3               A0

// ----------------------------------------------------------------------------

#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLedWidth         128
#define OLedHight          64
#define OLedReset          -1
#define OLedAdr          0x3C

Adafruit_SSD1306 OLed(OLedWidth, OLedHight, &Wire, OLedReset);

#include "Lib_74HC595.h"
#include "Lib_74HC165.h"
#include "BitsOpperations.h"

// ============================================================================
//                                     Midi
// ============================================================================

#define MSGMask        B11110000
#define CHNMask        B00001111

#define MidiModeNote   B10000000
#define MidiCtrlChange B10110000
#define MidiProgChange B11000000

                        // 16 Butons + 4 Pots
byte ChnMessage[20] = { B10001010, B10001010, B10001010, B10001010,   B10001010, B10001010, B10001010, B10111010, 
                        B10001010, B10001010, B10001010, B10001010,   B10110001, B10110001, B10110001, B10110001,                     
                        B10110000, B10110000, B10110000, B10110000  };
                        
byte ChnData1[20]   = {  41,  43,  45,  47,     48,  50,  49,  51, 
                         36,  38,  44,  46,     01,  02,  03,  04,
                          7,   1,  12,  13 }; 

byte ChnData2[20]   = { 127, 127, 127, 127 ,   127, 127, 127, 127 , 
                        127, 127, 127, 127 ,   127, 127, 127, 127 ,
                          0,   0,   0,   0 };
                        
byte ChnData2f[20]  = {   0,   0,   0,   0 ,     0,   0,   0,   0 , 
                          0,   0,   0,   0 ,     0,   0,   0,   0 ,
                          0,   0,   0,   0 };
                        
unsigned int Lights =      0; // Status of lights
unsigned int Toggle = 0x8888; // Define which button is in toggle mode

// ----------------------------------------------------------------------------
// Note On     : 1001 cccc  -  0nnn nnnn  -  0vvv vvvv  : c channel 0-16 : n Note 0-127       : v Velocity 0-127
// Note Off    : 1000 cccc  -  0nnn nnnn  -  0vvv vvvv  : c channel 0-16 : n Note 0-127       : v Velocity 0-127
// Prg Change  : 1100 cccc  -  0iii iiii  -  0... ....  : c channel 0-16 : i instrument 0-127 : . unused
// Ctrl Change : 1011 cccc  -  0nnn nnnn  -  0vvv vvvv  : c channel 0-16 : n Controler  0-127 : v value 0-127

void SendMidiCmd(byte Msg,byte Data1,byte Data2 ) {
}

void SendButtonPress(byte Chn ) {
  byte Msg;

  Msg=ChnMessage[Chn];
  SendMidiCmd(Msg, ChnData1[Chn], ChnData2[Chn] ); 
}

void SendButtonRelease(byte Chn ) {
  byte Msg;

  Msg=ChnMessage[Chn];
  if (( Msg & MSGMask ) == MidiModeNote ) {
     Msg = Msg & B11101111; 
  }
  SendMidiCmd(Msg, ChnData1[Chn], ChnData2[Chn] ); 
}

// ----------------------------------------------------------------------------
//                         Rotary Encoder , button & Pots
// ----------------------------------------------------------------------------

#define BTN_NONE           0
#define BTN_ENC            1
#define BTN_ENC_Long       2

#define BTN_LONGDELAY   1000

byte keydown  =    BTN_NONE;
byte key      =    BTN_NONE;
unsigned long      BTNTime;

int  encodermov     = 0;
byte LastBtnPressed = 0;

//#define ENCSOFTFILTER
#define ENCSOFTFILTERDELAY 25

#ifdef ENCSOFTFILTER
unsigned long OEncTime = 0; 

void doEncoder() {
  unsigned long Nt = millis();
  if ( Nt - OEncTime > ENCSOFTFILTERDELAY ) {
    if (digitalRead(ROT_A) == digitalRead(ROT_B)) {
      encodermov = +1;
    } else {
      encodermov = -1;
    } 
    OEncTime=Nt;   
  }
}
#else
void doEncoder() {
  if (digitalRead(ROT_A) == digitalRead(ROT_B)) {
    encodermov = +1;
  } else {
    encodermov = -1;
  } 
}
#endif

void ClearEncoder() {
  cli();
  encodermov=0;
  sei(); 
}

// ----------------------------------------------------------------------------

#define ROUNDPOTS 0  //
#define DIVPOTS   8  // Analogread 0-1023 - values 0-127 

void ReadPots() {
  ChnData2[16]=( analogRead(Pot0) + ROUNDPOTS ) / DIVPOTS;
  ChnData2[17]=( analogRead(Pot1) + ROUNDPOTS ) / DIVPOTS;
  ChnData2[18]=( analogRead(Pot2) + ROUNDPOTS ) / DIVPOTS;
  ChnData2[19]=( analogRead(Pot3) + ROUNDPOTS ) / DIVPOTS;

  for (byte i=16; i<20;i++) {
    if ( ChnData2f[i] != ChnData2[i] ) { 
      ChnData2f[i]=ChnData2[i];
      LastBtnPressed=i;
    }
  }
}

// ----------------------------------------------------------------------------

void ReadBtnState() {
  unsigned long NTime; 
  NTime=millis();
  
  if ( keydown == BTN_NONE ) { // no key waiting 
    if ( digitalRead (BTNEncoder )==LOW ) { BTNTime=NTime;  keydown=BTN_ENC;    }
  } else {                        // key allready down
    if ( NTime - BTNTime > 10 ) { // avoid rebounds
        switch (keydown) {
           case BTN_ENC:
                 if ( digitalRead (BTNEncoder)==HIGH ) { // keypress on release ;)   
                   if ( NTime - BTNTime >= BTN_LONGDELAY )  key = BTN_ENC_Long;                
                   else                                     key = BTN_ENC;
                   keydown=BTN_NONE;
                 }
                 break;
       }       
    }
  }
}

bool keypressed() {
  return ( key != BTN_NONE );
}

byte readkey() {
  byte tmp = key;
  key=BTN_NONE;
  return( tmp);
}

void MyDelay(unsigned int del) {
  unsigned long ot = millis();
  while ( millis()-ot < del ) {
    ReadBtnState();
  } 
}

// ============================================================================
//                           M E N U    &    D I S P L A Y
// ============================================================================

#define L1                0
#define L2               16
#define L3               32
#define L4               48
#define Lmargin           4

void TitleMenu(String str) {
  OLed.clearDisplay();
  OLed.setTextSize(2);
  OLed.setTextColor(SSD1306_WHITE);
  OLed.setCursor(Lmargin, L1);
  OLed.println(str); 
}

void OLedprint2 ( int value ) {
   if (( value ) < 10 ) { OLed.print(F(" ")); }    // finaly not using sprintf , use more size
   OLed.print(value);
}

void OLedprint4 ( int value ) {
   if (( value ) < 1000 ) { OLed.print(F(" ")); }  // finaly not using sprintf , use more size
   if (( value ) <  100 ) { OLed.print(F(" ")); }   
   if (( value ) <   10 ) { OLed.print(F(" ")); }
   OLed.print(value);
}

// ============================================================================
//                                    Mode Live
// ============================================================================

unsigned int BtnStatus  = 1;
unsigned int oBtnStatus = 0;

void DoLive() {
  boolean       LetRunning = true;
  unsigned long omillis    = 0;  
  unsigned long amillis    = 0;  
  unsigned long zmillis    = 0; 
  unsigned int  BtnChange;

  while ( LetRunning ) {
    ReadBtnState();
    if ( readkey() == BTN_ENC ) {  // if Select Pressed exit
      LetRunning=false;
    }

    if ( millis() - amillis > 5 ) {
      BtnStatus=L165ReadOneWord( );
      amillis=millis();
    }

    if ( BtnStatus != oBtnStatus ) {
      BtnChange= oBtnStatus ^ BtnStatus; // get button changed
      if ( BtnChange > 0 ) {
        for (byte i=0;i<16;i++) {
          unsigned int tmp = BtnChange & ( 1 << i ) ; // Btn change status
          unsigned int prs = BtnStatus & ( 1 << i ) ; // Btn pressed
          unsigned int tgl = Toggle    & ( 1 << i ) ; // Toggle mode
          if ( tmp != 0 ) {
            if ( prs > 0 ) {                  // Btn Pressed
              LastBtnPressed=i;
              SendButtonPress(i);
              if ( tgl > 0 ) {
                Lights = Lights ^ prs;        // Toggle mode , so first press light , second switch off  
              } else {
                Lights = Lights | ( 1 << i ); // Not in toggle mode press switch on light
              }
            } else {                           // Bnt Released 
              SendButtonRelease(i);
              if ( tgl == 0 ) {
                Lights = Lights & ~( 1 << i ); // Not in toggle mode release switch off light
              }
            }
          }
        }
      }  
      L595SendOneWord(Lights);
      oBtnStatus=BtnStatus;
    }
   
     if ( millis() - zmillis > 250 ) {     // Refresh pots every 250 us
      zmillis = millis();
      ChnData2[16]=( analogRead(Pot0) + ROUNDPOTS ) / DIVPOTS;
      ChnData2[17]=( analogRead(Pot1) + ROUNDPOTS ) / DIVPOTS;
      ChnData2[18]=( analogRead(Pot2) + ROUNDPOTS ) / DIVPOTS;
      ChnData2[19]=( analogRead(Pot3) + ROUNDPOTS ) / DIVPOTS;
      for (byte i=16; i<20;i++) {
        if ( ChnData2f[i] != ChnData2[i] ) { 
          ChnData2f[i]=ChnData2[i];
          SendButtonPress(i);
          LastBtnPressed=i;
        }
      }      
    }
       
    if ( millis() - omillis > 300 ) {    // Refresh Display every 300 us
      TitleMenu(F("Live"));
      OLed.setCursor( 4, L2);  OLed.print(F("Chanl")); 
      OLed.setCursor(68, L2);  OLedprint4(ChnMessage[LastBtnPressed] & CHNMask);
      OLed.setCursor(68, L3);  OLedprint4(ChnData1[LastBtnPressed]);
      OLed.setCursor(68, L4);  OLedprint4(ChnData2[LastBtnPressed]);
      
      switch ( ChnMessage[LastBtnPressed] & MSGMask ) {
        case MidiModeNote  :
             OLed.setCursor( 4, L3);  OLed.print(F("Note"));            
             OLed.setCursor( 4, L4);  OLed.print(F("Vel"));             
             break;
        case MidiCtrlChange:            
             OLed.setCursor( 4, L3);  OLed.print(F("Ctrl"));          
             OLed.setCursor( 4, L4);  OLed.print(F("Val"));                         
             break;  
        case MidiProgChange:
             OLed.setCursor( 4, L3);  OLed.print(F("Prog"));          
             break;
      }

      OLed.display();
      omillis = millis();
    }
  }

  ClearEncoder();
}

// ----------------------------------------------------------------------------
//                                   S E T U P
// ----------------------------------------------------------------------------

void DoConfig() {
  boolean       LetRunning = true;
  unsigned long omillis    = 0;  
  byte          Edt        = 0;
  unsigned int  msg;
  
  while ( LetRunning ) {
    ReadBtnState();
    switch ( readkey() ) {
      case BTN_ENC:  
         Edt++;
         if ( Edt > 3) { Edt=0; }
         break; 
      case BTN_ENC_Long:   
         LetRunning=false;
         break;     
    }

    ReadPots();
    if ( LastBtnPressed>15) {
      L595SendOneWord(0);
    }
    BtnStatus=L165ReadOneWord( );
    if ( BtnStatus != oBtnStatus ) {
      oBtnStatus=BtnStatus;
      for (byte i=0;i<16;i++)  {
        if ( ( BtnStatus >> i ) & 1 == 1 ) {
          LastBtnPressed=i; 
          L595SendOneWord(BtnStatus);
        }
      }
    }
  
    if (encodermov != 0 ) {
      switch ( Edt ) {
         case 0:  // Channel number
            ChnMessage[LastBtnPressed] = ( ChnMessage[LastBtnPressed] & MSGMask ) | ((( ChnMessage[LastBtnPressed] & CHNMask ) + encodermov) & CHNMask );
            break;    
         case 1:  // change Mode
            if ( BtnStatus < 16 ) {
              msg = ( ChnMessage[LastBtnPressed] & MSGMask );
              if ( msg == MidiModeNote ) { msg = MidiCtrlChange;  }
              else if ( msg == MidiProgChange ) { msg = MidiModeNote;  }
              else {
                if ( (unsigned int)(Toggle & ( (unsigned int)( 1 << LastBtnPressed ) )) > 0 ) {
                  msg = MidiProgChange;
                  Toggle = Toggle & ( ~ (unsigned int)( 1 << LastBtnPressed ) ); 
                } else {
                  Toggle = Toggle | ( (unsigned int)( 1 << LastBtnPressed ) );
                  msg = MidiCtrlChange;
                }
              }
              ChnMessage[LastBtnPressed] = msg | ( ChnMessage[LastBtnPressed] & CHNMask );
            }
            break;               
         case 2:  // change channel
            ChnData1[LastBtnPressed] = ( ChnData1[LastBtnPressed] + encodermov ) & B01111111;
            break; 
         case 3:  // change data2
            if ( BtnStatus < 16 ) {
              ChnData2[LastBtnPressed] = ( ChnData2[LastBtnPressed] + encodermov ) & B01111111;
            }
            break;                
         case 4:  // change data2f ( min value )
            ChnData2f[LastBtnPressed] = ( ChnData2f[LastBtnPressed] + encodermov ) & B01111111;
            break;                
      }
      cli();
      encodermov=0;
      sei();
    }
    
    if ( millis() - omillis > 150 ) { // Refresh Display every 150 us
      TitleMenu(F("Bt "));  

      switch(Edt) {
        case 0 : OLed.fillRect(64, L1,  63, 16, SSD1306_WHITE); // Channel
                 break;
        case 1 : OLed.fillRect( 0, L2,  63, 16, SSD1306_WHITE); // Mode
                 break;         
        case 2 : OLed.fillRect(64, L2,  63, 16, SSD1306_WHITE); // Note
                 break;         
        case 3 : OLed.fillRect(64, L3,  63, 16, SSD1306_WHITE); // Value / Value on
                 break;        
        case 4 : OLed.fillRect( 0, L3,  63, 16, SSD1306_WHITE); // Value off 
                 break;                     
      }                       
      OLed.setTextColor(SSD1306_INVERSE);
      
      OLed.setCursor(36, L1);OLedprint2(LastBtnPressed);
      OLed.setCursor(68, L1);OLed.print(F("Ch "));  OLedprint2(ChnMessage[LastBtnPressed] & CHNMask);
      
//      OLed.setCursor(68, L2); OLedprint2( ChnMessage[LastBtnPressed] & CHNMask );  // chn
//      unsigned int Tg = ( Toggle    & ( 1 << LastBtnPressed ) );
      switch ( ChnMessage[LastBtnPressed] & MSGMask ) {
        case MidiModeNote  :
             OLed.setCursor( 4, L2);  OLed.print(F("Note"));
             OLed.setCursor(68, L2);  OLedprint4(ChnData1[LastBtnPressed]);
             OLed.setCursor( 4, L3);  OLed.print(F("Vel"));
             OLed.setCursor(68, L3);  OLedprint4(ChnData2[LastBtnPressed]);
             break;
        case MidiCtrlChange:            
             OLed.setCursor(68, L2);  OLedprint4(ChnData1[LastBtnPressed]);       
             OLed.setCursor(68, L3);  OLedprint4(ChnData2[LastBtnPressed]);
             if ( (unsigned int)(Toggle & ( (unsigned int)( 1 << LastBtnPressed ) )) > 0 ) {
               OLed.setCursor( 4, L2);  OLed.print(F("CtrS"));
               OLed.setCursor( 4, L3);  OLed.print(ChnData2f[LastBtnPressed]);
             } else {
               OLed.setCursor( 4, L2);  OLed.print(F("Ctrl"));            
               OLed.setCursor( 4, L3);  OLed.print(F("Val"));
             }             
             break;  
        case MidiProgChange:
             OLed.setCursor( 4, L2);  OLed.print(F("Prog"));
             OLed.setCursor(68, L2);  OLedprint4(ChnData1[LastBtnPressed]);          
             break;
      }
      
      OLed.display();
      omillis = millis();
    }
  }

  L595SendOneWord(0);
  ClearEncoder();  
}

// ----------------------------------------------------------------------------
//                                     D E M O
// ----------------------------------------------------------------------------

int Tempo = 150;

#define NbDemo 13
 unsigned int TabDemo[NbDemo] = {     0,    15,   240,  3840,
                                  61440,     0, 61440, 65280,
                                  65520, 65535, 28662,  1632,
                                      0 };

void PlayPatern(byte NbSteps, unsigned int Tab[] ) {
  for (byte i=0;i<NbSteps;i++)  {
    L595SendOneWord(Tab[i]);
    ReadBtnState();
    MyDelay(Tempo);
  }
}

void DoDemo() {
  boolean       LetRunning = true;
  unsigned long omillis    = 0;  
    
  while ( LetRunning ) {
    ReadBtnState();
    if ( readkey() == BTN_ENC ) {     // if Select Pressed exit
      LetRunning=false;
    }

    PlayPatern(NbDemo,TabDemo);

    if ( millis() - omillis > 150 ) { // Refresh Display every 150 us
      TitleMenu(F("Demo"));
      OLed.display();
      omillis = millis();
    }
  }
  ClearEncoder();  
}

// ============================================================================
//                          I N I T I A L I Z A T I O N
// ============================================================================

void setup() {
  Serial.begin(115200);

  pinMode(ROT_A      , INPUT_PULLUP);
  pinMode(ROT_B      , INPUT_PULLUP);
  pinMode(BTNEncoder , INPUT_PULLUP);
  
  L595Init();
  L165Init();

  if(!OLed.begin(SSD1306_SWITCHCAPVCC, OLedAdr)) {
    Serial.println(F("SSD1306 Error"));
  } else {
    Serial.println(F("SSD1306 Initialized"));
  }
  OLed.display();  // display adafruit logo

  attachInterrupt(digitalPinToInterrupt(ROT_A), doEncoder, CHANGE ); 
  
  delay(1000);  
  PlayPatern(NbDemo,TabDemo);
  delay(1000);
  ClearEncoder();  
}

// ============================================================================
//                             M A I N    L O O P
// ============================================================================

int menu  = 0;
int omenu = 2;

void loop() {

  if (encodermov != 0 ) {
    cli();
    menu+=encodermov;
    encodermov=0;
    sei();
    if ( menu > 2 ) { menu=0; }
    if ( menu < 0 ) { menu=2; }
  }

  if ( menu != omenu ) {
    OLed.clearDisplay();
    OLed.setTextSize(2);
    OLed.fillRect(0, menu*16, 127, 16, SSD1306_WHITE);
    OLed.setTextColor(SSD1306_INVERSE);
    OLed.setCursor(4, L1);  OLed.print(F("Live"));
    OLed.setCursor(4, L2);  OLed.print(F("Setup"));
    OLed.setCursor(4, L3);  OLed.print(F("Demo"));
    OLed.display();
    omenu=menu;
  }

  ReadBtnState();
  if ( keypressed() ) {
    byte k = readkey();
    if ( k == BTN_ENC ) {
      switch ( menu ) {
        case 0: DoLive();
                break;     
        case 1: DoConfig(); 
                break;
        case 2: DoDemo();
                break;                                       
      }
    }
    omenu=menu+1;  // to force redraw 
  }    
}
