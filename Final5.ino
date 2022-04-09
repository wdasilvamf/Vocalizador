                                                                                                                                                                                                                                                                                                                                                                           
 /*       
   * UFMG 
   * Fac Educação 
   *  LAPOA - GEINE 
   *  Laboratório Interdisciplinar de Produção de Objetos de Aprendizagem para a pessoa com deficiência 
   *  Grupo Interdisciplinar de Estudos sobre Educação Inclusiva e Necessidades Educacionais Especiais
   *  Profa. Regina Célia Passos Ribeiro de Campos
   *    Estudante Beatriz Fonseca Torres
   * 
   * Esc.Engenharia  
   *   Depto Eletrônica - Cefala
   *   Prof. Maurílio Nunes Vieira
   *     Estudante Wanderson Maciel 
   */
   

// i2c LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// MP3 Player
#include <SoftwareSerial.h>
#include "DFPlayer_Mini_Mp3.h"


// CD4515 (4 to 16 decoder) address lines
#define strobe 12
#define A       8        // D8  = PORTB PB0 ... Data 1 => 4515/pin 2  (lsb)
#define B       9        // D9  = PORTB PB1 ... Data 2 => 4515/pin 3   
#define C       10       // D10 = PORTB PB2 ... Data 3 => 4515/pin 21
#define D       11       // D11 = PORTB PB3 ... Data 4 => 4515/pin 22 (msb)
#define strobe  12       //        => 4515 pino 1

//                    ------------------------------------------
//           PORTB:   PB7  PB6  PB5   PB4  PB3   PB2   PB1   PB0
// Arduino UNO/NANO:            D13   D12  ~D11  ~D10  ~D9   D8
// CD 4515                           Srobe  D     C     B    A
//                    ------------------------------------------

// kebyboard return lines
#define Ret1    2                     // keyboard return 1
#define Ret2    3                     // keyboard return 2
#define Ret3    4                     // keyboard return 1

#define DEL     127                   // ASCII for "delete"
#define ENT     13                    // ASCII for CR = Carriage Return ('enter')
#define SPC     32                    // ASCII for Space

#define COL     14                    // nb of columns in alfabeto[3][COL]

// Mp3Player
#define Rx      5
#define Tx      6
#define Busy    7
#define VOL     15                     // default Volume (0 ... 30)
#define VolUp   1                      // A1 input (pull up resistor)
#define VolDown 0                      // A0 input (pull up resistor)
#define EQ      2                      // 0/1/2/3/4/5  =  Normal/Pop/Rock/Jazz/Classic/Bass

                     

void mp3_set_EQ (uint16_t eq); 
// i2C LCD Display at address 0x3f 
LiquidCrystal_I2C lcd(0x3f,2,1,0,4,5,6,7,3, POSITIVE);

// creates character for "enter" (carriage return)
byte EnterSymbol[8] = {
  B00000,                  
  B00001,                      
  B00001,                         
  B00101,                          
  B01001,                          
  B11111,                          
  B01000,                          
  B00100                           
};
 

// MP3 Player
SoftwareSerial mySerial(Rx, Tx); 

