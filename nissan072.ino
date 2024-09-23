/* Experimental LDC and serial comm NISSAN Primera GA16DE 94     */
/* http://dl.dropbox.com/u/24710673/nissan/nissan_scheme.pdf     */
/* Vojta Pichl pichl.vojta@post.cz                               */
/* for Arduino IDE 1.0  http://arduino.cc/en/Main/Software       */
/* GNU GPL 						         */
/*                                                               */
/*  START     b1: press during start setting injector sizes      */
/*                                                               */
/*  BASIC Scr b1: short press - chgn scr MEAS 1-100 km/h 0-400m  */
/*                long press  - screen calibration               */
/*            b0: short press - basic scr. chg. line 3 and 4     */
/*                long press  - delete eeprom data               */
/*  MAES Scr  b1: short press - chgn scr FAULT CODES             */
/*            b0: short press - delete measured data             */
/*  FAULT Scr b1: short press - chgn scr BASIC                   */
/*                long press  - Clear Self-Learning Value        */
/*            b0: long press  - Clear DTC (fault codes)          */

#define VERSION 0.72

/* TODO: 
 DONE fault codes
 DONE Clear DTC (fault codes)
 DONE time of drive
 other sensors? Throttle Position 
 Adjust Fuel Injection Time ?
 DONE Clear Self-Learning Value ?
 Store daily data to eeprom, if temp>60 deg C restore it, when start
 */

#include <EEPROM.h>
#include <avr/eeprom.h> 
//#include <MsTimer2.h> // http://www.arduino.cc/playground/Main/MsTimer2
//#include <TimerOne.h>

#include <LiquidCrystal.h>
// RS,Enable,D4,D5,D6,D7,D8  10k resistor 5v-out-gnd

LiquidCrystal lcd(13, 12, 11, 10 ,9, 8);
#define LCD_x 16
#define LCD_y 4

#define OSTRY_PROVOZ

// control LCD LEDs, PWM out
#define ledPin 6
#define ledMax 200 // 50
#define ledMaxB 210 // 50
#define ledMin 100 // 5
#define button0 7
#define button1 5


