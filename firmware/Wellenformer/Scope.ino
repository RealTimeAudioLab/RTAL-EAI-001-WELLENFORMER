/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Scope.ino
 *
 *  Description:
 *      Waveform, scope and sample visualization routines.
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

void scope1_delta()
{
  Sample1Changed=false;
  if(SDCARDWAVE1>127) SDCARDWAVE1=127;       
  for (uint16_t j=0; j<256; j++) samples1[j]=sampledata[j+(SDCARDWAVE1*256)];
  for (uint16_t j=0; j<256; j++) 
     {
      sample_1=(255-samples1_alt[j])+32;
      tft.drawLine(j+110,160,j+110,sample_1, TFT_WHITE);
      sample_1=(255-samples1[j])+32;
      tft.drawLine(j+110,160,j+110,sample_1, TFT_ORANGE);
     }
  tft.drawLine(100, 160, 376, 160, TFT_BLACK);
  for (uint16_t j=0; j<256; j++) samples1_alt[j]=samples1[j];   
     
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,17);
  tft.print(F("BNK:"));
  tft.print(SDCARDbank);
  tft.print(F(" "));
  tft.print(F("SAM:"));
  tft.print(SDCARDval);
  tft.print(F(" "));  
  tft.print(F("WAV:"));
  tft.print(SDCARDWAVE1);
  tft.print(F("   "));
  if(WavetableChanged==true || Wavetable_Written==true){
      tft.setCursor(110,290);
      tft.print("                      ");
      Wavetable_Written=false;}
  tft.setCursor(110,290);      
  tft.print(FPSTR((char*)(string_table[SDCARDbank*128+SDCARDval]))); 
}

void scope1_full() 
{
  tft.fillScreen(TFT_LTGREY);
  tft.fillRect(80, 10, 320, 300, TFT_WHITE);
  
  tft.fillRect(0, 0, 79, 320, TFT_GREY);
  tft.fillRect(5, 3, 69, 313, TFT_WHITE);
  tft.fillRect(10, 10, 59, 68, TFT_LTRED);
  tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);  
  tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
  tft.fillRect(10, 238, 59, 68, TFT_LTGREEN);   
  tft.fillRect(401, 0, 79, 320, TFT_GREY);  
  tft.fillRect(406, 3, 69, 313, TFT_WHITE);
  tft.fillRect(411, 10, 59, 68, TFT_LTGREEN);
  tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
  tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
  tft.fillRect(411, 238, 59, 68, TFT_LTGREY);

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_LTRED);
  tft.setCursor(17,34);
  tft.print(F("SAVE"));
  tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
  tft.setCursor(17,110);
  tft.print(F("CRE1"));
  tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
  tft.setCursor(17,95+38+38+15);
  tft.print(F("CRE2")); 
  tft.setCursor(17,95+38+38+38+38+15);
  tft.setTextColor(TFT_BLACK,TFT_LTGREEN);
  tft.print(F("BACK"));
                        
  tft.setTextSize(3);  
  tft.setCursor(433,34);
  tft.print(F("1")); 
  tft.setTextColor(TFT_BLACK,TFT_LTGREY);
  tft.setCursor(433,110);
  tft.print(F("2")); 
  tft.setCursor(433,95+38+38+15);
  tft.print(F("4")); 
  tft.setCursor(433,95+38+38+38+38+15);
  tft.print(F("8")); 
  N_Balken=1;  

  for (uint16_t j=0; j<256; j++) samples1[j]=sampledata[j+(SDCARDWAVE1*256)];   
  for (uint16_t j=0; j<256; j++) 
     {
      sample_1=(255-samples1[j])+32;
      tft.drawLine(j+110,160,j+110,sample_1, TFT_ORANGE);
     }
  tft.drawLine(100, 160, 376, 160, TFT_BLACK);        
       
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,17);
  tft.print(F("BNK:"));
  tft.print(SDCARDbank);
  tft.print(F(" "));
  tft.print(F("SAM:"));
  tft.print(SDCARDval);
  tft.print(F(" "));  
  tft.print(F("WAV:"));
  tft.print(SDCARDWAVE1);
  tft.print(F("   "));
  tft.setCursor(110,290);
  tft.print(FPSTR((char*)(string_table[SDCARDbank*128+SDCARDval]))); 
  tft.fillTriangle(100,140,100,180,80,160, TFT_LTGREY);
  tft.fillTriangle(380,140,376,180,400,160, TFT_LTGREY);
}