void setup(){
  // LCD i2C   powered by extenal supply
  // i2c uses pins A4 and A5 (with 4.7 kohm pull up resistors)
  lcd.begin (16,2);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.blink();
  lcd.print("UFMG - Out/2017");
  lcd.setCursor(0,1);
  lcd.print(" Inicializando ");
  lcd.createChar(ENT,EnterSymbol);      

  
  // CD4515 connections  
  pinMode(A, OUTPUT);                 // Data 1 A (pino 2 4515)
  pinMode(B, OUTPUT);                 // Data 2 B (pino 3 4515)   
  pinMode(C, OUTPUT);                 // Data 3 C (pino 21 4515)   
  pinMode(D, OUTPUT);                 // Data 4 D (pino 22 4515) 
  pinMode(strobe, OUTPUT);            // Strobe (pino 1 4515)
  
  // keyboard return lines
  pinMode(Ret1, INPUT_PULLUP);        // selected by alfabeto[0,i], i = 0 ... 11
  pinMode(Ret2, INPUT_PULLUP);        // selected by alfabeto[1,i], i = 0 ... 11
  pinMode(Ret3, INPUT_PULLUP);        // selected by alfabeto[2,i], i = 0 ... 11

  // MP3 Player 
  // http://www.picaxe.com/docs/spe033.pdf
  /* MP3 Player 
    http://www.picaxe.com/docs/spe033.pdf
    Arquivos ".mp3" ou ".wav":
     - devem estar na pasta /mp3
     - devem ser numerados com 4 algarismos:
       0001.mp3   (a)
       0002.mp3   (b)
        ...       ...
       0038.mp3   (-)

     - pode ser colado texto após os 4 algarimos:
       0001a.wav      (a)
       0002b.wav      (b)
        ...           ...
       00
       0038menos.wav  (-)
  */
  
  /************************************************
  **Arduino          DFPlayer: 
  *Pin5 -- 1kohm --   TX (3); 
  *Pin6 -- 1kohm --   RX (2);
  *pin7               BUSY (16)
  *5V                 1 (Vcc)
  *GND                7 (Gnd)
  *                   6 headphone +
  *                   8 headphone -
  *                   4 DAC_R  => PAM 8403 power amp                
  *                   5 DAC_L  => PAM 8403 power amp
  ***************************************************/
  pinMode(Busy, INPUT);
  Serial.begin (9600);
  mySerial.begin (9600);
  mp3_set_serial (mySerial);  //set softwareSerial for DFPlayer-mini mp3 module 
  delay(5000); // delay 1ms to set volume
  mp3_set_volume (VOL); // value 0~30; default VOL = 25
  delay(10);
  
  mp3_set_EQ (EQ);    // default Equalizer  0/1/2/3/4/5  Normal/Pop/Rock/Jazz/Classic/Bass

  // PAM403 3W CLASS-D STEREO AUDIO AMPLIFIER
  // https://www.allelectronics.com/mas_assets/theme/allelectronics/spec/AMP-8403.pdf
  // PAM 8610 (2 x 15W)?
  // https://tronixlabs.com.au/breakout-boards/amplifier/mini-amplifier-breakout-board-2-x-3w-class-d-pam8403/

  hello(2000);            // mensagens iniciais 2000 = delay in ms
 
}
 
/*   saída do 4515
                Y0 =    0    1    2    3    4      5     6    7    8     9   10    11     12    13 */
                
char alfabeto[3][COL]={{'a', 'd', 'g', 'j', 'm',   '0',  '2', 'n', 'q', 't', 'w', ENT,   '1',  '3'},
                      { 'b', 'e', 'h', 'k', 'z',   '4',  '6', 'o', 'r', 'u', 'x', SPC,   '5',  '7'},  
                      { 'c', 'f', 'i', 'l', DEL,   '-',  '9', 'p', 's', 'v', 'y', '?',   '8',  '+'}};    

uint16_t Oder[3][COL] = {1,   4,   7,  10,   13,   27,   29,  14,  17,  20,  23,  40,    28,   30,
                         2,   5,   8,  11,   26,   31,   33,  15,  18,  21,  24,  41,    32,   34,
                         3,   6,   9,  12,   39,   38,   36,  16,  19,  22,  25,  42,    35,   37};

/*
ordem dos arquivos de áudio (podem ser inseridos helps controlados, por exemplo, com pulsadas do botão '?'
a  b  c  d  e  f  g  h  i   j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  0  1  2  3  4  5  6  7  8  9   +   -  del  ent  spc  ?
1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36  37  38  39   40   41   42
                  
*/
                     
int lcd_count = 0;    // Global variable (usar static?)
void loop() {
  Address4515();
  VolCntr();   
}
               
                 
void Address4515() {
  int  j;      
  byte i;
 
  for( i=0; i < COL; i++) { 
     /* 
     4515 input: 0000 DCBA   (i)
                 0000 0000   (0)
                 0000 0001   (1)
                 0000 0010   (2)
                 0000 ...    ...
                 0000 1100  (COL)
   */
    digitalWrite(strobe, HIGH);             // Enable addressing
       
    PORTB = PORTB & B11110000;              // clear Data1 ... Data4 of CD4515
    PORTB = PORTB | i;                      // Address CD 4515
           
    digitalWrite(strobe, LOW);              // disable addressing

    j = KeyScan();                          // key pressed?
     
    if(j<3){                                // yes
      if(alfabeto[j][i] != DEL && alfabeto[j][i] != ENT && alfabeto[j][i] != SPC && alfabeto[j][i] != '?' ) {
         LcdDisplay(j,(int)i);
         Audio(Oder[j][i]);
       }
    }
  } 
}  




