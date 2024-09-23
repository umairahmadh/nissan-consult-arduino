
//***************** for 16x4 cur. poz adj. hack *******
void lcd_setCursor (int coll, int line)
{
  if ((LCD_x==16)&&(LCD_y==4))
  {
    if (line<2)
    {
      lcd.setCursor(coll,line);
    }
    else
    {
      lcd.setCursor((coll-4),line);
    };
  }
  else
  {
    lcd.setCursor(coll,line);
  };
};



void blink_LCD_LED()
{
  delay (200);
  analogWrite(ledPin, ledMin);
  lcd.noDisplay();
  delay (200);
  analogWrite(ledPin, ledMaxB);
  lcd.display();
  delay (200);
  analogWrite(ledPin, ledMin);
  lcd.noDisplay();
  delay (200);
  analogWrite(ledPin, ledMaxB);
  lcd.display();
  delay (200);
  analogWrite(ledPin, ledMin);
  lcd.noDisplay();
  delay (200);
  analogWrite(ledPin, ledMax);
  lcd.display();
};

//****************** time print *******************************
void print_time (unsigned long time_to_print, bool seconds) //, int coll_t, int line_t)
{
  int hour;
  int minute;
  int second;

  time_to_print = time_to_print/1000ul;
  //time_of_eeprom = 0ul;
  hour = time_to_print/(60*60);
  minute = (time_to_print%(60*60))/60;
  //  second = ((time_to_print%(60*60))%60)/60;
  second = time_to_print - ((hour*60*60)+minute*60);

  if (hour<10) lcd.print ("0");
  lcd.print (hour,DEC);
  lcd.print (":");
  if (minute<10) lcd.print ("0");
  lcd.print (minute,DEC);
  if (seconds == true)
  {
    lcd.print (":");
    if (second<10) lcd.print ("0");
    lcd.print (second,DEC);  
  };
};


//****************** lambda voltage ************************ 
void print_O2()
{
  //         lcd.print ("O2: ");
  lcd_setCursor(0,3);
  if (O2<10)
  {
    lcd.print ("   ");
    lcd.print (O2,DEC);
  }
  if (O2>=10 && O2<100)
  {
    lcd.print ("  ");
    lcd.print (O2,DEC);
  }
  if (O2>=100 && O2<1000)
  {
    lcd.print (" ");
    lcd.print (O2,DEC);
  }
  if (O2>=1000)
  {
    lcd.print (O2,DEC);
  }
};