void scope2_delta()
{ 
  Sample2Changed=false;
  if(SDCARDWAVE2>127) SDCARDWAVE2=127;
  for (uint16_t j=0; j<256; j++) samples2[j]=sampledata[j+(SDCARDWAVE2*256)];
  for (uint16_t j=0; j<256; j++) 
     {
      sample_2=(255-samples2_alt[j])+32;
      tft.drawLine(j+110,160,j+110,sample_2, TFT_WHITE);
      sample_2=(255-samples2[j])+32;
      tft.drawLine(j+110,160,j+110,sample_2, TFT_ORANGE);
     }
  tft.drawLine(100, 160, 376, 160, TFT_BLACK);
  for (uint16_t j=0; j<256; j++) samples2_alt[j]=samples2[j]; 
     
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,17);
  tft.print(F("BNK:"));
  tft.print(SDCARDbank);
  tft.print(F(" "));
  tft.print(F("SAM:"));
  tft.print(SDCARDval);
  tft.print(F(" "));  
  tft.print(F("WAV:"));
  tft.print(SDCARDWAVE2);
  tft.print(F("   "));
  if(WavetableChanged==true || Wavetable_Written==true){
      tft.setCursor(110,290);
      tft.print("                      ");
      Wavetable_Written=false;} 
  tft.setCursor(110,290);
  tft.print(FPSTR((char*)(string_table[SDCARDbank*128+SDCARDval]))); 
}

void scope2_full() 
{
  tft.fillScreen(TFT_LTGREY);
  tft.fillRect(80, 10, 320, 300, TFT_WHITE);

  tft.fillRect(0, 0, 79, 320, TFT_GREY);
  tft.fillRect(5, 3, 69, 313, TFT_WHITE);
  tft.fillRect(10, 10, 59, 68, TFT_LTRED);
  tft.fillRect(10, 86, 59, 68, TFT_LTYELLOW);   
  tft.fillRect(10, 162, 59, 68, TFT_LTYELLOW);
  tft.fillRect(10, 238, 59, 68, TFT_LTGREEN);
  tft.fillRect(401, 0, 79, 320, TFT_GREY);  
  tft.fillRect(406, 3, 69, 313, TFT_WHITE);
  tft.fillRect(411, 10, 59, 68, TFT_LTGREEN);
  tft.fillRect(411, 86, 59, 68, TFT_LTGREY);
  tft.fillRect(411, 162, 59, 68, TFT_LTGREY);
  tft.fillRect(411, 238, 59, 68, TFT_LTGREY);

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_LTRED);
  tft.setCursor(17,34);
  tft.print(F("SAVE")); 
  tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
  tft.setCursor(17,110);
  tft.print(F("CRE1"));
  tft.setTextColor(TFT_BLACK,TFT_LTYELLOW); 
  tft.setCursor(17,95+38+38+15);
  tft.print(F("CRE2"));  
  tft.setCursor(17,95+38+38+38+38+15);
  tft.setTextColor(TFT_BLACK,TFT_LTGREEN);
  tft.print(F("BACK"));
    
  tft.setTextSize(3);  
  tft.setCursor(433,34);
  tft.print(F("1")); 
  tft.setTextColor(TFT_BLACK,TFT_LTGREY);
  tft.setCursor(433,110);
  tft.print(F("2")); 
  tft.setCursor(433,95+38+38+15);
  tft.print(F("4")); 
  tft.setCursor(433,95+38+38+38+38+15);
  tft.print(F("8"));
  N_Balken=1;
            
  for (uint16_t j=0; j<256; j++) samples2[j]=sampledata[j+(SDCARDWAVE2*256)];   
  for (uint16_t j=0; j<256; j++) 
     {
      sample_2=(255-samples2[j])+32;
      tft.drawLine(j+110,160,j+110,sample_2, TFT_ORANGE);
     }
  tft.drawLine(100, 160, 376, 160, TFT_BLACK);   
     
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,17);
  tft.print(F("BNK:"));
  tft.print(SDCARDbank);
  tft.print(F(" "));
  tft.print(F("SAM:"));
  tft.print(SDCARDval);
  tft.print(F(" "));  
  tft.print(F("WAV:"));
  tft.print(SDCARDWAVE2);
  tft.print(F("   "));
  tft.setCursor(110,290);
  tft.print(FPSTR((char*)(string_table[SDCARDbank*128+SDCARDval])));
  tft.fillTriangle(100,140,100,180,80,160, TFT_LTGREY);
  tft.fillTriangle(380,140,376,180,400,160, TFT_LTGREY);   
}