int KeyScan()
/*
 * CD4515 applies LOW to addressed output
*/
{
 int j;
 for(j = 0; j<3; j++){ 
  if(digitalRead(j+Ret1) == 0) { 
    delay(200);                             // empirical
    while(digitalRead(j+Ret1) == 0) { 
      // wait while key is pressed
    } 
    break;
  }
 }
 return j;  // j = 3 ==> no key was pressed
}               



void LcdDisplay(int j, int i) {
  // lcd_count: global variable
  if(lcd_count==16){                      // last column?
    lcd.setCursor(0,1);                   // begin of 2nd line
  }
  
  if(lcd_count > 31){                     // is last position in LCD?
    lcd.clear();                          // clear and move cursor to (0,0)
    lcd_count=0;
  }

  if(alfabeto[j][i] == ENT) {
    lcd.print((char) ENT);
    lcd_count++;
  }
  else {
    lcd.print(alfabeto[j][i]);
    lcd_count++;
  }
  
  if(i==25 && lcd_count != 16 && lcd_count != 32) {
     lcd.print(" ");
  } 

  delay(50);
}



void Audio(uint16_t num) {
 do{
 }while (!digitalRead(Busy));
 mp3_play(num);
 delay(10);                               // Is this line necessary?
}


void hello(int tempo) {
  // mensagens iniciais
  // lcd_count: global variable

  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0,0); lcd.print("Alfabeto Vocali-");
  lcd.setCursor(0,1); lcd.print("zador em Braille");
  delay(tempo);

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("FAE/LAPOA/GEINE");
  lcd.setCursor(0,1); lcd.print(" Profa Regina");
  delay(tempo);
  
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("FAE/LAPOA/GEINE");
  lcd.setCursor(0,1); lcd.print("Beatriz Fonseca");
  
  delay(tempo);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("DElt/Cefala/LSI");
  lcd.setCursor(0,1); lcd.print(" Prof Maurilio");
  delay(tempo);
  
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("DElt/Cefala/LSI");
  lcd.setCursor(0,1); lcd.print("Wanderson Maciel");
  delay(tempo);
  
  lcd.clear();
  lcd.blink(); 
  lcd_count=0;                        // global variable
}

void VolCntr(){
  static int vol = VOL;                 // initialize first time only
  int x;
  
  if (x = analogRead(VolUp) < 100) {  // check VolUp key
    do{
      if ((vol++) > 30) {
        vol = 30;
      }
      mp3_set_volume (vol); // value 0~30; 
      delay(100);
      // LcdDisplay(2,13); // +
  
    } while ((x = analogRead(VolUp) < 100));
  }  

  if (x = analogRead(VolDown) < 100) {  // check VolDown key
    do{
      if ((vol--) < 0) {
        vol = 0;
      }
      mp3_set_volume (vol); // value 0~30; 
      delay(100);
      // LcdDisplay(2,5); // -
      
    } while ((x = analogRead(VolDown) < 100));
  }  
}


void AudioEqualization(){
  static int Eq = 1; 
  // 0/1/2/3/4/5  =  Normal/Pop/Rock/Jazz/Classic/Bass

  if ((Eq++) > 5) {
        Eq = 0;
  }

   mp3_set_EQ (Eq);

   switch(Eq) {
      case 0:   // Normal
        LcdDisplay(0,0);  
      break;

      case 1:  // Pop
      LcdDisplay(0,5);
      break;

      case 2:   // Rock
        LcdDisplay(0,12);
      break;

      case 3:   // Jazz
        LcdDisplay(0,6);
      break;

      case 4:   // Classic
        LcdDisplay(0,13);
      break;

      case 5:   //Bass
         LcdDisplay(1,12);
      break;
 
   }
   delay(200);
   
}

