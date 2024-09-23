int MSB_a_LSB (int MSB, int LSB)
{

  return((MSB*256) & LSB);
}




// ************************* reset ****************

void scr_reset ()
{
  analogWrite(ledPin, ledMin);
  lcd.clear();
  lcd_setCursor(0,1);
  lcd.print (" MAZU SPOTREBU"); 
  blink_LCD_LED();
  if (digitalRead (button0) == HIGH)
  {
    eeprom_write_dword ((uint32_t*)0,long(0));
    eeprom_write_dword ((uint32_t*)8,long(0));
    eeprom_write_dword ((uint32_t*)32,long(0));
    eeprom_write_dword ((uint32_t*)40,long(0));
    eeprom_write_dword ((uint32_t*)44,long(0));    
    eeprom_write_dword ((uint32_t*)48,long(0));    
    ok_spotreba=0.0;
    prum_spotreba=0.0;
    prum_spotreba_eeprom=0.0;
    prum_spotreba_mereni=0ul;  
    prum_spotreba_mereni_eeprom=0ul;
    prum_rychlost=0ul;
    prum_rychlost_eeprom=0ul;
    draha=0ul;
    draha_eeprom=0ul;
    spotrebovano_stop = 0.0;
    spotrebovano_stop_eeprom = 0.0;
    time_of_denni = 0ul;
    time_of_eeprom = 0ul;
  };
  time_button = millis ();
  analogWrite(ledPin, ledMax);
  lcd.clear();
};






//******************* const cal. ************* linear scaling y=k*x+q
int prepocti_konstanty(int zobrazit)
{
  const_k_rychlost = ((float)k90-(float)k50)/40.0 ;
  const_q_rychlost = (float)k50-50.0*(const_k_rychlost);
  if (zobrazit == 1)
  {
    lcd_setCursor(0,3);
    lcd.print ("k: ");
    lcd.print(const_k_rychlost,2);
    lcd.print (" q: ");
    lcd.print(const_q_rychlost,2);
    lcd.print("  ");
  };
};






//******************* settings *****************************************
void scr_settings()
{
  lcd.clear();
  lcd_setCursor(0,0);
  // ***********0123456789012345***************
  lcd.print (" MER.  SK. RYCH");
  lcd_setCursor(0,1);
  // ***********0123456789012345***************
  lcd.print (" 50    "); 
  lcd_setCursor(7,1);  
  lcd.print(k50,DEC);
  lcd_setCursor(0,2);
  lcd.print (" 90    ");
  lcd_setCursor(7,2);
  lcd.print(k90,DEC); 
  lcd_setCursor(0,3);
  lcd.print ("k: ");
  lcd.print(const_k_rychlost,2);
  lcd.print (" q: ");
  lcd.print(const_q_rychlost,2);

  ukoncit =0;
  lcd.cursor();
  lcd.blink();
  lcd_setCursor(6,1);
  pomocna = 1;
  butt_byl_1=false;
  delay (900);
  do{
    //     time = millis ();
    if (digitalRead (button0) == HIGH) butt_byl_0=true;
    if (digitalRead (button1) == HIGH)
    {
      if (butt_byl_1==false) time_button = millis();
      butt_byl_1=true;
      if (time_button+1000ul <millis())
      {
        //blink_LCD_LED();
        if (digitalRead (button1) == HIGH)
        {
          EEPROM.write (24,k50);
          EEPROM.write (25,k90);
          ukoncit=1;
          lcd.clear();
          //delay (400);
        };
      };
    }
    if ((digitalRead (button0) == LOW) && (butt_byl_0==true))
    {   
      butt_byl_0 = false;
      if (pomocna == 1)
      {
        k50 = k50 + 1;
        if (k50>60) k50 = 40;
        prepocti_konstanty(1);
        lcd_setCursor(7,1);  
        lcd.print(k50,DEC);
        lcd.print("   ");
        lcd_setCursor(6,1);
      }; 
      if (pomocna == 2)
      {
        k90 = k90 + 1;
        if (k90>105) k90 = 75;
        prepocti_konstanty(1);
        lcd_setCursor(7,2);  
        lcd.print(k90,DEC);
        lcd.print("   ");
        lcd_setCursor(6,2);
      };
      delay (200); 

    };
    if ((digitalRead (button1) == LOW) && (butt_byl_1==true))
    {
      butt_byl_1=false;
      /*         if (time_button+1000ul <millis())
       {
       EEPROM.write (24,k50);
       EEPROM.write (25,k90);
       ukoncit=1;
       lcd.clear();
       }
       else*/
      //         {
      if (pomocna == 1){
        pomocna=2;
        lcd_setCursor(6,2); 
      }
      else {
        pomocna=1; 
        lcd_setCursor(6,1); 
      };
      delay (400);
      //         };
    };   

  }
  while (ukoncit==0);
  //  delay(400);
  butt_byl_1_set = true;
  lcd.noCursor();
  lcd.noBlink();
};



