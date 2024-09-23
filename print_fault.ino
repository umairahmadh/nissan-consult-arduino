//************* Print fault codes ***************************
// opov 207 46 255 2 85 0 255 2 85 0
//       0   1  2  3  4 5
// ECU odpovida 0 start stream 0x30 - pozor potvrz konce z minula, ignorovat
//              1 inv. prikaz 0x2e
//              2 0xff
//              3 pocet byte ve zprave 2
//              4 chyba 0x55
//              5 vyskyt x krat 0

void scr_fault_codes()
{
  lcd.clear();
  //  delay (200);
  ukoncit = 0;
  pomocna = read_faults();
  if (pomocna == 1)
  {
    /*   lcd_setCursor(0,1);   
     lcd.print (faults[0],DEC);
     lcd.print (" ");
     lcd.print (faults[1],DEC);
     lcd.print (" ");
     lcd.print (faults[2],DEC);*/
    lcd_setCursor(0,1);   
    lcd.print ("Fault codes");

    lcd_setCursor(0,2);
    lcd.print (faults[3],HEX);
    lcd.print (" ");
    lcd.print (faults[4],DEC);
    lcd.print ("x  ");
    if (faults[2]>2)
    {
      lcd_setCursor(8,2);   
      lcd.print (faults[5],HEX);
      lcd.print (" ");
      lcd.print (faults[6],DEC);
      lcd.print ("x ");
    }; 
    if (faults[2]>4)
    {
      lcd_setCursor(0,3);   
      lcd.print (faults[7],HEX);
      lcd.print (" ");
      lcd.print (faults[8],DEC);
      lcd.print ("x  ");
    };
    if (faults[2]>6)
    {
      lcd_setCursor(8,3); 
      lcd.print (faults[9],HEX);
      lcd.print (" ");
      lcd.print (faults[10],DEC);
      lcd.print ("x ");
    }
  }
  else 
  {
    lcd_setCursor(0,1);
    lcd.print ("Reading Error");
  };
  ukoncit2 = 0;
  time_comm_last=millis();
  do{
    time = millis();
    if (digitalRead (button1) == HIGH)
    {
      butt_byl_1=true;
      if ((time_button_1+1000)<time)
      {
        clear_self_learning_value();
        delay (2000);
        butt_byl_1 = false;
        ukoncit = 1;
      };
    };
    if (digitalRead (button1) == LOW)
    {
      if (butt_byl_1==true) ukoncit=1;
      time_button_1 = time; 
    };

    if ((time_comm_last+100ul) < time)
    {
      time_comm_last = time;
      comm();                // Komunikuj
    };

    if (digitalRead (button0) == HIGH) 
    {
      butt_byl_0 = true;
      if ((time_button+1000)<time) 
      {
        clear_fault_codes ();
        delay (2000);
        butt_byl_0 = false;
        ukoncit = 1;
      };
      delay (50);
    };
    if (digitalRead (button0) == LOW) 
    {
      butt_byl_0 = false;
      time_button = time;
    };

  }
  while((ukoncit==0) && (ukoncit2==0));

};


int read_faults ()
{
  prijem_OK = false;
  if (comm_OK==1)
  {
    Serial.flush ();
    delay (10);                // bylo 100
    Serial.write ((byte)0xd1);  // fault codes
    Serial.write ((byte)0xf0);  // telegram end
    f = 30;
    while (f>0)
    {
      faults[f] = 0;                
      f--;
    }; 

    ukoncit2 = 0;
    time_prijem_start = millis();
    f = 0;


    do{
      if ((time_prijem_start + 200ul)<millis())  // watchdog
      {
        init_ECU();
        lcd.clear();
        lcd_setCursor (0,1);
        lcd.print ("        NEKOMUN");
        delay (200);
        ukoncit2 = 2;
        ukoncit = 1;
      };
      if (Serial.available()>0) 
      {
        faults[f] = Serial.read();
        if (faults [f] == 0x2e) prijem_OK=true; // start stream begin save data
        if (prijem_OK==true) f++;
      };

      // if (((faults[2]>0) && ((f+3) >= faults[2])) ||
      if (f>30) 
      {
        ukoncit2 = 1;
        Serial.write ((byte)0x30);
      };

    } 
    while (ukoncit2 == 0);
  };
  return (ukoncit2);
};

void clear_fault_codes()

{
  analogWrite(ledPin, ledMin);
  lcd.clear();
  lcd_setCursor(0,1);
  lcd.print ("MAZU FAULT CODES"); 
  blink_LCD_LED();
  if (digitalRead (button0) == HIGH)
  {
    Serial.flush ();
    delay (10);                // bylo 100
    Serial.write ((byte)0xc1);  // clear fault codes
    Serial.write ((byte)0xf0);  // telegram end
    delay (100);
    Serial.write ((byte)0x30); //
    lcd.clear();
    lcd_setCursor(0,1);
    lcd.print ("   SMAZANO");
    ukoncit = 1;
  }
  else
  {
    lcd.clear();
    lcd_setCursor(0,1);
    lcd.print ("  NESMAZANO");
    ukoncit = 1;
  };
}

void clear_self_learning_value()
{
  analogWrite(ledPin, ledMin);
  lcd.clear();
  lcd_setCursor(0,1);
  lcd.print ("MAZU ");
  lcd_setCursor(0,2);
  lcd.print ("SELF-LEARNING");
  lcd_setCursor(0,3);
  lcd.print ("VALUES");
  blink_LCD_LED();
  if (digitalRead (button1) == HIGH)
  {
    Serial.flush ();
    delay (10);                // bylo 100
    Serial.write ((byte)0x0a);  // clear fault codes
    Serial.write ((byte)0x8b);  // telegram end
    Serial.write ((byte)0x00);
    Serial.write ((byte)0xf0);
    delay (100);
    //  Serial.write ((byte)0x30); //
    lcd.clear();
    lcd_setCursor(0,1);
    lcd.print ("   SMAZANO");
    ukoncit = 1;
  }
  else
  {
    lcd.clear();
    lcd_setCursor(0,1);
    lcd.print ("  NESMAZANO");
    ukoncit = 1;
  };
};