// ****************** LCD print ********************************
void scr_basic ()
{
  lcd_setCursor(0,0); 
  //         lcd.print ("Spot:");
  if (ok_spotreba<10.0) lcd.print (" ");
  lcd.print (ok_spotreba,1);
  lcd_setCursor(4,0);
  if (rychlost >10.0)       
  {
    lcd.write ((byte)2);           
    //           lcd.print ("Lt/100  ");                    
  }
  else
  {
    lcd.write ((byte)3);  
    //           lcd.print ("Lt/h    ");                    
  };

  // prumer spotreba denni
  lcd_setCursor(0,1);
  lcd.write ((byte)1); 
  if (prum_spotreba_celkem>=100.0)
  {
    lcd.print ("99.9");
  }
  else 
  {
    if (prum_spotreba_celkem<10.0) lcd.print (" ");
    lcd.print (prum_spotreba_celkem,2);
  };
  lcd_setCursor(6,1);
  lcd.write ((byte)2); // LT/ST 

  // prumer spotreba dlouhodoba
  lcd_setCursor(9,1);
  lcd.write ((byte)5); // znak prumer dlohodob
  //          lcd.print (" ");
  if (prum_spotreba_celkem_eeprom<10.0) lcd.print (" ");
  lcd.print (prum_spotreba_celkem_eeprom,2);
  //         lcd_setCursor(15,1);
  lcd_setCursor(15,1);
  lcd.write ((byte)2); // LT/ST 
  //        lcd.print ("   ");        
  //         lcd.print ("Lt/100");

  if (scr_ll==0)
  { 
    lcd_setCursor(0,2);      
    //         lcd.print ("Rych: ");
    lcd.print (" ");
    if (rychlost<10.0) lcd.print ("  ");
    if ((rychlost>=10.0)&&(rychlost<100.0)) lcd.print (" ");
    lcd.print (rychlost,0);
    //      lcd_setCursor(4,2);
    lcd.print ("km"); ///h  ");
    lcd.write(7);
    lcd.print("  ");

    lcd_setCursor(9,2); 
    //         lcd.print ("Ot:  ");
    if (otacky<1000 && otacky>0){
      lcd.print (" ");
    };
    if (otacky==0){
      lcd.print ("   ");
    };         
    lcd.print (otacky,DEC);
    lcd_setCursor(13,2);
    lcd.print ("rpm");


    lcd_setCursor(0,3);
    lcd.print(" "); 
    if (teplota<10) lcd.print("  ");
    if (teplota>=10 && teplota < 100) lcd.print(" "); 
    //         lcd.print ("Tepl:");
    lcd.print (teplota,DEC);
    //           lcd_setCursor(11,3);
    lcd.print (" ");
    lcd.write ((byte)6); // degre
    lcd.print("C  ");

    lcd_setCursor(9,3); 
    lcd.print(" ");
    //         lcd.print ("Batt:");
    lcd.print (napeti,1);
    //           lcd_setCursor(1,3);
    lcd.print (" V");
  };

  if (scr_ll==1)
  {
    lcd_setCursor(0,2);      
    lcd.write(1);
    if (prum_rychlost<10.0){
      lcd.print ("  ");
    };
    if ((prum_rychlost>=10.0)&&(prum_rychlost<100.0)){
      lcd.print (" ");
    };
    lcd.print (prum_rychlost,0);
    lcd_setCursor(4,2);
    lcd.print("km");
    lcd.write(7);
    lcd.print("   ");

    lcd_setCursor(9,2); 
    lcd.write(5);
    if (prum_rychlost_eeprom<10.0){
      lcd.print ("  ");
    };
    if ((prum_rychlost_eeprom>=10.0)&&(prum_rychlost_eeprom<100.0)){
      lcd.print (" ");
    };
    lcd.print (prum_rychlost_eeprom,0);
    lcd_setCursor(13,2);
    lcd.print("km");
    lcd.write(7);
    lcd.print("   ");

    print_O2();

    lcd_setCursor(4,3);
    lcd.print (" mV   ");

    lcd_setCursor(9,3);
    lcd.print (vstrik,2);
    lcd_setCursor(13,3);
    lcd.print (" ms");
  };
  if (scr_ll==2)
  {
    lcd_setCursor(0,2);
    pomocna=((float)draha)/1000000.0;
    //           if (draha < 10000ul)
    if (pomocna<10.0) lcd.print("  ");
    if (pomocna>=10.0 && pomocna < 100.0) lcd.print("   ");
    if (pomocna>=100.0 && pomocna < 1000.0) lcd.print("  ");
    if (pomocna>=1000.0 && pomocna < 10000.0) lcd.print(" ");
    if (pomocna<10.0)
    {
      lcd.print (pomocna,1);
    }
    else
    {
      lcd.print(pomocna,0);
    };
    lcd.print ("km  ");

    lcd_setCursor(9,2);
    pomocna=((float)draha_eeprom)/1000000.0;
    if (pomocna<10.0) lcd.print("  ");
    if (pomocna>=10.0 && pomocna < 100.0) lcd.print("   ");
    if (pomocna>=100.0 && pomocna < 1000.0) lcd.print("  ");
    if (pomocna>=1000.0 && pomocna < 10000.0) lcd.print(" ");
    if (pomocna<10.0)
    {
      lcd.print (pomocna,1);
    }
    else
    {
      lcd.print(pomocna,0);
    };
    lcd.print ("km  ");           


    lcd_setCursor(0,3);
    if (spotrebovano_celkem < 100.0) lcd.print(" ");
    if (spotrebovano_celkem<10.0)
    {
      lcd.print(spotrebovano_celkem,2);
    }
    else
    {
      lcd.print(spotrebovano_celkem,1);
    };
    lcd.print ("Lt");

    lcd_setCursor(9,3);
    if (spotrebovano_celkem_eeprom<10.0) lcd.print("  ");
    if (spotrebovano_celkem_eeprom>=10.0 && spotrebovano_celkem_eeprom < 100.0) lcd.print(" ");
    lcd.print(spotrebovano_celkem_eeprom,1);
    lcd.print ("Lt");

  };
  if (scr_ll==3)
  {
    lcd_setCursor(0,2);
    //    lcd.print ("D: ");
#ifndef OSTRY_PROVOZ
    time_of_denni = 123456789;
#endif
    print_time (time_of_denni,true);
    //    lcd.print ("     ");

    //    lcd_setCursor(0,3);
    lcd.print ("  ");
    print_time (time_of_eeprom,false);
    //    lcd.print ("     ");
    lcd.print (" ");

    lcd_setCursor(0,3);
    lcd.print ("                ");

  };

};



