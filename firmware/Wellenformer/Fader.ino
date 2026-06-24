/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Fader.ino
 *
 *  Description:
 *      Graphical fader and level-meter helper routines.
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

// #########################################################################
//  Draw a linear meter on the screen
// #########################################################################
void plotLinear(char *label, int x, int y)
{
  int w = 36;
  tft.setTextSize(1);
  tft.drawRect(x, y, w, 185, TFT_GREY);
  tft.fillRect(x+2, y + 19, w-3, 185 - 38, TFT_WHITE);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(label, x + w / 2, y + 2, 2);

  for (int i = 0; i < 140; i += 10) tft.drawFastHLine(x + 20, y + 27 + i, 6, TFT_BLACK);
  for (int i = 130; i > 0; i -= 50) tft.drawFastHLine(x + 20, y + 27 + i, 9, TFT_BLACK);
  
  tft.fillTriangle(x+3, y + 158, x+3+16, y+158, x + 3, y + 158 - 5, TFT_RED);
  tft.fillTriangle(x+3, y + 158, x+3+16, y+158, x + 3, y + 158 + 5, TFT_RED);
}

// #########################################################################
//  Adjust 12 linear meter pointer positions
// #########################################################################
void plotPointer(byte counter)
{
  int dy = 187;
  byte pw = 16;
  char buf[8];

  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  // Move the 12 pointers one pixel towards new value
  for (int i = 0; i < 12; i++)
  {
    if(counter*12+i==12 || counter*12+i==13)  //PITCH1/2 als +24 bis -24 anzeigen
      {
       if(PITCHtab24[value[counter*12+i]]>127) {int temp=PITCHtab24[value[counter*12+i]]-256; dtostrf(temp, 4, 0, buf);} 
       else dtostrf(PITCHtab24[value[counter*12+i]], 4, 0, buf);
      }
     else if(counter*12+i==84 || counter*12+i==85)  //MIDI CH und CC 0-15 anzeigen
      { 
       dtostrf(value[counter*12+i]>>3, 4, 0, buf);
      }  
     else if(counter*12+i==0 || counter*12+i==79)  //Bank 0-7 und Arp Notes 0-7
      { 
       dtostrf(value[counter*12+i]>>4, 4, 0, buf);
      }                  
    else dtostrf(value[counter*12+i], 4, 0, buf);
    
    if(counter*12+i!=48 && counter*12+i!=52 && counter*12+i!=17 && counter*12+i!=72 && counter*12+i!=87 && counter*12+i!=88 && counter*12+i!=76 && counter*12+i!=77) tft.drawRightString(buf, i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);

    if(counter*12+i==48)
      {
       switch (LFO1WAVE)
          {
           case 0:
           tft.drawRightString(" TRI ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString(" REC ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 2:
           tft.drawRightString(" SAW ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 3:
           tft.drawRightString(" RND ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           default:
           break; 
          }
      }
      
    if(counter*12+i==52)
      {
       switch (LFO2WAVE)
          {
           case 0:
           tft.drawRightString(" TRI ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString(" REC ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 2:
           tft.drawRightString(" SAW ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 3:
           tft.drawRightString(" RND ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           default:
           break; 
          }
      }

    if(counter*12+i==17)
      {
       switch (Velocity_Mode)
          {
           case 0:
           tft.drawRightString(" OFF", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString(" ON ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 2:
           tft.drawRightString(" TA1", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 3:
           tft.drawRightString(" TA2", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 4:
           tft.drawRightString(" TA3", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 5:
           tft.drawRightString(" TA4", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 6:
           tft.drawRightString(" RES", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break; 
           case 7:
           tft.drawRightString(" CUT", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;                                           
           default:
           break; 
          }
      } 

    if(counter*12+i==72)
      {
       switch (arpmode)
          {
           case 0:
           tft.drawRightString(" OFF", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString("  UP", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 2:
           tft.drawRightString(" DWN", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 3:
           tft.drawRightString(" BNC", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 4:
           tft.drawRightString(" UDO", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 5:
           tft.drawRightString("  13", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 6:
           tft.drawRightString(" 13E", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break; 
           case 7:
           tft.drawRightString(" RND", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;                                           
           default:
           break; 
          }
      }       

    if(counter*12+i==87)
      {
       switch (Wheel_Target)
          {
           case 0:
           tft.drawRightString("  OFF", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString(" LFO1", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 2:
           tft.drawRightString(" LFO2", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 3:
           tft.drawRightString(" FREQ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 4:
           tft.drawRightString("  RES", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;                                        
           default:
           break; 
          }
      } 
      
    if(counter*12+i==88)
      {
       switch (Aftertouch_Target)
          {
           case 0:
           tft.drawRightString("  OFF", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString(" FREQ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 2:
           tft.drawRightString("  RES", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 3:
           tft.drawRightString("   WT", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;                                        
           default:
           break; 
          }
      }

    if(counter*12+i==76)  // ARP PLAY/HOLD/TRANS
      {
       if (!hold & !holdTrans) tft.drawRightString("  OFF", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
       else if (hold & !holdTrans) tft.drawRightString("   ON", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
       else if (hold & holdTrans) tft.drawRightString("  TRS", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);        
      }      
      
    if(counter*12+i==77)    
      {
       switch (octave)
          {
           case -2:
           tft.drawRightString("  -2 ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;            
           case -1:
           tft.drawRightString("  -1 ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 0:
           tft.drawRightString("   0 ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;
           case 1:
           tft.drawRightString("   1 ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break; 
           case 2:
           tft.drawRightString("   2 ", i * 40 + 36 - 5, 187 - 27 + 155 - 18, 2);
           break;                                                 
           default:
           break; 
          }          
      }
                         

    int dx = 3 + 40 * i;
    if (value[counter*12+i] < 0) value[counter*12+i] = 0; // Limit value to emulate needle end stops
    if (value[counter*12+i] > 127) value[counter*12+i] = 127;

    while (!(value[counter*12+i] == old_value[counter*12+i])) {
      dy = 187 + 100 - old_value[counter*12+i];
      if (old_value[counter*12+i] > value[counter*12+i])
      {
        tft.drawLine(dx, dy - 5, dx + pw, dy, TFT_WHITE);
        old_value[counter*12+i]--;
        tft.drawLine(dx, dy + 6, dx + pw, dy + 1, TFT_RED);
      }
      else
      {
        tft.drawLine(dx, dy + 5, dx + pw, dy, TFT_WHITE);
        old_value[counter*12+i]++;
        tft.drawLine(dx, dy - 6, dx + pw, dy - 1, TFT_RED);
      }
    }
  }
}
