/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      CoreTask.ino
 *
 *  Description:
 *      Core task handling for display, control and background processing.
 *
 *  Hardware:
 *      ESP32 Dev Module
 *      I2S audio output
 *      SPI display / touchscreen interface
 *      SD card sample and wavetable storage
 *
 *  Original Design:
 *      RealTimeAudioLab
 *
 *  Original Development Environment:
 *      Arduino IDE 1.8.13
 *      ESP32 Arduino Core 1.0.4
 *      Board: ESP32 Dev Module
 *      Partition Scheme: Huge APP
 *      PSRAM: Disabled
 *
 *  Repository:
 *      https://github.com/RealTimeAudioLab/RTAL-WSYN-001-WELLENFORMER
 *
 *  Copyright (c) 2025-2026 RealTimeAudioLab
 *
 *  Released as part of the RTAL Engineering Heritage Collection.
 *  See LICENSE.md and NOTICE.md for publication and third-party license notes.
 *
 ******************************************************************************/

void CoreTask0( void *parameter )
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  ts.begin();
  ts.setRotation(1);
  delay(100);  

  pinMode(16, OUTPUT); 
  digitalWrite(16, LOW); 
  pinMode(17, OUTPUT); 
  digitalWrite(17, LOW);

  pinMode(ledPin1, OUTPUT); 
  digitalWrite(ledPin1, LOW); 
  pinMode(ledPin2, OUTPUT); 
  digitalWrite(ledPin2, LOW);
  pinMode(ledPin3, OUTPUT); 
  digitalWrite(ledPin3, LOW);             

  pinMode(5, OUTPUT); 
  digitalWrite(5, HIGH);
  if(!SD.begin(5)) ESP.restart();
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  } 
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.println("initialisation done.");
  delay(10); 

  File root = SD.open("/7");
  if(!root) return;
  File file = root.openNextFile();
  while(file){
      String mBstring=file.name();
      String test = mBstring.substring(3);
      uint32_t scount=(test.toInt());
      if (scount>last_file) last_file=scount;
      file = root.openNextFile();} 
  root.close();
  file.close();  
    
  byte d = 40;
  plotLinear(" DIR ", 0, 130);
  plotLinear(" WAV ", 1 * d, 130);  
  plotLinear(" ST1 ", 2 * d, 130);
  plotLinear(" LP1 ", 3 * d, 130);
  plotLinear(" SO1 ", 4 * d, 130);
  plotLinear(" SE1 ", 5 * d, 130);
  plotLinear(" ST2 ", 6 * d, 130);
  plotLinear(" LP2 ", 7 * d, 130);
  plotLinear(" SO2 ", 8 * d, 130);
  plotLinear(" SE2 ", 9 * d, 130);
  plotLinear(" VL1 ", 10 * d, 130);
  plotLinear(" VL2 ", 11 * d, 130);

