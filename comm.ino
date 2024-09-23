//******************* inicializace ECU *******************


int init_ECU ()
{
  i == 0;
  Serial.write ((byte)0xFF);
  Serial.write ((byte)0xFF);
  Serial.write ((byte)0xEF);
  lcd_setCursor(10, 0);

  do{
    if (Serial.available()>0)
    {
      read_byte = Serial.read();
      lcd_setCursor(0, 1);
      lcd.print (read_byte,HEX);
    };
    if (i>998)
    {
      lcd_setCursor(10, 0);
      i=0;
      Serial.write ((byte)0xFF);
      Serial.write ((byte)0xFF);
      Serial.write ((byte)0xEF);
      //      Serial.write ((byte)0xFF);
      //      Serial.write ((byte)0xFF);
      //      Serial.write ((byte)0xEF);
    };

    i++; 
    lcd_setCursor (11,0);
    lcd.print (i,DEC);
  } 
  while ((read_byte!=0x10)); //&&(read_byte!=0x30));
  return (1);
};



//********************* Communication ***************************
void comm ()
{
  if (comm_OK==1)
  {
    Serial.flush ();
    delay (10);                // bylo 100
    Serial.write ((byte)0x5a);  // zacatek telegramu

    Serial.write ((byte)0x0c);  // batt volt  9  8
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x00);  // rmp MSB   8  7
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x01);  // rmp LSB   7  6  
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x14);  // vstrik MSB  6  5
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x15);  // vstrik LSB  5  4
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x08);  // teplota   4  3
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x16);  // predstih  3  2
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x0b);  // rychlost  2  1
    Serial.write ((byte)0x5a);
    Serial.write ((byte)0x09);  // O2  MSB  0

    Serial.write ((byte)0xf0);  // telegram end
    i=0;
    c=0;
    f = 30;
    while (f>0)
    {
      last[f] = 0;                
      f--;
    }; 
    last[0] =0;

    ukoncit = 1;
    time_prijem_start = millis();
    do{
      if ((time_prijem_start + 200ul)<millis())  // watchdog
      {
        init_ECU();
        lcd.clear();
        lcd_setCursor (0,0);
        lcd.print ("        NEKOMUN");
        delay (200);
        ukoncit = 0;
        ukoncit2 = 1;
      };
      if (Serial.available()>0)
      {
        i++;
        //         lcd_setCursor(14,3);      
        //         lcd.print (i,DEC);
        f = 30;
        while (f>0)
        {
          last[f] = last[f-1];                
          f--;
        };         
        last[0] = Serial.read();
        //         if (c>13) {c=0;lcd_setCursor (0,1);lcd.print("              ");};
        //        lcd_setCursor (0,1);
        c=c+3;

      };
      //       if ((last_3==0xA5)&&(last_2==0xFF)&&(last_1==0x01))
      // ECU odpovida start stream - pozor z minula flush nevycisti uplne,
      //   inv. prikaz, 0xFF, pocet byte, zprava (A5,C,FF,1,XX a potom uz jenom FF,1,XX)

      // telegram je cely dekodovat      
      if ((last[10]==0xFF)&&(last[9]==0x09))
      {
        Serial.write ((byte)0x30);

        pomocna = (float)last [8];
        napeti = (pomocna*80.0)/1000.0;         

        pomocna = (float)last [7];
        pomocna2 = (float)last [6];
        otacky_akt = (long)(((pomocna*256.0)+pomocna2)*12.5);        
        otacky = (otacky_akt+otacky_last)/2;
        otacky_last = otacky;

        pomocna = (float)last[5];
        pomocna2 = (float)last[4];
        vstrik_akt = (((pomocna*256.0)+pomocna2)/100.0);
        vstrik = (vstrik_akt+vstrik_last)/2.0;
        vstrik_last = vstrik;

        pomocna = (float)last [3];
        teplota = (long)(pomocna-50.0);

        pomocna = (float)last[2];
        predstih = (float)(pomocna - 110.0);

        pomocna = (float)last [1];
        rychlost_akt = (float)(pomocna*2.0);
        rychlost = (rychlost_akt+rychlost_last)/2.0;
        // speed cal.
        if (rychlost>=2.0)
        {
          rychlost = (rychlost * const_k_rychlost) - const_q_rychlost;
        }
        else {
          rychlost=0.0;
        };

        rychlost_last = rychlost;

        pomocna = (float)last [0];
        O2 = (long)(pomocna*10.0);

        if (O2 == 0.0 && lambda_byla_nula == false)
        {
          time_lambda_nula = millis();
          lambda_byla_nula = true;
        };      
        if (O2 > 0.0) 
        {
          lambda_byla_nula = false;
        };

        pomocna = (float)otacky;
        pomocna2 = (float)rychlost;
        if (rychlost >10.0)
        {  //                   mnoz/min                           lt/hod               Lt/100km
          ok_spotreba = (float)(((pomocna*2.0*vstrik/(60.0*1000.0))*vstrikovace*60.0/1000.0)/pomocna2)*100.0;
        }                                                        // 185 cc GA16DE
        else
        {
          ok_spotreba = (float)((pomocna*2.0*vstrik/(60.0*1000.0))*vstrikovace*60.0/1000.0); 
        };

        // pokud lambda hlasi nulu a teplota vetsi nez 30, nulova okamzita spoteba
        if ((rychlost >10.0) && (teplota > 30ul) && (O2==0l)&&((time_lambda_nula+200ul)<millis())&&(vstrik<4.5))
        {
          ok_spotreba = 0.000001;
        }

        // pocet mereni (bezrozmer)
        if (rychlost >10.0)
        {
          prum_spotreba = ((prum_spotreba*(float)prum_spotreba_mereni)+ok_spotreba)/(float)(prum_spotreba_mereni+1ul);
          prum_spotreba_mereni++;
        };

        // dlouhodoby prumer
        if (rychlost>10.0)
        {
          prum_spotreba_eeprom = ((prum_spotreba_eeprom*(float)prum_spotreba_mereni_eeprom)+ok_spotreba)/(float)(prum_spotreba_mereni_eeprom+1ul);
          prum_spotreba_mereni_eeprom++;
        };

        //draha
        time = millis();
        //                   mm/ms
        if (rychlost>2.0)
        {
          pomocna = (float)rychlost;
          // tady mozna zrada
          pomocna_ul =(pomocna*(1000000.0/(60.0*60.0)));
          draha_akt = (pomocna_ul*(time-time_mereni_last))/1000.0;
        }
        else
        {
          draha_akt = 0ul;
        };

        if (rychlost<=10.0)
        {
          spotrebovano_stop = spotrebovano_stop + (ok_spotreba * ((time - time_mereni_last)/(1000.0*60.0*60.0)));
          spotrebovano_stop_eeprom = spotrebovano_stop_eeprom + (ok_spotreba * ((time - time_mereni_last)/(1000.0*60.0*60.0)));
        };

        //prumer rychlost - pocitat podle drahy a casu
        /*
        if (rychlost>10.0)
         {
         prum_rychlost = (prum_rychlost * (float)prum_spotreba_mereni + rychlost)/ (float)(prum_spotreba_mereni+1ul);
         prum_rychlost_eeprom = ((float)prum_rychlost_eeprom * (float)prum_spotreba_mereni_eeprom + rychlost)/ (float)(prum_spotreba_mereni_eeprom+1ul);
         };*/

        pomocna=((float)draha)/1000000.0;
        pomocna2 = ((float)time_of_denni)/(60.0*60.0*1000.0); 
        if (pomocna>0.0 && pomocna2>0.0)
        {
          prum_rychlost = pomocna / pomocna2;
        }
        else
        {
          prum_rychlost = 0.0;
        };

        pomocna=((float)draha_eeprom)/1000000.0;
        pomocna2 = ((float)time_of_eeprom)/(60.0*60.0*1000.0); 
        if (pomocna>0.0 && pomocna2>0.0)
        {
          prum_rychlost_eeprom = pomocna / pomocna2;
        }
        else
        {
          prum_rychlost = 0.0;
        };

        // spotrebovano_stop_eeprom = 0.0;        
        time_mereni_last = time;

        draha=draha+draha_akt;
        draha_eeprom=draha_eeprom+draha_akt; 

        if (mereni_bude_0_400==true) draha_mereni=draha_akt+draha_mereni;  


        // bargraph okamzite spotreby *****************************
        //    if (scr==0)
        //    {    
        if (((napeti < 13.0) && (otacky > 500)) || (teplota>100l) )
        {
          if ((napeti < 13.0) && (otacky > 500))
          {
            lcd_setCursor (5,0);
            lcd.print ("  NAPETI ! ");
          };
          if (teplota>100l)
          {
            lcd_setCursor (5,0);
            lcd.print ("  TEPLOTA !");
          };
        }
        else
        {

          lcd_setCursor (5,0);
          if (ok_spotreba< 4.5) lcd.print ("           ");
          if (ok_spotreba>=4.5)
          {
            if (ok_spotreba<4.75) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<5.0) lcd.print("          ");
          };
          if (ok_spotreba>=5.0)
          {
            if (ok_spotreba<5.25) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<5.5) lcd.print("         ");
          }; 
          if (ok_spotreba>=5.5) 
          {
            if (ok_spotreba<5.75) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<6.0) lcd.print("        ");
          };
          if (ok_spotreba>=6.0)
          {
            if (ok_spotreba<6.25) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<6.5) lcd.print("       ");
          }; 
          if (ok_spotreba>=6.5)
          {
            if (ok_spotreba<6.75) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<7.0) lcd.print("      ");
          };  
          if (ok_spotreba>=7.0)
          {
            if (ok_spotreba<7.25) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<7.5) lcd.print("     ");
          };  
          if (ok_spotreba>=7.5)
          {
            if (ok_spotreba<7.75) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<8.0) lcd.print("    ");
          };  
          if (ok_spotreba>=8.0)
          {
            if (ok_spotreba<8.25) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<8.5) lcd.print("   ");
          }; 
          if (ok_spotreba>=8.5)
          {
            if (ok_spotreba<8.75) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<9.0) lcd.print("  ");
          };  
          if (ok_spotreba>=9.0)
          {
            if (ok_spotreba<9.25) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            if (ok_spotreba<9.5) lcd.print(" ");
          };  
          if (ok_spotreba>=9.5)
          {
            if (ok_spotreba<9.75) {
              lcd.write ((byte)4); 
            }
            else {
              lcd.write ((byte)0);
            };
            //           if (ok_spotreba<8.0) lcd.print("          ");
          };  
        };         

        if (scr_ll==1&&scr==0)  print_O2();

        ukoncit = 0;
      };
      // };
    }
    while (ukoncit == 1);
  }
  else
  {

    lcd_setCursor (6,0);
    lcd.print ("NEKO     ");
    delay (100);
    lcd_setCursor (6,0);
    lcd.print ("    MUNIK");
    delay (100);
  };

  // spotrebovano litru


  spotrebovano_celkem = spotrebovano + spotrebovano_stop;
  spotrebovano_celkem_eeprom = spotrebovano_eeprom + spotrebovano_stop_eeprom;

  if (draha>0ul)
  {
    pomocna=(float)draha/1000000.0;
    spotrebovano = prum_spotreba*(pomocna/100.0);
    //    pomocna=(float)draha/1000000.0;
    spotrebovano_celkem = spotrebovano + spotrebovano_stop;
    prum_spotreba_celkem = spotrebovano_celkem / (pomocna/100.0);
  }
  else
  {
    spotrebovano = 0.0;
    prum_spotreba_celkem = 0.0;
  };

  if (draha_eeprom>0ul)
  {
    pomocna=(float)draha_eeprom/1000000.0;
    spotrebovano_eeprom = prum_spotreba_eeprom*(pomocna/100.0);
    spotrebovano_celkem_eeprom = spotrebovano_eeprom + spotrebovano_stop_eeprom;
    prum_spotreba_celkem_eeprom = spotrebovano_celkem_eeprom / (pomocna/100.0);
  }
  else
  {
    spotrebovano_eeprom = 0.0;
    prum_spotreba_celkem_eeprom = 0.0;
  };

  time = millis();
  if (otacky>0.0)
  {
    time_of_denni = time_of_denni + (time - time_of_last);
    time_of_eeprom = time_of_eeprom + (time - time_of_last);
  };
  time_of_last = time;

};        








