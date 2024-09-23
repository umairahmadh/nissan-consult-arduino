//******************* mereni 0-100km/h a 0-400m ***
void mereni()
{
  lcd.clear();
  lcd_setCursor(0,1);
  // ***********0123456789012345***************
  lcd.print ("0-100km");
  lcd.write ((byte)7);
  lcd_setCursor (0,2);
  lcd.print("0-400m   ");
  //  lcd_setCursor(9,1);
  //  lcd.print(" s");
  ukoncit=0;
  butt_byl_1=false;
  if (rychlost<2.0)
  {
    mereni_bude_0_100 = true;
    mereni_bude_0_400 = true;
  }
  else
  {
    mereni_bude_0_100 = false;
    mereni_bude_0_400 = false;
    lcd_setCursor (0,3);
    lcd.print (" STOP  ");
    do{
      if (digitalRead (button1) == HIGH) butt_byl_1=true;
      if ((digitalRead (button1) == LOW)&&(butt_byl_1==true)) ukoncit=1; 
      time = millis();
      if ((time_comm_last+100ul) < time)
      {
        time_comm_last = time;
        comm();                // Komunikuj
      };

    }
    while(rychlost>=2.0 && ukoncit==0);
  };
  draha_mereni=0ul; 
  time_0_100 = 0ul;
  time_0_400 = 0ul;
  time_comm_last=millis();
  do{

    // smazat az pojede preruseni
    time = millis();
    if ((time_comm_last+100ul) < time)
    {
      time_comm_last = time;
      comm();                // Komunikuj
    };

    if (digitalRead (button1) == HIGH) butt_byl_1=true;
    if ((digitalRead (button1) == LOW)&&(butt_byl_1==true)) ukoncit=1;      

    if (digitalRead (button0) == HIGH) butt_byl_0=true;
    if ((digitalRead (button0) == LOW)&&(butt_byl_0==true))
    {

      if ((rychlost>=2.0)&&((mereni_bude_0_100==true)||(mereni_bude_0_400==true)))
      {
        lcd_setCursor (0,3);
        lcd.print (" STOP  ");
      }
      else
      {
        // tady predelat maze i za jizdy
        lcd_setCursor (0,3);  
        lcd.print(" MAZU  ");
        mereni_bude_0_100 = true;
        mereni_bude_0_400 = true;  
        draha_mereni=0ul; 
        time_0_100 = 0ul;
        time_0_400 = 0ul;
        lcd_setCursor (9,1);
        lcd.print ("       ");
        lcd_setCursor (9,2);
        lcd.print ("       ");           
      };
      butt_byl_0=false;
      delay (1000);
      lcd_setCursor (0,3);
      lcd.print("              ");
    }


    lcd_setCursor(0,0);
    lcd.print (rychlost,0);
    lcd.print (" km"); 
    lcd.write(7); 
    lcd.print("  ");

    //    time=millis();
    if ((rychlost<2.0)&&(mereni_bude_0_100==true)) 
    {
      time_0_100 = millis();
      lcd_setCursor (0,3);
      lcd.print ("PRIPRAV");
    };
    if ((rychlost<2.0)&&(mereni_bude_0_400==true)) 
    {
      draha_mereni=0ul;
      time_0_400 = millis();
    };
    //******* 1-100 *****************
    if ((rychlost>=2.0)&&(mereni_bude_0_100==true))
    {
      lcd_setCursor (9,1);
      lcd.print (((float)(millis()-time_0_100)/1000.0),3);
      lcd.print ("s  ");
      lcd_setCursor (0,3);
      lcd.print(" MERIM ");
    };
    if ((rychlost>=100.0) && (mereni_bude_0_100==true))
    {
      time_0_100 = millis()-time_0_100;
      lcd_setCursor (9,1);
      lcd.print ((((float)time_0_100)/1000.0),3);
      lcd.print ("s  ");
      mereni_bude_0_100 = false;
      lcd_setCursor (0,3);
      lcd.print("100 OK  ");
    };
    //******* 1-400 *****************  
    if ((rychlost>=2.0)&&(mereni_bude_0_400==true))
    {
      lcd_setCursor (9,2);
      lcd.print (((float)(millis()-time_0_400)/1000.0),0);
      lcd.print ("s ");
      lcd.print (draha_mereni/1000ul);
      lcd.print ("m  ");

    };
    if ((draha_mereni>=400000ul) && (mereni_bude_0_400==true))
    {
      time_0_400 = millis()-time_0_400;
      lcd_setCursor (9,2);
      lcd.print ((((float)time_0_400)/1000.0),3);
      lcd.print ("s  ");
      mereni_bude_0_400 = false;
      lcd_setCursor (9,3);
      lcd.print("400 OK ");
    };


  }
  while(ukoncit==0);
  lcd.clear();
  butt_byl_1=false;
  scr_fault_codes();
  butt_byl_1=false;
  lcd.clear();

};