void scope_return() 
{
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, 81, 126, TFT_GREY);
  tft.fillRect(5, 3, 71, 119, TFT_WHITE);
  tft.fillRect(10, 8, 61, 34, TFT_LTGREY);
  tft.fillRect(10, 46, 61, 34, TFT_LTGREY);
  tft.fillRect(10, 84, 61, 34, TFT_LTGREY);  
  tft.fillRect(81, 0, 158, 126, TFT_GREY);
  tft.fillRect(86, 3, 148, 119, TFT_WHITE);
  tft.drawLine(96, 63, 229, 63, TFT_BLACK);
  tft.fillRect(240, 0, 158, 126, TFT_GREY);
  tft.fillRect(245, 3, 148, 119, TFT_WHITE);
  tft.drawLine(250, 63, 387, 63, TFT_BLACK);
  tft.fillRect(399, 0, 81, 126, TFT_GREY);
  tft.fillRect(404, 3, 71, 119, TFT_WHITE);
  tft.fillRect(409, 8, 61, 34, TFT_LTGREY);
  tft.fillRect(409, 46, 61, 34, TFT_LTGREY);
  tft.fillRect(409, 84, 61, 34, TFT_LTBLUE);
  tft.fillRect(10, 84, 61, 34, TFT_LTBLUE);
  tft.fillRect(409, 84, 61, 34, TFT_LTBLUE); 
  tft.setTextSize(2);
  tft.fillRect(409, 46, 61, 34, TFT_LTRED);  //Button 5 SAVE
  tft.setTextColor(TFT_BLACK,TFT_LTRED); 
  tft.setCursor(416,57);
  tft.print(F("SAVE"));
  tft.fillRect(10, 46, 61, 34, TFT_GREEN);  //Button 2 LOAD
  tft.setTextColor(TFT_BLACK,TFT_GREEN); 
  tft.setCursor(17,57);
  tft.print(F("LOAD"));
  tft.fillRect(10, 8, 61, 34, TFT_LTORANGE);
  tft.fillRect(409, 8, 61, 34, TFT_LTORANGE);
  tft.setTextColor(TFT_BLACK,TFT_LTORANGE);  //Button 1 PROGRAM DOWN
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
  ShowMenu();
  ShowSample_1(); 
  ShowSample_2();
}

void Create_Wavetable1()  //CRE1 für Wavetable 1
{
  for(uint8_t m=0; m<127; m++)
      {
       for(uint16_t n=0; n<256; n++)
          {
           sampledata[((m+1)*256)+n]=sampledata[(m*256)+n]; 
           if((sampledata[(m*256)+n]<sampledata[(127*256)+n])) {samples1[n]=sampledata[((m+1)*256)+n]+=1;}
           else if((sampledata[(m*256)+n]>sampledata[(127*256)+n])) {samples1[n]=sampledata[((m+1)*256)+n]-=1;}
           else {samples1[n]=sampledata[((m+1)*256)+n];}

//           float faktor = float(sampledata[(127*256)+n]+1 - sampledata[n]) / 128.0;
//           samples1[n] = sampledata[((m+1)*256)+n]+= int8_t(faktor);
           
           sample_1=(255-samples1_alt[n])+32;
           tft.drawLine(n+110,160,n+110,sample_1, TFT_WHITE);
           sample_1=(255-samples1[n])+32;
           tft.drawLine(n+110,160,n+110,sample_1, TFT_ORANGE);
          } 
       tft.drawLine(100, 160, 376, 160, TFT_BLACK);
       for (uint16_t j=0; j<256; j++) samples1_alt[j]=samples1[j];
       tft.setTextSize(2);
       tft.setTextColor(TFT_BLACK,TFT_WHITE); 
       tft.setCursor(110,17);
       tft.print(F("BNK:"));
       tft.print(SDCARDbank);
       tft.print(F(" "));
       tft.print(F("SAM:"));
       tft.print(SDCARDval);
       tft.print(F(" "));  
       tft.print(F("WAV:"));
       tft.print(m);
       tft.print(F("   "));               
      }
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,290);
  tft.print(F("New Wavetable created"));
  Wavetable_Written=true;     
}