byte black[8] = {
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
};
byte prum[8] = {
  //  B00000,
  //  B00000,
  B01101,
  B10010,
  B10101,
  B01001,
  B10110,
  B00000,
  B00000,
  B00000,
};
byte prum_DL[8] = {
  //  B00000,
  //  B00000,
  B01101,
  B10010,
  B10101,
  B01001,
  B10110,
  B00000,
  B10011,
  B11011,
};
byte lt_100[8] = {
  B10111,
  B10010,
  B11010,
  B00000,
  B11111,
  B10010,
  B01010,
  B11010,
};  
byte lt_h[8] = {
  B10111,
  B10010,
  B11010,
  B00000,
  B10100,
  B11100,
  B10100,
  B00000,
}; 
byte black_2[8] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
};
byte deg[8] = {
  B00000,
  B00010,
  B00101,
  B00010,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte hod[8] = {
  B00001,
  B00010,
  B00100,
  B01000,
  B10000,
  B00101,
  B00111,
  B00101,
};

// VAR dec ********************************************
int i;
int c;
int f;
int read_byte;
int comm_OK;
int MAF_LSB;
int MAF_MSB;
int last[32];
//  byte last_1;
//  byte last_2;
//  byte last_3;
//  byte last_4;
int faults[32];
byte ukoncit;
byte ukoncit2;
byte zapis_eeprom=0;
byte scr_ll = 0;
byte scr = 0;
float pomocna=0.0;
float pomocna2=0.0;
float napeti=0.0;
float predstih=0.0;
float vstrik=0.0;
float vstrik_akt=0.0;
float vstrik_last=0.0;
long otacky=0l;
long otacky_akt=0l;
long otacky_last=0l;
float rychlost=0.0;
float rychlost_akt=0.0;
float rychlost_last=0.0;
float prum_rychlost=0.0;
float prum_rychlost_eeprom=0.0;
float const_k_rychlost = 1.0;
float const_q_rychlost = 0.0;
int k50=50;
int k90=90;
long teplota=0l;
long O2=0l;
float ok_spotreba=0.0;
float prum_spotreba=0.0;
float prum_spotreba_eeprom=0.0;
float vstrikovace=185.0; // 185 for GA16DE
float spotrebovano;
float spotrebovano_eeprom;
float spotrebovano_stop;
float spotrebovano_stop_eeprom;
float spotrebovano_celkem;
float spotrebovano_celkem_eeprom; 
float prum_spotreba_celkem;
float prum_spotreba_celkem_eeprom;
long prum_spotreba_mereni=0l;  
long prum_spotreba_mereni_eeprom=0l;  
unsigned long time;
unsigned long time_comm_last;
unsigned long time_print_last;
unsigned long time_write_eeprom=0ul;
unsigned long draha_akt=0ul;
unsigned long draha=0ul;
unsigned long draha_eeprom;
unsigned long draha_mereni;
unsigned long time_mereni_last;
unsigned long pomocna_ul=0ul;
unsigned long time_0_100=0ul;
unsigned long time_0_400=0ul;
unsigned long time_lambda_nula;
unsigned long time_prijem_start;
unsigned long time_of_denni=0ul;
unsigned long time_of_eeprom=0ul;
unsigned long time_of_last=0ul;
unsigned long doba_hodin_cele;


//  unsigned long time_vyp_prum_last;
boolean butt_byl_0 = false;
boolean butt_byl_1 = false;
boolean butt_byl_1_set = false;
boolean mereni_bude_0_100=false;
boolean mereni_bude_0_400=false;
boolean lambda_byla_nula=false;
boolean prijem_OK = false;
unsigned long time_button;
unsigned long time_button_1;
int testLED=200;  



//******************* setup *******************
void setup() {
  lcd.createChar(0, black);
  delay (10);
  lcd.createChar(1, prum);
  delay (10);
  lcd.createChar(2, lt_100);
  delay (10);
  lcd.createChar(3, lt_h);
  delay (10);
  lcd.createChar(4, black_2);
  delay (10);
  lcd.createChar(5, prum_DL);
  delay (10);
  lcd.createChar(6, deg);
  delay (10);
  lcd.createChar(7, hod);
  delay (20);
  lcd.begin(LCD_x, LCD_y);
  Serial.begin (9600);
  prum_spotreba_mereni_eeprom = eeprom_read_dword ((uint32_t*)0);
  prum_spotreba_eeprom = (float(eeprom_read_dword ((uint32_t*)8)))/1000.0 ;
  vstrikovace =           float(eeprom_read_dword ((uint32_t*)16));
  if (vstrikovace==0) vstrikovace = 185.0;

  draha_eeprom = eeprom_read_dword ((uint32_t*)32);
//  prum_rychlost_eeprom = (float)eeprom_read_dword ((uint32_t*)40);
  spotrebovano_stop_eeprom = (float(eeprom_read_dword ((uint32_t*)44)))/1000.0 ;
  time_of_eeprom = eeprom_read_dword ((uint32_t*)48);

  k50 = (int) EEPROM.read(24);
  k90 = (int) EEPROM.read(25);
  scr_ll = (int) EEPROM.read(26);
  prepocti_konstanty(0);

  pinMode(ledPin, OUTPUT);
  pinMode(button0, INPUT);
  //   digitalWrite(button0, HIGH); // pull up resistor
  analogWrite(ledPin, ledMax);

  if (digitalRead (button1) == HIGH || digitalRead (button0) == HIGH)
  {    
    lcd_setCursor(0, 0);
    lcd.print("NASTAVENI");
    lcd_setCursor(0, 1);
    lcd.print("konec dlouze");
    blink_LCD_LED(); 
    blink_LCD_LED();
    blink_LCD_LED();
    lcd_setCursor(0, 2);
    lcd.print("Vstriky:   "); 
    lcd_setCursor(0, 3);
    lcd.print (vstrikovace,0);
    lcd.print (" cc");
    ukoncit = 0;
    time_button = millis();
    do{
      time = millis(); 
      if (digitalRead (button1) == HIGH)
      {
        //    butt_byl_0 = true;
        if ((time_button+1000)<time) 
        {
          blink_LCD_LED();
          if (digitalRead (button1) == HIGH)
          {
            ukoncit = 1;
            lcd.clear();
            eeprom_write_dword ((uint32_t*)16,long(vstrikovace)); 
          };
          //       butt_byl_0 = false;
          ukoncit = 1;
          lcd.clear();
        };
      };

      if (digitalRead (button1) == LOW)
      { 
        time_button = time;
      };

      if (digitalRead (button0) == HIGH)
      {
        butt_byl_0 = true;
      };
      if (digitalRead (button0) == LOW) 
      {
        if(butt_byl_0==true) 
        {
          if (vstrikovace==185.0){
            vstrikovace=259.0;
          } // 185 GA16DE 259 SR20DE
          else {
            vstrikovace = 185.0;
          };
          lcd_setCursor(0, 3);
          lcd.print (vstrikovace,0);
          lcd.print (" cc");
        };
        butt_byl_0 = false;
        //         time_button = time;
        delay (100);
        //         time_button = time;
      };

    } 
    while (ukoncit == 0);
  };

  lcd_setCursor(0, 2);
  lcd.print("Nissan interface"); 
  lcd_setCursor(0, 3);
  lcd.print(VERSION,2);
  lcd.print(" Inj: "); 
  lcd.print (vstrikovace,0);
  lcd.print (" cc");
  delay (1000);
}






//******************* main *********************************************************
void loop(){

  // Init ECU   
  comm_OK=0;
  lcd_setCursor(0, 0);  // column,line
  lcd.print("Start comm "); 
  lcd_setCursor(10, 0);
#ifdef OSTRY_PROVOZ
  comm_OK = init_ECU ();
#endif
  if (comm_OK == 1) 
  {
    lcd_setCursor(0, 0);
    lcd.print("OK, comm"); 
    delay (1000);
  };

  // start loop
  time=millis();
  time_comm_last=time-100ul;
  time_print_last=time-800ul;
  time_write_eeprom=time;
  scr=0;
  lcd.clear ();
  lcd_setCursor(0, 0);
  // ************** loop *******************************
  // nefunguje mrzne
  //#ifdef OSTRY_PROVOZ
  //   MsTimer2::set(1000, comm); // 100ms period communication
  //   MsTimer2::start();

  //#endif

  do{
    time = millis ();

    //      if (digitalRead (button0) == HIGH ) analogWrite(ledPin, 200);
    //      if (digitalRead (button0) == LOW ) analogWrite(ledPin, 0);

    if (digitalRead (button0) == HIGH) 
    {
      butt_byl_0 = true;
      if ((time_button+1000)<time) 
      {
        scr_reset ();
        butt_byl_0 = false;
      };
      delay (50);
    };
    if (digitalRead (button0) == LOW) 
    {
      if(butt_byl_0==true) 
      {
        scr_ll++;
        if (scr_ll>3) scr_ll=0;
        EEPROM.write (26,scr_ll);

        lcd_setCursor(0,3);
        lcd.print ("                ");
        scr_basic(); 
        delay (150); 
      };
      butt_byl_0 = false;
      time_button = time;
    };

    if (butt_byl_1_set == false)
    {
      if ((digitalRead (button1) == HIGH))
      {         
        butt_byl_1 = true;
        if ((time_button_1+1000)<time)
        {
          scr = 1;
          scr_settings();               
          scr = 0;
          scr_basic();
          butt_byl_1 = false;
        };
        delay (50);
      };

      if (digitalRead (button1) == LOW)
      {
        if (butt_byl_1==true)
        {
          scr=2;
          // scr_ll=0;
          mereni();
          scr=0;
          scr_basic();
          butt_byl_1_set = true;
          delay(100);
        };
        butt_byl_1=false;
        time_button_1 = time;
      }; 

    };

    if ((digitalRead(button1) == LOW) &&(butt_byl_1_set==true)) {
      butt_byl_1_set = false;
      delay(200);
    };

    //    predelano na MsTimer2     
    if ((time_comm_last+100ul) < time) // bylo 100
    {
      time_comm_last = time;
      comm();                // Komunikuj
    };
    if (((time_print_last+1000ul) < time) && (scr == 0))
    {
      time_print_last = time;
      scr_basic();            //Zobraz

    };
    // pokud rychlost mensi nez 30 co 20s zapisuj eeprom
    if (((time_write_eeprom+20000ul) < time) && (rychlost < 30.0))
    {
      eeprom_write_dword ((uint32_t*)0,prum_spotreba_mereni_eeprom);
      eeprom_write_dword ((uint32_t*)8,long(prum_spotreba_eeprom*1000.0));
      eeprom_write_dword ((uint32_t*)32,draha_eeprom);
//      eeprom_write_dword ((uint32_t*)40,prum_rychlost_eeprom); 
      eeprom_write_dword ((uint32_t*)44,long(spotrebovano_stop_eeprom*1000.0)); 
      eeprom_write_dword ((uint32_t*)48,time_of_eeprom);   

      time_write_eeprom = time;
    };

  }
  while (1);

};   








