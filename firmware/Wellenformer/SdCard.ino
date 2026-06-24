/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      SdCard.ino
 *
 *  Description:
 *      SD card file handling and sample file detection.
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

void CheckSD_File()
{     
   File8b16k=false;     //16kB  8bit Samples    ".bin"
   File8b32k=false;     //32kB  8bit Samples    ".bin"
   File8b64k=false;     //64kB  8bit Samples    ".bin"
   File8b32kwt=false;   //32kB  8bit Wavetable  ".bwt"
   File16b32kwt=false;  //32kB 16bit Wavetable  ".w16"
   File16b32k=false;    //32kB 16bit Samples    ".b16"   
   File16b64k=false;    //64kB 16bit Samples    ".b16"
   
   mBstring = "/" + String(SDCARDbank, DEC) + "/" + String(SDCARDval, DEC) + ".bin";
   if(SD.exists(mBstring))
     {
      File dataFile = SD.open(mBstring);
      if (dataFile.size()==16384)
         {
          Serial.println(F("16kB 8bit Sample"));
          File8b16k=true;
          dataFile.close();
          return;  
         }                 

      else if (dataFile.size()==32768)
         {
          Serial.println(F("32kB 8bit Sample"));
          File8b32k=true; 
          dataFile.close();
          return;
         } 

      else if (dataFile.size()==65536)
         {
          Serial.println(F("64kB 8bit Sample"));
          File8b64k=true;
          dataFile.close();
          return;           
         }
     }

   mBstring = "/" + String(SDCARDbank, DEC) + "/" + String(SDCARDval, DEC) + ".bwt";       
   if(SD.exists(mBstring))
     {
       Serial.println(F("32kB  8bit Wavetable"));
       File8b32kwt=true;
       return;     
     }     

   mBstring = "/" + String(SDCARDbank, DEC) + "/" + String(SDCARDval, DEC) + ".w16";
   if(SD.exists(mBstring))
     {
       Serial.println(F("32kB 16bit Wavetable"));
       File16b32kwt=true; 
       return;     
     }

   mBstring = "/" + String(SDCARDbank, DEC) + "/" + String(SDCARDval, DEC) + ".b16";            
   if(SD.exists(mBstring))
     {
       File dataFile = SD.open(mBstring);
       if (dataFile.size()==32768)
           {
            Serial.println(F("32kB 16bit Sample"));
            File16b32k=true;
            dataFile.close();
            return;             
           }
       else if (dataFile.size()==65536)
           {
            Serial.println(F("64kB 16bit Sample"));
            File16b64k=true;
            dataFile.close();
            return;             
           }               
     }
}  