void Create_Wavetable2()  //CRE1 für Wavetable 2
{
  for(uint8_t m=0; m<127; m++)
      {
       for(uint16_t n=0; n<256; n++)
          {
           sampledata[((m+1)*256)+n]=sampledata[(m*256)+n]; 
           if((sampledata[(m*256)+n]<sampledata[(127*256)+n])) {samples2[n]=sampledata[((m+1)*256)+n]+=1;}
           else if((sampledata[(m*256)+n]>sampledata[(127*256)+n])) {samples2[n]=sampledata[((m+1)*256)+n]-=1;}
           else {samples2[n]=sampledata[((m+1)*256)+n];}
           sample_2=(255-samples2_alt[n])+32;
           tft.drawLine(n+110,160,n+110,sample_2, TFT_WHITE);
           sample_2=(255-samples2[n])+32;
           tft.drawLine(n+110,160,n+110,sample_2, TFT_ORANGE);
          } 
       tft.drawLine(100, 160, 376, 160, TFT_BLACK);
       for (uint16_t j=0; j<256; j++) samples2_alt[j]=samples2[j];
       tft.setTextSize(2);
       tft.setTextColor(TFT_BLACK,TFT_WHITE); 
       tft.setCursor(110,17);
       tft.print(F("BNK:"));
       tft.print(SDCARDbank);
       tft.print(F(" "));
       tft.print(F("SAM:"));
       tft.print(SDCARDval);
       tft.print(F(" "));  
       tft.print(F("WAV:"));
       tft.print(m);
       tft.print(F("   "));               
      }
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,290);
  tft.print(F("New Wavetable created")); 
  Wavetable_Written=true;    
}

void Create2_Wavetable1()   //CRE2 für Wavetable 1
{
  for(uint16_t n=0; n<256; n++) sampledata[(63*256)+n]=sampledata[(127*256)+n];
  for(uint8_t m=0; m<64; m++)
      {
       for(uint16_t n=0; n<256; n++) 
          {
           sampledata[((m+1)*256)+n]=sampledata[(m*256)+n]; 
           if((sampledata[(m*256)+n]<sampledata[(63*256)+n])) {samples1[n]=sampledata[((m+1)*256)+n]+=2;}
           else if((sampledata[(m*256)+n]>sampledata[(63*256)+n])) {samples1[n]=sampledata[((m+1)*256)+n]-=2;}
           else {samples1[n]=sampledata[((m+1)*256)+n];}
           sample_1=(255-samples1_alt[n])+32;
           tft.drawLine(n+110,160,n+110,sample_1, TFT_WHITE);
           sample_1=(255-samples1[n])+32;
           tft.drawLine(n+110,160,n+110,sample_1, TFT_ORANGE);
          }
       tft.setTextSize(2);
       tft.setTextColor(TFT_BLACK,TFT_WHITE); 
       tft.setCursor(110,17);
       tft.print(F("BNK:"));
       tft.print(SDCARDbank);
       tft.print(F(" "));
       tft.print(F("SAM:"));
       tft.print(SDCARDval);
       tft.print(F(" "));  
       tft.print(F("WAV:"));
       tft.print(m);
       tft.print(F("   "));
       for (uint16_t j=0; j<256; j++) samples1_alt[j]=samples1[j];     
      }      
  for(uint8_t m=64; m<128; m++)
      {
       for(uint16_t n=0; n<256; n++)
          {
           samples1[n]=sampledata[(m*256)+n]=sampledata[((127-m)*256)+n]; 
           sample_1=(255-samples1_alt[n])+32;
           tft.drawLine(n+110,160,n+110,sample_1, TFT_WHITE);
           sample_1=(255-samples1[n])+32;
           tft.drawLine(n+110,160,n+110,sample_1, TFT_ORANGE);
          }
       tft.setTextSize(2);
       tft.setTextColor(TFT_BLACK,TFT_WHITE); 
       tft.setCursor(110,17);
       tft.print(F("BNK:"));
       tft.print(SDCARDbank);
       tft.print(F(" "));
       tft.print(F("SAM:"));
       tft.print(SDCARDval);
       tft.print(F(" "));  
       tft.print(F("WAV:"));
       tft.print(m);
       tft.print(F("   ")); 
       for (uint16_t j=0; j<256; j++) samples1_alt[j]=samples1[j];    
      }            
  tft.drawLine(100, 160, 376, 160, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,290);
  tft.print(F("New Wavetable created")); 
  Wavetable_Written=true;      
}