// Buttons und Wellenformenanzeigen  
  tft.fillRect(0, 0, 81, 126, TFT_GREY);  //Buttons 1-3
  tft.fillRect(5, 3, 71, 119, TFT_WHITE);
  tft.fillRect(10, 8, 61, 34, TFT_LTGREY);
  tft.fillRect(10, 46, 61, 34, TFT_LTGREY);
  tft.fillRect(10, 84, 61, 34, TFT_LTBLUE); 
   
  tft.fillRect(81, 0, 158, 126, TFT_GREY);  //Scope1
  tft.fillRect(86, 3, 148, 119, TFT_WHITE);
  tft.drawLine(96, 63, 229, 63, TFT_BLACK);
  
  tft.fillRect(240, 0, 158, 126, TFT_GREY); //Scope2
  tft.fillRect(245, 3, 148, 119, TFT_WHITE);
  tft.drawLine(250, 63, 387, 63, TFT_BLACK);
  
  tft.fillRect(399, 0, 81, 126, TFT_GREY);  //Buttons 4-6
  tft.fillRect(404, 3, 71, 119, TFT_WHITE);
  tft.fillRect(409, 8, 61, 34, TFT_LTGREY);
  tft.fillRect(409, 46, 61, 34, TFT_LTGREY);
  tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
  
  tft.setTextSize(2);
  tft.fillRect(10, 8, 61, 34, TFT_LTORANGE);  //Button 1 und 4 PROGRAM UP/DOWN
  tft.fillRect(409, 8, 61, 34, TFT_LTORANGE);
  tft.setTextColor(TFT_BLACK,TFT_LTORANGE);
  tft.setCursor(17,19);
  tft.print(F("P")); 
  tft.print((PROGRAM / 100) % 10);
  tft.print((PROGRAM / 10) % 10);
  tft.print(PROGRAM % 10); 
  tft.setCursor(416,19); 
  tft.print(F("P"));  
  tft.print((PROGRAM / 100) % 10);
  tft.print((PROGRAM / 10) % 10);
  tft.print(PROGRAM % 10); 
  tft.fillRect(10, 46, 61, 34, TFT_GREEN);  //Button 2 LOAD
  tft.setTextColor(TFT_BLACK,TFT_GREEN); 
  tft.setCursor(17,57);
  tft.print(F("LOAD"));    
  tft.setTextColor(TFT_BLACK,TFT_LTBLUE); //Button 3 und 6 
  tft.setCursor(17,95);
  tft.print(F("OPT")); 
  tft.setCursor(416,95);
  tft.print(F("DCO2")); 
  tft.fillRect(409, 46, 61, 34, TFT_LTRED);  //Button 5 SAVE
  tft.setTextColor(TFT_BLACK,TFT_LTRED); 
  tft.setCursor(416,57);
  tft.print(F("SAVE"));
 
  digitalWrite(ledPin1, HIGH); 
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW); 
  LoadSample();
  digitalWrite(ledPin1, LOW); 
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, LOW);  
  ShowSample_1(); 
  digitalWrite(ledPin1, LOW); 
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, HIGH);  
  ShowSample_2();
  digitalWrite(ledPin1, LOW); 
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);     
      
  for (;;) 
   {
    delay(1);

    if(button5_pressed_second==true) //wenn SAVE nur 1x gedrückt und innerhalb von 2sec kein weiterer Button dann beende Save-Vorgang
      {
        if(millis()-save_back_millis>4000)
          {
           button5_pressed_second=false;
           tft.setTextSize(2);
           tft.fillRect(409, 46, 61, 34, TFT_LTRED); 
           tft.setTextColor(TFT_BLACK,TFT_LTRED); 
           tft.setCursor(416,57);
           tft.print(F("SAVE"));
          }
      }          
      
    uint16_t x, y;
    if (ts.tirqTouched()) {
      if (ts.touched()) {
        TS_Point p = ts.getPoint(); //xmin=150; xmax=3950; ymin=200; ymax=4000;
        p.x = p.x - 100;
        if(scope1_fullscreen==true) //IRQ Scope 1
          {
           if(p.x>250&&p.x<600) //Buttons 1-4 
              {
               if(p.y>300&&p.y<1100) {scbutton1_pressed=true; button_millis=millis();}
               if(p.y>1300&&p.y<2000) {scbutton2_pressed=true; button_millis=millis();}
               if(p.y>2200&&p.y<2800) {scbutton3_pressed=true; button_millis=millis();}
               if(p.y>3100&&p.y<3700) //button 4
                 {
                  if(scbutton2_pressed_state>0) scbutton2_pressed_state=0;
                  if(scbutton3_pressed_state>0) scbutton3_pressed_state=0;
                  scope1_fullscreen=false;
                  scope_return();
                 }              
              } 
           if(p.x>3350&&p.x<3700) //Buttons 5-8
              {
               if(p.y>300&&p.y<1100) {scbutton5_pressed=true; button_millis=millis();}
               if(p.y>1300&&p.y<2000) {scbutton6_pressed=true; button_millis=millis();}
               if(p.y>2200&&p.y<2800) {scbutton7_pressed=true; button_millis=millis();}
               if(p.y>3100&&p.y<3700) {scbutton8_pressed=true; button_millis=millis();}
              }
           if(p.x>1000&&p.x<3030&&p.y>700&&p.y<3600) //Scope 1
              {
                byte x=map(p.x,1000,3030,0,256-N_Balken);
                byte y=map(p.y,700,3600,255,0);             
                for(uint16_t i=x; i<(x+N_Balken+1); i++)
                   {
                    samples1[i]=y;
                    sampledata[i+(SDCARDWAVE1*256)]=samples1[i];
                    sample_1=(255-samples1_alt[i])+32;
                    tft.drawLine(i+110,160,i+110,sample_1, TFT_WHITE);
                    sample_1=(255-samples1[i])+32;
                    tft.drawLine(i+110,160,i+110,sample_1, TFT_ORANGE);
                    tft.drawLine(100, 160, 376, 160, TFT_BLACK);
                    samples1_alt[i]=samples1[i];
                   } 
              }
          if(p.x>750&&p.x<900&&p.y>1950&&p.y<2300) //Pfeil nach links
              { 
               leftarrow_pressed=true; 
               button_millis=millis();             
              }

          if(p.x>3100&&p.x<3250&&p.y>1950&&p.y<2300) //Pfeil nach rechts
              { 
               rightarrow_pressed=true; 
               button_millis=millis();              
              }   
          } // IRQ Scope 1 Ende
          
        else if(scope2_fullscreen==true)  //IRQ Scope 2
          {
          if(p.x>250&&p.x<600) //Buttons 1-4 
              {
               if(p.y>300&&p.y<1100) {scbutton1_pressed=true; button_millis=millis();}
               if(p.y>1300&&p.y<2000) {scbutton2_pressed=true; button_millis=millis();}
               if(p.y>2200&&p.y<2800) {scbutton3_pressed=true; button_millis=millis();}
               if(p.y>3100&&p.y<3700) //button 4 BACK
                 {
                  if(scbutton2_pressed_state>0) scbutton2_pressed_state=0;
                  if(scbutton3_pressed_state>0) scbutton3_pressed_state=0; 
                  scope2_fullscreen=false;
                  scope_return();
                 } 
              } 
        if(p.x>3350&&p.x<3700) //Buttons 5-8
              {
               if(p.y>300&&p.y<1100) {scbutton5_pressed=true; button_millis=millis();}
               if(p.y>1300&&p.y<2000) {scbutton6_pressed=true; button_millis=millis();}
               if(p.y>2200&&p.y<2800) {scbutton7_pressed=true; button_millis=millis();}
               if(p.y>3100&&p.y<3700) {scbutton8_pressed=true; button_millis=millis();}
              }
        if(p.x>1000&&p.x<3030&&p.y>700&&p.y<3600) //Scope 2
              {
                byte x=map(p.x,1000,3030,0,256-N_Balken);
                byte y=map(p.y,700,3600,255,0);               
                for(uint16_t i=x; i<(x+N_Balken+1); i++)
                   {
                    samples2[i]=y;
                    sampledata[i+(SDCARDWAVE2*256)]=samples2[i];
                    sample_2=(255-samples2_alt[i])+32;
                    tft.drawLine(i+110,160,i+110,sample_2, TFT_WHITE);
                    sample_2=(255-samples2[i])+32;
                    tft.drawLine(i+110,160,i+110,sample_2, TFT_ORANGE);
                    tft.drawLine(100, 160, 376, 160, TFT_BLACK);
                    samples2_alt[i]=samples2[i];
                   } 
              }
        if(p.x>750&&p.x<900&&p.y>1950&&p.y<2300) //Pfeil nach links
              { 
               leftarrow_pressed=true; 
               button_millis=millis();             
              }

        if(p.x>3100&&p.x<3250&&p.y>1950&&p.y<2300) //Pfeil nach rechts
              { 
               rightarrow_pressed=true; 
               button_millis=millis();              
              }                                       
          } //IRQ Scope 2 Ende
        else
          { //IRQ Standard               
            switch (menu){
               case 0:
                if((p.x>200&&p.x<400)&&(p.y>1700&&p.y<3610)) {value[0]=map(p.y,3600,1800,0,127); if(value[0]>127) value[0]=127; SDCARDbank=value[0]>>4; WavetableChanged_millis=millis(); WavetableChanged=true;}
                if((p.x>500&&p.x<700)&&(p.y>1700&&p.y<3610)) {value[1]=map(p.y,3600,1800,0,127); if(value[1]>127) value[1]=127; SDCARDval=value[1]; WavetableChanged_millis=millis(); WavetableChanged=true;} //if(Cvalue<32) SDCARDval=value[1]; else SDCARDval=31;
                if((p.x>800&&p.x<1000)&&(p.y>1700&&p.y<3610)) {value[2]=map(p.y,3600,1800,0,127); if(value[2]>127) value[2]=127; if(value[2]<0) value[2]=0; SAMPLESTART1=value[2];}
                if((p.x>1100&&p.x<1300)&&(p.y>1700&&p.y<3610)) {value[3]=map(p.y,3600,1800,0,127); if(value[3]>127) value[3]=127; if(value[3]<0) value[3]=0; SAMPLELOO1=SDCARDWAVE1=value[3]; ShowSample_1();}
                if((p.x>1400&&p.x<1600)&&(p.y>1700&&p.y<3610)) {value[4]=map(p.y,3600,1800,0,127); if(value[4]>127) value[4]=127; if(value[4]<0) value[4]=0; SAMPLELEN1=value[4];}
                if((p.x>1700&&p.x<1900)&&(p.y>1700&&p.y<3610)) {value[5]=map(p.y,3600,1800,0,127); if(value[5]>127) value[5]=127; if(value[5]<0) value[5]=0; SAMPLEINCTIME1=value[5];}
                if((p.x>2000&&p.x<2200)&&(p.y>1700&&p.y<3610)) {value[6]=map(p.y,3600,1800,0,127); if(value[6]>127) value[6]=127; if(value[6]<0) value[6]=0; SAMPLESTART2=value[6];}
                if((p.x>2300&&p.x<2500)&&(p.y>1700&&p.y<3610)) {value[7]=map(p.y,3600,1800,0,127); if(value[7]>127) value[7]=127; if(value[7]<0) value[7]=0; SAMPLELOO2=SDCARDWAVE2=value[7]; ShowSample_2();}
                if((p.x>2600&&p.x<2800)&&(p.y>1700&&p.y<3610)) {value[8]=map(p.y,3600,1800,0,127); if(value[8]>127) value[8]=127; if(value[8]<0) value[8]=0; SAMPLELEN2=value[8];}
                if((p.x>2900&&p.x<3100)&&(p.y>1700&&p.y<3610)) {value[9]=map(p.y,3600,1800,0,127); if(value[9]>127) value[9]=127; if(value[9]<0) value[9]=0; SAMPLEINCTIME2=value[9];}
                if((p.x>3200&&p.x<3400)&&(p.y>1700&&p.y<3610)) {value[10]=map(p.y,3600,1800,0,127); if(value[10]>127) value[10]=127; if(value[10]<0) value[10]=0; VOL1=value[10];}
                if((p.x>3500&&p.x<3700)&&(p.y>1700&&p.y<3610)) {value[11]=map(p.y,3600,1800,0,127); if(value[11]>127) value[11]=127; if(value[11]<0) value[11]=0; VOL2=value[11];}
                break;
                
              case 1:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[12]=map(p.y,3600,1800,0,127); if(value[12]>127) value[12]=127; if(value[12]<0) value[12]=0; Pitch1=value[12];}
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[13]=map(p.y,3600,1800,0,127); if(value[13]>127) value[13]=127; if(value[13]<0) value[13]=0; Pitch2=value[13];} 
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[14]=map(p.y,3600,1800,0,127); if(value[14]>127) value[14]=127; if(value[14]<0) value[14]=0; PITCHFINE1=value[14];}
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[15]=map(p.y,3600,1800,0,127); if(value[15]>127) value[15]=127; if(value[15]<0) value[15]=0; PITCHFINE2=value[15];}
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) {value[16]=map(p.y,3600,1800,0,127); if(value[16]>127) value[16]=127; if(value[16]<0) value[16]=0;
                   GLISSANDO=value[16]; 
                   for(byte i = 0; i < MAX_NOTES; i++)
                      {
                       Glis_count[i]=0;
                       Glis_val[i]=-24;   
                      }
                   }
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[17]=map(p.y,3600,1800,0,127); if(value[17]>127) value[17]=127; if(value[17]<0) value[17]=0; Velocity_Mode=value[17]>>4;}
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[18]=map(p.y,3600,1800,0,127); if(value[18]>127) value[18]=127; if(value[18]<0) value[18]=0; NoiseLevel=value[18];}
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[19]=map(p.y,3600,1800,0,127); if(value[19]>127) value[19]=127; if(value[19]<0) value[19]=0; GAIN1=value[19]>>4;}
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[20]=map(p.y,3600,1800,0,127); if(value[20]>127) value[20]=127; if(value[20]<0) value[20]=0; GAIN2=value[20]>>4;}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[21]=map(p.y,3600,1800,0,127); if(value[21]>127) value[21]=127; if(value[21]<0) value[21]=0; Fclpf=mapFloat(value[21],0,127,0.001f, 0.25f); Set_LPF_Parameter();}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[22]=map(p.y,3600,1800,0,127); if(value[22]>127) value[22]=127; if(value[22]<0) value[22]=0; Qlpf=mapFloat(value[22],0,127,0.707, 10.0f); Set_LPF_Parameter();}
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[23]=map(p.y,3600,1800,0,127); if(value[23]>127) value[23]=127; if(value[23]<0) value[23]=0; EFX_Sat=value[23];}
                break;
    
              case 2:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[24]=map(p.y,3600,1800,0,127); if(value[24]>127) value[24]=127; if(value[24]<0) value[24]=0; FREQUENZ=FREQUENZ_save=value[24];}
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[25]=map(p.y,3600,1800,0,127); if(value[25]>127) value[25]=127; if(value[25]<0) value[25]=0; q=q_save=value[25]<<9;} 
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[26]=map(p.y,3600,1800,0,127); if(value[26]>127) value[26]=127; if(value[26]<0) value[26]=0; DCFATTACK=value[41]=value[26];}
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[27]=map(p.y,3600,1800,0,127); if(value[27]>127) value[27]=127; if(value[27]<0) value[27]=0; DCFDECAY=value[42]=value[27];} 
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) {value[28]=map(p.y,3600,1800,0,127); if(value[28]>127) value[28]=127; if(value[28]<0) value[28]=0; if (value[28]<127) DCFSUSTAIN=((value[28]+1)<<9); else DCFSUSTAIN=65535; value[43]=value[28];}
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[29]=map(p.y,3600,1800,0,127); if(value[29]>127) value[29]=127; if(value[29]<0) value[29]=0; value[29];}
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[30]=map(p.y,3600,1800,0,127); if(value[30]>127) value[30]=127; if(value[30]<0) value[30]=0; value[30];}
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[31]=map(p.y,3600,1800,0,127); if(value[31]>127) value[31]=127; if(value[31]<0) value[31]=0; Fcls=mapFloat(value[31],0,127,0.001,0.25); Set_LowShelf_Parameter();} 
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[32]=map(p.y,3600,1800,0,127); if(value[32]>127) value[32]=127; if(value[32]<0) value[32]=0; peakGainls=mapFloat(value[32],0,127,0.0,10.0); Set_LowShelf_Parameter();}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[33]=map(p.y,3600,1800,0,127); if(value[33]>127) value[33]=127; if(value[33]<0) value[33]=0; Fchs=mapFloat(value[33],0,127,0.032,0.5); Set_HighShelf_Parameter();}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[34]=map(p.y,3600,1800,0,127); if(value[34]>127) value[34]=127; if(value[34]<0) value[34]=0; peakGainhs=mapFloat(value[34],0,127,0.0,10.0); Set_HighShelf_Parameter();}
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[35]=map(p.y,3600,1800,0,127); if(value[35]>127) value[35]=127; if(value[35]<0) value[35]=0; DCFKeyFollow=value[35];} //DCF KeyFollow
                break;
    
              case 3:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[36]=map(p.y,3600,1800,0,127); if(value[36]>127) value[36]=127; if(value[36]<0) value[36]=0; ATTACK=value[36];}
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[37]=map(p.y,3600,1800,0,127); if(value[37]>127) value[37]=127; if(value[37]<0) value[37]=0; DECAY=value[37];} 
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[38]=map(p.y,3600,1800,0,127); if(value[38]>127) value[38]=127; if(value[38]<0) value[38]=0; SUSTAIN=map(value[38],0,127,1,65535);}
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[39]=map(p.y,3600,1800,0,127); if(value[39]>127) value[39]=127; if(value[39]<0) value[39]=0; RELEASE=value[39];} 
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) {value[40]=map(p.y,3600,1800,0,127); if(value[40]>127) value[40]=127; if(value[40]<0) value[40]=0; value[40];}
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[41]=map(p.y,3600,1800,0,127); if(value[41]>127) value[41]=127; if(value[41]<0) value[41]=0; DCFATTACK=value[26]=value[41];}
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[42]=map(p.y,3600,1800,0,127); if(value[42]>127) value[42]=127; if(value[42]<0) value[42]=0; DCFDECAY=value[27]=value[42];}
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[43]=map(p.y,3600,1800,0,127); if(value[43]>127) value[43]=127; if(value[43]<0) value[43]=0; if (value[43]<127) DCFSUSTAIN=((value[43]+1)<<9); else DCFSUSTAIN=65535; value[28]=value[43];} 
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[44]=map(p.y,3600,1800,0,127); if(value[44]>127) value[44]=127; if(value[44]<0) value[44]=0; value[44];}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[45]=map(p.y,3600,1800,0,127); if(value[45]>127) value[45]=127; if(value[45]<0) value[45]=0; value[45];}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[46]=map(p.y,3600,1800,0,127); if(value[46]>127) value[46]=127; if(value[46]<0) value[46]=0; value[46];}
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[47]=map(p.y,3600,1800,0,127); if(value[47]>127) value[47]=127; if(value[47]<0) value[47]=0; value[47];}
                break;
    
              case 4:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[48]=map(p.y,3600,1800,0,127); if(value[48]>127) value[48]=127; if(value[48]<0) value[48]=0; LFO1WAVE=value[48]>>5;}
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[49]=map(p.y,3600,1800,0,127); if(value[49]>127) value[49]=127; if(value[49]<0) value[49]=0; LFO1SPEED=value[49];} 
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[50]=map(p.y,3600,1800,0,127); if(value[50]>127) value[50]=127; if(value[50]<0) value[50]=0; LFO1DEPHT=value[50];}
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[51]=map(p.y,3600,1800,0,127); if(value[51]>127) value[51]=127; if(value[51]<0) value[51]=0; LFO1SYNC=value[51];} 
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) {value[52]=map(p.y,3600,1800,0,127); if(value[52]>127) value[52]=127; if(value[52]<0) value[52]=0; LFO2WAVE=value[52]>>5;}
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[53]=map(p.y,3600,1800,0,127); if(value[53]>127) value[53]=127; if(value[53]<0) value[53]=0; LFO2SPEED=value[53];}
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[54]=map(p.y,3600,1800,0,127); if(value[54]>127) value[54]=127; if(value[54]<0) value[54]=0; LFO2DEPHT=value[54];}
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[55]=map(p.y,3600,1800,0,127); if(value[55]>127) value[55]=127; if(value[55]<0) value[55]=0; LFO2SYNC=value[55];} 
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[56]=map(p.y,3600,1800,0,127); if(value[56]>127) value[56]=127; if(value[56]<0) value[56]=0; LFO3WAVE=value[56]>>5;}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[57]=map(p.y,3600,1800,0,127); if(value[57]>127) value[57]=127; if(value[57]<0) value[57]=0; LFO3SPEED=value[57];}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[58]=map(p.y,3600,1800,0,127); if(value[58]>127) value[58]=127; if(value[58]<0) value[58]=0; LFO3DEPHT=value[58];}
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[59]=map(p.y,3600,1800,0,127); if(value[59]>127) value[59]=127; if(value[59]<0) value[59]=0; LFO3SYNC=value[59];}
                break;  
    
              case 5:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[60]=map(p.y,3600,1800,0,127); if(value[60]>127) value[60]=127; if(value[60]<0) value[60]=0; wet=float((value[60]/127.0f)*2.0f);}
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[61]=map(p.y,3600,1800,0,127); if(value[61]>127) value[61]=127; if(value[61]<0) value[61]=0;
                   revtime=float((value[61]+5)/132.0f);
                   cf0_p=0, cf1_p=0, cf2_p=0, cf3_p=0, ap0_p=0, ap1_p=0, ap2_p=0;
                   cf0_lim = (int)(revtime*l_CB0);
                   cf1_lim = (int)(revtime*l_CB1);
                   cf2_lim = (int)(revtime*l_CB2);
                   cf3_lim = (int)(revtime*l_CB3);
                   ap0_lim = (int)(revtime*l_AP0);
                   ap1_lim = (int)(revtime*l_AP1);
                   ap2_lim = (int)(revtime*l_AP2);}
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[62]=map(p.y,3600,1800,0,127); if(value[62]>127) value[62]=127; if(value[62]<0) value[62]=0; Delay_SetLevel_l(value[62]*516);} 
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[63]=map(p.y,3600,1800,0,127); if(value[63]>127) value[63]=127; if(value[63]<0) value[63]=0; Delay_SetLevel_r(value[63]*516);} 
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) {value[64]=map(p.y,3600,1800,0,127); if(value[64]>127) value[64]=127; if(value[64]<0) value[64]=0; Delay_SetFeedback_l(value[64]*516);}
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[65]=map(p.y,3600,1800,0,127); if(value[65]>127) value[65]=127; if(value[65]<0) value[65]=0; Delay_SetFeedback_r(value[65]*516);}
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[66]=map(p.y,3600,1800,0,127); if(value[66]>127) value[66]=127; if(value[66]<0) value[66]=0; StereoEFX=value[66];}
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[67]=map(p.y,3600,1800,0,127); if(value[67]>127) value[67]=127; if(value[67]<0) value[67]=0; Delay_SetLength_l(value[67]);} 
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[68]=map(p.y,3600,1800,0,127); if(value[68]>127) value[68]=127; if(value[68]<0) value[68]=0; Delay_SetLength_r(value[68]);}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[69]=map(p.y,3600,1800,0,127); if(value[69]>127) value[69]=127; if(value[69]<0) value[69]=0; Struct_EFX=value[69];}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[70]=map(p.y,3600,1800,0,127); if(value[70]>127) value[70]=127; if(value[70]<0) value[70]=0; EFXMIDICLKDIV=value[70]; if(value[70]==0) EFXMIDICLKDIV_set2null=true;}
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[71]=map(p.y,3600,1800,0,127); if(value[71]>127) value[71]=127; if(value[71]<0) value[71]=0; EFX_Wide_Stereo=value[71];}
                break;
    
              case 6:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[72]=map(p.y,3600,1800,0,127); if(value[72]>127) value[72]=127; if(value[72]<0) value[72]=0; ChangeArpMode();}  //ARP MODE
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[73]=map(p.y,3600,1800,0,127); if(value[73]>127) value[73]=127; if(value[73]<0) value[73]=0;} //ARP SPEED 
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[74]=map(p.y,3600,1800,0,127); if(value[74]>127) value[74]=127; if(value[74]<0) value[74]=0;} //ARP VEL
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[75]=map(p.y,3600,1800,0,127); if(value[75]>127) value[75]=127; if(value[75]<0) value[75]=0; if(value[75]<10) clockSync=false; else clockSync=true;} //ARP CLK 
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) {value[76]=map(p.y,3600,1800,0,127); if(value[76]>127) value[76]=127; if(value[76]<0) value[76]=0; if(value[76]<10) {hold=false; noteStatus[0]=0;} else hold=true; if(value[76]>63) holdTrans=true; else holdTrans=false;} //ARP HOLD
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[77]=map(p.y,3600,1800,0,127); if(value[77]>127) value[77]=127; if(value[77]<0) value[77]=0;
                    if(value[77]<25) octave=-2;
                    else if(value[77]>=25&&value[77]<50) octave=-1; 
                    else if(value[77]>=50&&value[77]<75) octave=0; 
                    else if(value[77]>=75&&value[77]<100) octave=1;
                    else if(value[77]>=100) octave=2; 
                    }
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[78]=map(p.y,3600,1800,0,127); if(value[78]>127) value[78]=127; if(value[78]<0) value[78]=0;} //Arp Gate
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[79]=map(p.y,3600,1800,0,127); if(value[79]>127) value[79]=127; if(value[79]<0) value[79]=0; ArpSwitch=value[79]>>4;}  //Arp Notes (on/off)
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[80]=map(p.y,3600,1800,0,127); if(value[80]>127) value[80]=127; if(value[80]<0) value[80]=0;}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[81]=map(p.y,3600,1800,0,127); if(value[81]>127) value[81]=127; if(value[81]<0) value[81]=0;}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[82]=map(p.y,3600,1800,0,127); if(value[82]>127) value[82]=127; if(value[82]<0) value[82]=0;} 
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[83]=map(p.y,3600,1800,0,127); if(value[83]>127) value[83]=127; if(value[83]<0) value[83]=0;}
                break; 

              case 7:
                if((p.x>200&&p.x<400)&&(p.y>1600&&p.y<3610)) {value[84]=map(p.y,3600,1800,0,127); if(value[84]>127) value[84]=127; if(value[84]<0) value[84]=0; ChannelNote=value[84]>>3;}
                if((p.x>500&&p.x<700)&&(p.y>1600&&p.y<3610)) {value[85]=map(p.y,3600,1800,0,127); if(value[85]>127) value[85]=127; if(value[85]<0) value[85]=0; ChannelCC=value[85]>>3;} 
                if((p.x>800&&p.x<1000)&&(p.y>1600&&p.y<3610)) {value[86]=map(p.y,3600,1800,0,127); if(value[86]>127) value[86]=127; if(value[86]<0) value[86]=0; if(value[86]==0) CCon=false; else CCon=true;}
                if((p.x>1100&&p.x<1300)&&(p.y>1600&&p.y<3610)) {value[87]=map(p.y,3600,1800,0,127); if(value[87]>127) value[87]=127; if(value[87]<0) value[87]=0; Wheel_Target=value[87]/30; if (Wheel_Target>4) Wheel_Target=4;} 
                if((p.x>1400&&p.x<1600)&&(p.y>1600&&p.y<3610)) 
                   {
                    value[88]=map(p.y,3600,1800,0,127); if(value[88]>127) value[88]=127; if(value[88]<0) value[88]=0; 
                    Aftertouch_Target=value[88]>>5;
                    if(Aftertouch_Target==0){FREQUENZ=FREQUENZ_save; q=q_save;}
                    if(Aftertouch_Target==1){q=q_save;}
                    if(Aftertouch_Target==2){FREQUENZ=FREQUENZ_save;}
                    if(Aftertouch_Target==3){FREQUENZ=FREQUENZ_save; q=q_save;}
                    }
                if((p.x>1700&&p.x<1900)&&(p.y>1600&&p.y<3610)) {value[89]=map(p.y,3600,1800,0,127); if(value[89]>127) value[89]=127; if(value[89]<0) value[89]=0;}
                if((p.x>2000&&p.x<2200)&&(p.y>1600&&p.y<3610)) {value[90]=map(p.y,3600,1800,0,127); if(value[90]>127) value[90]=127; if(value[90]<0) value[90]=0; chorusToMix_l=value[90]*516; chorusToMix_r=value[90]*516;}
                if((p.x>2300&&p.x<2500)&&(p.y>1600&&p.y<3610)) {value[91]=map(p.y,3600,1800,0,127); if(value[91]>127) value[91]=127; if(value[91]<0) value[91]=0; LFO4SPEED=value[91];}
                if((p.x>2600&&p.x<2800)&&(p.y>1600&&p.y<3610)) {value[92]=map(p.y,3600,1800,0,127); if(value[92]>127) value[92]=127; if(value[92]<0) value[92]=0; LFO4DEPHT=value[92];}
                if((p.x>2900&&p.x<3100)&&(p.y>1600&&p.y<3610)) {value[93]=map(p.y,3600,1800,0,127); if(value[93]>127) value[93]=127; if(value[93]<0) value[93]=0; chorusToMix2_l=value[93]*516; chorusToMix2_r=value[93]*516;}
                if((p.x>3200&&p.x<3400)&&(p.y>1600&&p.y<3610)) {value[94]=map(p.y,3600,1800,0,127); if(value[94]>127) value[94]=127; if(value[94]<0) value[94]=0; LFO5SPEED=value[94];} 
                if((p.x>3500&&p.x<3700)&&(p.y>1600&&p.y<3610)) {value[95]=map(p.y,3600,1800,0,127); if(value[95]>127) value[95]=127; if(value[95]<0) value[95]=0; LFO5DEPHT=value[95];}
                break;                                                             
              default:
                break;                 
              }
    
            if(p.x>250&&p.x<600) //Buttons 1-3 
              {
               if(p.y>400&&p.y<700) {button1_pressed=true; button_millis=millis();}
               if(p.y>800&&p.y<1100) {button2_pressed=true; button_millis=millis();}
               if(p.y>1200&&p.y<1500) {button3_pressed=true; button_millis=millis();}
              } 
              
            if(p.x>3350&&p.x<3700) //Buttons 4-6
              {
               if(p.y>300&&p.y<600) {button4_pressed=true; button_millis=millis();}
               if(p.y>800&&p.y<1100) {button5_pressed=true; button_millis=millis();}
               if(p.y>1300&&p.y<1600) {button6_pressed=true; button_millis=millis();}
              }
    
            if(p.x>900&&p.x<1800&&p.y>500&&p.y<1500&&scope2_fullscreen==false) //Scope 1
              {
               scope1_fullscreen=true;
               scope1_full();
              }
              
            if(p.x>2200&&p.x<3000&&p.y>500&&p.y<1500&&scope1_fullscreen==false) //Scope 2
              {
               scope2_fullscreen=true;
               scope2_full();
              }

            if(p.y>3700&&p.y<4000) //Buttons 1-12 (Position der Values unter den Slidern) 
              {
               for (uint8_t i=0; i<12; i++)
                  {
                  if (p.x>(250+(i*300)) && p.x<(350+(i*300)))
                     {
                      Pressed_ValButton=i;
                      ValButton_pressed=true; 
                      button_millis=millis();
                     }
                  }
              }
                                          
           }  //IRQ Standard Ende
         } //Touch Ende
     }  //Touch IRQ Ende  
    
    if(scope1_fullscreen==true) //Loop Scope 1
      {
        if(scbutton1_pressed==true) 
          {
           if(millis()-button_millis>100)
             {
              scbutton1_pressed=false;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                } 
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                } 
              Save_Wavetable();
             }   
           }        
        if(scbutton2_pressed==true) 
          {
           if(millis()-button_millis>100)
             {
              scbutton2_pressed=false;
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                }          
              switch (scbutton2_pressed_state)
                { 
                 case 0:
                 scbutton2_pressed_state++;
                 SDCARDWAVE1=0;
                 tft.fillRect(10, 86, 59, 68, TFT_YELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_RED,TFT_YELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1"));                 
                 scope1_delta();
                 break;
                 case 1:
                 scbutton2_pressed_state++;
                 SDCARDWAVE1=127;
                 scope1_delta();
                 break;
                 case 2:
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1"));                 
                 Create_Wavetable1();
                 break;
                } 
             }   
           }
        if(scbutton3_pressed==true) 
          {
           if(millis()-button_millis>100)
             {
              scbutton3_pressed=false;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                }              
              switch (scbutton3_pressed_state)
                { 
                 case 0:
                 scbutton3_pressed_state++;
                 SDCARDWAVE1=0;  
                 tft.fillRect(10, 162, 59, 68, TFT_YELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_RED,TFT_YELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                 scope1_delta();
                 break;
                 case 1:
                 scbutton3_pressed_state++;
                 SDCARDWAVE1=127;
                 scope1_delta();
                 break;
                 case 2:
                 scbutton3_pressed_state=0; 
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2"));                  
                 Create2_Wavetable1();
                 break;
                } 
             }   
           }           
        if(scbutton5_pressed==true) 
          {
           tft.fillRect(411, 10, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton5_pressed=false;
              N_Balken=1; 
              tft.fillRect(411, 10, 59, 68, TFT_LTGREEN);          
              tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREY);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);  
              tft.setCursor(433,34);
              tft.print(F("1")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));    
              }
           }
        if(scbutton6_pressed==true) 
          {
           tft.fillRect(411, 86, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton6_pressed=false;
              N_Balken=2;         
              tft.fillRect(411, 10, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 86, 59, 68, TFT_LTGREEN);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREY);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);  
              tft.setCursor(433,34);
              tft.print(F("1")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);  
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));                
              }
           }     
        if(scbutton7_pressed==true) 
          {
           tft.fillRect(411, 162, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton7_pressed=false;
              N_Balken=4;        
              tft.fillRect(411, 10, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREEN);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREY);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);  
              tft.setCursor(433,34);
              tft.print(F("1"));   
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));                 
              }
           }
        if(scbutton8_pressed==true) 
          {
           tft.fillRect(411, 238, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton8_pressed=false;
              N_Balken=8; 
              tft.fillRect(411, 10, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREEN);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);  
              tft.setCursor(433,34);
              tft.print(F("1"));   
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));            
             }
           }
        if(leftarrow_pressed==true) 
          { 
           if(millis()-button_millis>100)
             {
              leftarrow_pressed=false;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                } 
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                } 
              if(SDCARDWAVE1>0)SDCARDWAVE1--; else SDCARDWAVE1=127;
              scope1_delta();              
             }
           }
        if(rightarrow_pressed==true) 
          { 
           if(millis()-button_millis>100)
             {
              rightarrow_pressed=false;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                } 
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                } 
              if(SDCARDWAVE1<126)SDCARDWAVE1++; else SDCARDWAVE1=0;
              scope1_delta(); 
             }
           }
       if (Sample1Changed==true && SAMPLELEN1!=0 && VOL1>0) 
          {
           scope1_delta();
          }
       if (WavetableChanged==true) 
          {
           if(millis()-WavetableChanged_millis>25)
             {
              LoadSample();
              scope1_delta();
              WavetableChanged=false;
             } 
          }
      } //Loop Scope 1 Ende
      
    else if(scope2_fullscreen==true)  //Loop Scope 2
      {
        if(scbutton1_pressed==true) 
          {
           if(millis()-button_millis>100)
             {
              scbutton1_pressed=false;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                } 
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                }               
              Save_Wavetable();
             }   
           }  
           
        if(scbutton2_pressed==true) 
          {
           if(millis()-button_millis>100)
             {
              scbutton2_pressed=false;
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                }          
              switch (scbutton2_pressed_state)
                { 
                 case 0:
                 scbutton2_pressed_state++;
                 SDCARDWAVE2=0;
                 tft.fillRect(10, 86, 59, 68, TFT_YELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_RED,TFT_YELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1"));                 
                 scope2_delta();
                 break;
                 case 1:
                 scbutton2_pressed_state++;
                 SDCARDWAVE2=127;
                 scope2_delta();
                 break;
                 case 2:
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1"));                 
                 Create_Wavetable2();
                 break;
                } 
             }
          }     
             
        if(scbutton3_pressed==true) 
          {
           if(millis()-button_millis>100)
             {
              scbutton3_pressed=false;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                }              
              switch (scbutton3_pressed_state)
                { 
                 case 0:
                 scbutton3_pressed_state++;
                 SDCARDWAVE2=0;  
                 tft.fillRect(10, 162, 59, 68, TFT_YELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_RED,TFT_YELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                 scope2_delta();
                 break;
                 case 1:
                 scbutton3_pressed_state++;
                 SDCARDWAVE2=127;
                 scope2_delta();
                 break;
                 case 2:
                 scbutton3_pressed_state=0; 
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2"));                  
                 Create2_Wavetable2();
                 break;
                } 
             }   
           }       
                            
        if(scbutton5_pressed==true) 
          {
           tft.fillRect(411, 10, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton5_pressed=false;
              N_Balken=1; 
              tft.fillRect(411, 10, 59, 68, TFT_LTGREEN);          
              tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREY);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);  
              tft.setCursor(433,34);
              tft.print(F("1")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));    
              }
           }
        if(scbutton6_pressed==true) 
          {
           tft.fillRect(411, 86, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton6_pressed=false;
              N_Balken=2;         
              tft.fillRect(411, 10, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 86, 59, 68, TFT_LTGREEN);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREY);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);  
              tft.setCursor(433,34);
              tft.print(F("1")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);  
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));                
              }
           }     
        if(scbutton7_pressed==true) 
          {
           tft.fillRect(411, 162, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton7_pressed=false;
              N_Balken=4;        
              tft.fillRect(411, 10, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREEN);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREY);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);  
              tft.setCursor(433,34);
              tft.print(F("1"));   
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));                 
              }
           }
        if(scbutton8_pressed==true) 
          {
           tft.fillRect(411, 238, 59, 68, TFT_LTGREY); 
           if(millis()-button_millis>100)
             {
              scbutton8_pressed=false;
              N_Balken=8; 
              tft.fillRect(411, 10, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
              tft.fillRect(411, 238, 59, 68, TFT_LTGREEN);
              tft.setTextSize(3);
              tft.setTextColor(TFT_BLACK,TFT_LTGREY);  
              tft.setCursor(433,34);
              tft.print(F("1"));   
              tft.setCursor(433,110);
              tft.print(F("2")); 
              tft.setCursor(433,95+38+38+15);
              tft.print(F("4")); 
              tft.setTextColor(TFT_BLACK,TFT_LTGREEN);
              tft.setCursor(433,95+38+38+38+38+15);
              tft.print(F("8"));            
             }
           }
        if(leftarrow_pressed==true) 
          { 
           if(millis()-button_millis>100)
             {
              leftarrow_pressed=false;
              if(SDCARDWAVE2>0)SDCARDWAVE2--; else SDCARDWAVE2=127;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                } 
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                }                
              Sample2Changed=true; 
              scope2_delta();            
             }
           }
        if(rightarrow_pressed==true) 
          { 
           if(millis()-button_millis>100)
             {
              rightarrow_pressed=false;
              if(SDCARDWAVE2<126)SDCARDWAVE2++; else SDCARDWAVE2=0;
              if(scbutton2_pressed_state>0)
                { 
                 scbutton2_pressed_state=0;
                 tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,110);
                 tft.print(F("CRE1")); 
                } 
              if(scbutton3_pressed_state>0)
                {
                 scbutton3_pressed_state=0;
                 tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
                 tft.setTextSize(2);
                 tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
                 tft.setCursor(17,95+38+38+15);
                 tft.print(F("CRE2")); 
                }                
              scope2_delta();
             }
           }
       if (Sample2Changed==true && SAMPLELEN2!=0 && VOL2>0) 
          {
           scope2_delta();
          }                       
       if (WavetableChanged==true) 
          {
           if(millis()-WavetableChanged_millis>25)
             {
              //WavetableChanged=false;
              LoadSample();
              scope2_delta();
              WavetableChanged=false;
             } 
          }
      } //Loop Scope 2 Ende
      
    else
      { //Loop Standard
        plotPointer(menu);

        if(Program_Changed==true) //wenn ein Midi ProgChange angekommen ist 
          {
            Program_Changed=false;
            LFO3DEPHT=value[58]=0; LFO3=0; 
            mBstring = "/" + String(PROGRAM, DEC) + ".sys";
            tft.setTextSize(2);
            if (SD.exists(mBstring)) 
                {
                 Serial.println("File vorhanden"); 
                 tft.setTextColor(TFT_BLACK,TFT_LTORANGE);
                 tft.setCursor(17,19);
                 tft.print("P"); 
                 tft.print((PROGRAM / 100) % 10);
                 tft.print((PROGRAM / 10) % 10);
                 tft.print(PROGRAM % 10); 
                 tft.setCursor(416,19); 
                 tft.print("P");  
                 tft.print((PROGRAM / 100) % 10);
                 tft.print((PROGRAM / 10) % 10);
                 tft.print(PROGRAM % 10);  
                 tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                 tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                 tft.setCursor(17,57);
                 tft.print(F("LOAD"));
                 mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                 File SDdataFile = SD.open(mBstring, FILE_READ);
                 while (SDdataFile.available()) SDdataFile.read(value, anzahl_parameter);
                 SDdataFile.close();
                 LoadParameter();
                 tft.setTextSize(2);
                 ShowMenu();
                 save_preferences_millis=millis();
                 save_Program_changed=true; 
                }
              else 
                {
                 Serial.println("kein File vorhanden"); 
                 tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                 tft.setCursor(17,19);
                 tft.print("P"); 
                 tft.print((PROGRAM / 100) % 10);
                 tft.print((PROGRAM / 10) % 10);
                 tft.print(PROGRAM % 10); 
                 tft.setCursor(416,19); 
                 tft.print("P");  
                 tft.print((PROGRAM / 100) % 10);
                 tft.print((PROGRAM / 10) % 10);
                 tft.print(PROGRAM % 10);                 
                 tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                 tft.setTextColor(TFT_WHITE,TFT_GREEN); 
                 tft.setCursor(17,57);
                 tft.print(F("LOAD"));  
                }  
          }      

        if(button1_pressed==true) //Program down
          {
           if(millis()-button_millis>100)
             {
              button1_pressed=false;
              if(button5_pressed_second==true) //wenn SAVE gedrückt und dann Program up
                {
                  tft.fillRect(10, 8, 61, 34, TFT_LTORANGE);                   
                  if(PROGRAM>0) PROGRAM--;
                  else PROGRAM=0;
                  mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                  tft.setTextSize(2);
                  if (SD.exists(mBstring)) 
                        {
                         Serial.println("File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE);
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10);
                         tft.fillRect(409, 46, 61, 34, TFT_RED); 
                         tft.setTextColor(TFT_BLACK,TFT_RED); 
                         tft.setCursor(416,57);
                         tft.print(F("SAVE")); 
                         save_back_millis=millis();                       
                        }
                  else 
                        {
                         Serial.println("kein File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10);                 
                         tft.fillRect(409, 46, 61, 34, TFT_RED); 
                         tft.setTextColor(TFT_WHITE,TFT_RED); 
                         tft.setCursor(416,57);
                         tft.print(F("SAVE")); 
                         save_back_millis=millis();                   
                        }  
                }
              else
                {
                  tft.fillRect(10, 8, 61, 34, TFT_LTORANGE);                   
                  if(PROGRAM>0) PROGRAM--;
                  else PROGRAM=0;
                  mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                  tft.setTextSize(2);
                  if (SD.exists(mBstring)) 
                        {
                         Serial.println("File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE);
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10);  
                         tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                         tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                         tft.setCursor(17,57);
                         tft.print(F("LOAD"));                         
                        }
                  else 
                        {
                         Serial.println("kein File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10);                 
                         tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                         tft.setTextColor(TFT_WHITE,TFT_GREEN); 
                         tft.setCursor(17,57);
                         tft.print(F("LOAD"));                      
                        }  
                }
             }
          }
    
        if(button2_pressed==true) //LOAD
          { 
           if(millis()-button_millis>100)
              {
               button2_pressed=false;            
               if(button5_pressed_second==true) //wenn SAVE gedrückt und dann LOAD
                  {
                   button5_pressed_second=false;
                   tft.setTextSize(2);
                   tft.fillRect(409, 46, 61, 34, TFT_LTRED); 
                   tft.setTextColor(TFT_BLACK,TFT_LTRED); 
                   tft.setCursor(416,57);
                   tft.print(F("SAVE"));
                  } 
                else
                  {
                    LFO3DEPHT=value[58]=0; LFO3=0; 
                    tft.fillRect(10, 46, 61, 34, TFT_GREEN); 
                    tft.fillRect(10, 46, 61, 34, TFT_LTGREY);
                    mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                    File SDdataFile = SD.open(mBstring, FILE_READ);
                    if(SDdataFile==true) {Serial.print("Loading SD CARD File: ");}
                    while (SDdataFile.available()) SDdataFile.read(value, anzahl_parameter);
                    SDdataFile.close();
                    Serial.print("SD CARD File loaded: ");
                    Serial.println(mBstring);
                    tft.setTextSize(2);
                    if (SD.exists(mBstring)) 
                       {
                        Serial.println("File vorhanden"); 
                        tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                        tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                        tft.setCursor(17,57);
                        tft.print(F("LOAD"));     
                       }
                    else
                       {
                        Serial.println("File nicht vorhanden"); 
                        tft.setTextColor(TFT_BLACK,TFT_LTGREY); 
                        tft.setCursor(17,57);
                        tft.print(F("FAIL"));                    
                       }
                 LoadParameter();
                 ShowMenu();
                 save_preferences_millis=millis();  //Speichern von PROGRAM in Preferences
                 save_Program_changed=true;   
                }     
          }
        } 
               
        if(button3_pressed==true) //MENU DOWN
          {
           if(millis()-button_millis>100)
             {
              button3_pressed=false;            
              if(button5_pressed_second==true) //wenn SAVE gedrückt und dann MENU DOWN
                {
                 button5_pressed_second=false;
                 tft.setTextSize(2);
                 tft.fillRect(409, 46, 61, 34, TFT_LTRED); 
                 tft.setTextColor(TFT_BLACK,TFT_LTRED); 
                 tft.setCursor(416,57);
                 tft.print(F("SAVE"));
                }
              else
                { 
                 tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);   
                 if (menu>0) 
                    {
                     menu--;     
                     MenuChanged=true;
                    }      
                 else 
                    {
                     menu=anzahl_menu-1; 
                     MenuChanged=true;
                    }             
                }
             }
          } 
                
        if(button4_pressed==true) //PROGRAM UP
          { 
           if(millis()-button_millis>100)
             {
              button4_pressed=false;
              if(button5_pressed_second==true) //wenn SAVE gedrückt und dann PROGRAM UP
                {
                  tft.fillRect(409, 8, 61, 34, TFT_LTORANGE);
                  if(PROGRAM<127) PROGRAM++;
                  else PROGRAM=127;
                  mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                  tft.setTextSize(2);
                  if (SD.exists(mBstring)) 
                        {
                         Serial.println("File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10);
                         tft.fillRect(409, 46, 61, 34, TFT_RED); 
                         tft.setTextColor(TFT_BLACK,TFT_RED); 
                         tft.setCursor(416,57);
                         tft.print(F("SAVE")); 
                         save_back_millis=millis();                       
                        }
                  else 
                        {
                         Serial.println("kein File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10);              
                         tft.fillRect(409, 46, 61, 34, TFT_RED); 
                         tft.setTextColor(TFT_WHITE,TFT_RED); 
                         tft.setCursor(416,57);
                         tft.print(F("SAVE")); 
                         save_back_millis=millis();                   
                        }                    
                } 
              else
                {
                  tft.fillRect(409, 8, 61, 34, TFT_LTORANGE);
                  if(PROGRAM<127) PROGRAM++;
                  else PROGRAM=127;
                  mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                  tft.setTextSize(2);
                  if (SD.exists(mBstring)) 
                        {
                         Serial.println("File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                         tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                         tft.setCursor(17,57);
                         tft.print(F("LOAD"));                     
                        }
                  else 
                        {
                         Serial.println("kein File vorhanden"); 
                         tft.setTextColor(TFT_BLACK,TFT_LTORANGE); 
                         tft.setCursor(17,19);
                         tft.print("P"); 
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.setCursor(416,19); 
                         tft.print("P");  
                         tft.print((PROGRAM / 100) % 10);
                         tft.print((PROGRAM / 10) % 10);
                         tft.print(PROGRAM % 10); 
                         tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                         tft.setTextColor(TFT_WHITE,TFT_GREEN); 
                         tft.setCursor(17,57);
                         tft.print(F("LOAD"));                       
                        }
                }
             }
          }
    
        if(button5_pressed==true) //SAVE
          { 
           tft.fillRect(409, 46, 61, 34, TFT_RED); 
           if(millis()-button_millis>100)
             {
              button5_pressed=false;
              if(button5_pressed_second==true) //SAVE zum 2ten mal gedrückt?
                {
                 button5_pressed_second=false;
                 tft.fillRect(409, 46, 61, 34, TFT_LTGREY); 
                 mBstring = "/" + String(PROGRAM, DEC) + ".sys";
                 File SDdataFile = SD.open(mBstring, FILE_WRITE);
                 if(SDdataFile==true) {Serial.print("Writing SD CARD File: ");}
                 SDdataFile.write(value, anzahl_parameter);
                 SDdataFile.close();
                 Serial.print("File written: ");
                 tft.setTextSize(2);
                 if (SD.exists(mBstring)) 
                    {
                     Serial.println("File vorhanden"); 
                     tft.fillRect(409, 46, 61, 34, TFT_LTRED); 
                     tft.setTextColor(TFT_BLACK,TFT_LTRED); 
                     tft.setCursor(416,57);
                     tft.print(F("SAVE"));
                     tft.fillRect(10, 46, 61, 34, TFT_GREEN);
                     tft.setTextColor(TFT_BLACK,TFT_GREEN); 
                     tft.setCursor(17,57);
                     tft.print(F("LOAD"));  
                    }
                 else 
                    {
                     Serial.println("kein File vorhanden"); 
                     tft.setTextColor(TFT_BLACK,TFT_LTGREY); 
                     tft.setCursor(416,57);
                     tft.print(F("FAIL"));
                    }
                 save_preferences_millis=millis();  //Speichern von PROGRAM in Preferences
                 save_Program_changed=true;
                }
              else //SAVE zum ersten mal gedrückt
                {
                 button5_pressed_second=true;
                 tft.setTextSize(2);
                 tft.fillRect(409, 46, 61, 34, TFT_RED); 
                 tft.setTextColor(TFT_BLACK,TFT_RED); 
                 tft.setCursor(416,57);
                 tft.print(F("SAVE"));
                 save_back_millis=millis();
                }
              }                              
          }

        if(button6_pressed==true) //MENU UP
          {
           if(millis()-button_millis>100)
             {
              button6_pressed=false;            
              if(button5_pressed_second==true) //wenn SAVE gedrückt und dann MENU DOWN
                {
                 button5_pressed_second=false;
                 tft.setTextSize(2);
                 tft.fillRect(409, 46, 61, 34, TFT_LTRED); 
                 tft.setTextColor(TFT_BLACK,TFT_LTRED); 
                 tft.setCursor(416,57);
                 tft.print(F("SAVE"));
                }
              else
                { 
                 tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);   
                 if (menu<anzahl_menu-1) 
                    {
                     menu++;     
                     MenuChanged=true;
                    }      
                 else 
                    {
                     menu=0; 
                     MenuChanged=true;
                    }             
                }
             }
          } 

       if (ValButton_pressed==true) //Buttons 1-12 (Position der Values unter den Slidern)
          {
           if(millis()-button_millis>100)
             {
              ValButton_pressed=false;
              uint8_t i=menu*12 + Pressed_ValButton;
              if (value[i]>0) value[i]--;
             }      
          }
            
       if (Sample1Changed==true && SAMPLELEN1!=0 && VOL1>0) ShowSample_1();
       if (Sample2Changed==true && SAMPLELEN2!=0 && VOL2>0) ShowSample_2();     
       if (WavetableChanged==true) 
          {
           if(millis()-WavetableChanged_millis>25)
             {
              LoadSample();
              ShowSample_1(); 
              ShowSample_2();
              WavetableChanged=false;
             }
           } 
       if (MenuChanged==true) ShowMenu();
      } //Loop Standard Ende       
   } //Loop for(;;) Ende  
} 

void ShowMenu()
  {
   MenuChanged=false;

   switch (menu)
     {
      case 0:
      d = 40;
      plotLinear(" DIR ", 0, 130);
      plotLinear(" WAV ", 1 * d, 130);  
      plotLinear(" ST1 ", 2 * d, 130);
      plotLinear(" LP1 ", 3 * d, 130);
      plotLinear(" SO1 ", 4 * d, 130);
      plotLinear(" SE1 ", 5 * d, 130);
      plotLinear(" ST2 ", 6 * d, 130);
      plotLinear(" LP2 ", 7 * d, 130);
      plotLinear(" SO2 ", 8 * d, 130);
      plotLinear(" SE2 ", 9 * d, 130);
      plotLinear(" VL1 ", 10 * d, 130);
      plotLinear(" VL2 ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;             
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("OPT")); 
      tft.setCursor(416,95);
      tft.print(F("DCO2")); 
      break;

      case 1:
      d = 40;
      plotLinear(" PT1 ", 0, 130);
      plotLinear(" PT2 ", 1 * d, 130);
      plotLinear(" FP1 ", 2 * d, 130);
      plotLinear(" FP2 ", 3 * d, 130);
      plotLinear(" GLI ", 4 * d, 130);
      plotLinear(" VEL ", 5 * d, 130);
      plotLinear(" NOI ", 6 * d, 130);
      plotLinear(" GA1 ", 7 * d, 130);
      plotLinear(" GA2 ", 8 * d, 130);
      plotLinear(" LPF ", 9 * d, 130);
      plotLinear("  Q  ", 10 * d, 130);
      plotLinear(" SAT ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("DCO1")); 
      tft.setCursor(416,95);
      tft.print(F("DCF"));  
      break;

      case 2:
      d = 40;
      plotLinear(" CUT ", 0, 130);
      plotLinear(" RES ", 1 * d, 130);
      plotLinear("  A  ", 2 * d, 130);
      plotLinear("  D  ", 3 * d, 130);
      plotLinear("  S  ", 4 * d, 130);
      plotLinear("  R  ", 5 * d, 130);
      plotLinear(" DEP ", 6 * d, 130);
      plotLinear(" FLS ", 7 * d, 130);
      plotLinear(" GLS ", 8 * d, 130);
      plotLinear(" FHS ", 9 * d, 130);
      plotLinear(" GHS ", 10 * d, 130);
      plotLinear(" KFO ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("DCO2")); 
      tft.setCursor(416,95);
      tft.print(F("DCA"));       
      break;

      case 3:
      d = 40;
      plotLinear("  A  ", 0, 130);
      plotLinear("  D  ", 1 * d, 130);
      plotLinear("  S  ", 2 * d, 130);
      plotLinear("  R  ", 3 * d, 130);
      plotLinear(" DEP ", 4 * d, 130);
      plotLinear("  A  ", 5 * d, 130);
      plotLinear("  D  ", 6 * d, 130);
      plotLinear("  S  ", 7 * d, 130);
      plotLinear("  R  ", 8 * d, 130);
      plotLinear(" DEP ", 9 * d, 130);
      plotLinear("     ", 10 * d, 130);
      plotLinear("     ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("DCF")); 
      tft.setCursor(416,95);
      tft.print(F("LFO"));       
      break;             

      case 4:
      d = 40;
      plotLinear(" WV1 ", 0, 130);
      plotLinear(" SP1 ", 1 * d, 130);
      plotLinear(" DP1 ", 2 * d, 130);
      plotLinear(" SY1 ", 3 * d, 130);
      plotLinear(" WV2 ", 4 * d, 130);
      plotLinear(" SP2 ", 5 * d, 130);
      plotLinear(" DP2 ", 6 * d, 130);
      plotLinear(" SY2 ", 7 * d, 130);
      plotLinear(" WV3 ", 8 * d, 130);
      plotLinear(" SP3 ", 9 * d, 130);
      plotLinear(" DP3 ", 10 * d, 130);
      plotLinear(" SY3 ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("DCA")); 
      tft.setCursor(416,95);
      tft.print(F("EFX"));       
      break;

      case 5:
      d = 40;
      plotLinear(" MIX ", 0, 130);
      plotLinear(" REV ", 1 * d, 130);
      plotLinear(" VAL ", 2 * d, 130);
      plotLinear(" VAL ", 3 * d, 130);
      plotLinear(" REP ", 4 * d, 130);
      plotLinear(" REP ", 5 * d, 130);
      plotLinear(" STO ", 6 * d, 130);
      plotLinear(" DEP ", 7 * d, 130);
      plotLinear(" DEP ", 8 * d, 130);
      plotLinear(" MOD ", 9 * d, 130);
      plotLinear(" SYN ", 10 * d, 130);
      plotLinear(" WID ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("LFO")); 
      tft.setCursor(416,95);
      tft.print(F("ARP"));       
      break; 

      case 6:
      d = 40;
      plotLinear(" ARP ", 0, 130);
      plotLinear(" SDP ", 1 * d, 130);
      plotLinear(" VEL ", 2 * d, 130);
      plotLinear(" CLK ", 3 * d, 130);
      plotLinear(" HLD ", 4 * d, 130);
      plotLinear(" OCT ", 5 * d, 130);
      plotLinear(" GTE ", 6 * d, 130);
      plotLinear(" NTE ", 7 * d, 130);
      plotLinear("     ", 8 * d, 130);
      plotLinear("     ", 9 * d, 130);
      plotLinear("     ", 10 * d, 130);
      plotLinear("     ", 11 * d, 130);
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("EFX")); 
      tft.setCursor(416,95);
      tft.print(F("OPT"));     
      break;           

      case 7:
      d = 40;
      plotLinear(" NOC ", 0, 130);
      plotLinear(" CCC ", 1 * d, 130);
      plotLinear(" CC  ", 2 * d, 130);
      plotLinear(" WHL ", 3 * d, 130);
      plotLinear(" AFT ", 4 * d, 130);
      plotLinear("     ", 5 * d, 130);
      plotLinear(" C1L ", 6 * d, 130);
      plotLinear(" C1S ", 7 * d, 130);
      plotLinear(" C1D ", 8 * d, 130);
      plotLinear(" C2L ", 9 * d, 130);
      plotLinear(" C2S ", 10 * d, 130);
      plotLinear(" C2D ", 11 * d, 130);
      
      for (byte i=0; i<anzahl_parameter; i++) old_value[i] = -1;
      plotPointer(menu);
      tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
      tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK,TFT_LTBLUE); 
      tft.setCursor(17,95);
      tft.print(F("ARP")); 
      tft.setCursor(416,95);
      tft.print(F("DCO1"));     
      break;
      
      default:
      break;
     }               
  }

void LoadSample()
  {
   CheckSD_File(); 
   mBstring = "/" + String(SDCARDbank, DEC) + "/" + String(SDCARDval, DEC) + ".bin";
   File dataFile = SD.open(mBstring);
   while (dataFile.available()) dataFile.read(sampledata, dataFile.size());
   dataFile.close();
  }  
  
void ShowSample_1()
  {
   Sample1Changed=false;
   if(SDCARDWAVE1>127) SDCARDWAVE1=127; 
   uint16_t i=0;
   for (uint16_t j=0; j<256; j++)
      {
       samples1[i]=sampledata[j+(SDCARDWAVE1*256)];
       if (i<255) i++;
       else i=255;
      }
   for (int16_t j=0; j<256; j=j+2) 
     {
      sample_1=((255-samples1_alt[j])/3)+20;
      tft.drawLine((j/2)+96,63,(j/2)+96,sample_1, TFT_WHITE);
     }
   tft.drawLine(91, 63, 229, 63, TFT_BLACK); 
   for (int16_t j=0; j<256; j=j+2) 
     {
      sample_1=((255-samples1[j])/3)+20;
      tft.drawLine((j/2)+96,63,(j/2)+96,sample_1, TFT_ORANGE);
     }
   tft.drawLine(91, 63, 229, 63, TFT_BLACK);   
   for (int16_t j=0; j<256; j++) samples1_alt[j]=samples1[j];
     
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK,TFT_WHITE); 
    tft.setCursor(91,10);
    tft.print(F("BNK:"));
    tft.print(SDCARDbank);
    tft.print(F(" "));
    tft.print(F("SAM:"));
    tft.print(SDCARDval);
    tft.print(F(" "));  
    tft.print(F("WAV:"));
    tft.print(SDCARDWAVE1);
    tft.print(F("   "));
    if(WavetableChanged==true){
      tft.setCursor(91,110);
      tft.print(F("                    "));}    
    tft.setCursor(91,110);
    tft.print(FPSTR((char*)(string_table[SDCARDbank*128+SDCARDval])));                    
 }  
 
void ShowSample_2()
  {
   Sample2Changed=false;
   if(SDCARDWAVE2>127) SDCARDWAVE2=127; 
   uint16_t i=0;
   for (uint16_t j=0; j<256; j++)
      {
       samples2[i]=sampledata[j+(SDCARDWAVE2*256)];
       if (i<255) i++;
       else i=255;
      }
   for (int16_t j=0; j<256; j=j+2) 
     {
      sample_2=((255-samples2_alt[j])/3)+20;
      tft.drawLine((j/2)+255,63,(j/2)+255,sample_2, TFT_WHITE);
     }
   tft.drawLine(250, 63, 387, 63, TFT_BLACK); 
   for (int16_t j=0; j<256; j=j+2) 
     {
      sample_2=((255-samples2[j])/3)+20;
      tft.drawLine((j/2)+255,63,(j/2)+255,sample_2, TFT_ORANGE);
     }
   tft.drawLine(250, 63, 387, 63, TFT_BLACK);   
   for (int16_t j=0; j<256; j++) samples2_alt[j]=samples2[j];
     
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK,TFT_WHITE); 
    tft.setCursor(250,10);
    tft.print("BNK:");
    tft.print(SDCARDbank);
    tft.print(" ");
    tft.print("SAM:");
    tft.print(SDCARDval);
    tft.print(" ");  
    tft.print("WAV:");
    tft.print(SDCARDWAVE2);
    tft.print("   ");
    if(WavetableChanged==true){
      tft.setCursor(250,110);
      tft.print(F("                    "));}       
    tft.setCursor(250,110);
    tft.print(FPSTR((char*)(string_table[SDCARDbank*128+SDCARDval])));                    
 }