void Create2_Wavetable2()   //CRE2 für Wavetable 2
{
  for(uint16_t n=0; n<256; n++) sampledata[(63*256)+n]=sampledata[(127*256)+n];
  for(uint8_t m=0; m<64; m++)
      {
       for(uint16_t n=0; n<256; n++) 
          {
           sampledata[((m+1)*256)+n]=sampledata[(m*256)+n]; 
           if((sampledata[(m*256)+n]<sampledata[(63*256)+n])) {samples2[n]=sampledata[((m+1)*256)+n]+=2;}
           else if((sampledata[(m*256)+n]>sampledata[(63*256)+n])) {samples2[n]=sampledata[((m+1)*256)+n]-=2;}
           else {samples2[n]=sampledata[((m+1)*256)+n];}
           sample_2=(255-samples2_alt[n])+32;
           tft.drawLine(n+110,160,n+110,sample_2, TFT_WHITE);
           sample_2=(255-samples2[n])+32;
           tft.drawLine(n+110,160,n+110,sample_2, TFT_ORANGE);
          }
       tft.setTextSize(2);
       tft.setTextColor(TFT_BLACK,TFT_WHITE); 
       tft.setCursor(110,17);
       tft.print(F("BNK:"));
       tft.print(SDCARDbank);
       tft.print(F(" "));
       tft.print(F("SAM:"));
       tft.print(SDCARDval);
       tft.print(F(" "));  
       tft.print(F("WAV:"));
       tft.print(m);
       tft.print(F("   "));
       for (uint16_t j=0; j<256; j++) samples2_alt[j]=samples2[j];     
      }      
  for(uint8_t m=64; m<128; m++)
      {
       for(uint16_t n=0; n<256; n++)
          {
           samples2[n]=sampledata[(m*256)+n]=sampledata[((127-m)*256)+n]; 
           sample_2=(255-samples2_alt[n])+32;
           tft.drawLine(n+110,160,n+110,sample_2, TFT_WHITE);
           sample_2=(255-samples2[n])+32;
           tft.drawLine(n+110,160,n+110,sample_2, TFT_ORANGE);
          }
       tft.setTextSize(2);
       tft.setTextColor(TFT_BLACK,TFT_WHITE); 
       tft.setCursor(110,17);
       tft.print(F("BNK:"));
       tft.print(SDCARDbank);
       tft.print(F(" "));
       tft.print(F("SAM:"));
       tft.print(SDCARDval);
       tft.print(F(" "));  
       tft.print(F("WAV:"));
       tft.print(m);
       tft.print(F("   ")); 
       for (uint16_t j=0; j<256; j++) samples2_alt[j]=samples2[j];    
      }            
  tft.drawLine(100, 160, 376, 160, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK,TFT_WHITE); 
  tft.setCursor(110,290);
  tft.print(F("New Wavetable created"));
  Wavetable_Written=true;       
}

void Save_Wavetable()
{
  if (last_file>126)
      {
       last_file=127;
       SD.remove("/7/127.bin");
       }
  else 
      {
       last_file=last_file+1;
      }

  mBstring="/7/" + String(last_file, DEC) + ".bin";  
  File dataFile = SD.open(mBstring, FILE_WRITE);
  if (!dataFile) return;
  dataFile.write((const byte*)sampledata, 32768);
  dataFile.close();

  if (SD.exists(mBstring)) 
        {
          tft.fillRect(10, 10, 59, 68, TFT_LTGREEN);
          delay(100);
          tft.fillRect(10, 10, 59, 68, TFT_LTRED);
          tft.setTextSize(2);
          tft.setTextColor(TFT_BLACK,TFT_LTRED);
          tft.setCursor(17,34);
          tft.print(F("SAVE"));
          tft.setTextColor(TFT_BLACK,TFT_WHITE); 
          tft.setCursor(110,290);
          tft.print(F("                       "));
          tft.setCursor(110,290);
          tft.print(F("File written:")); 
          tft.print(mBstring);
        }
   else 
        {
          tft.fillRect(10, 10, 59, 68, TFT_RED);
          tft.setTextSize(2);
          tft.setTextColor(TFT_BLACK,TFT_WHITE); 
          tft.setCursor(110,290);
          tft.print(F("File not written, error!  ")); 
        } 
   Wavetable_Written=true;      
}
