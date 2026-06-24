/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      LoadParameter.ino
 *
 *  Description:
 *      Parameter loading and preset initialization routines.
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

void LoadParameter()
{
  SDCARDbank=value[0]>>4;
  SDCARDval=value[1];
  SAMPLESTART1=value[2];
  SAMPLELOO1=SDCARDWAVE1=value[3];
  SAMPLELEN1=value[4];
  SAMPLEINCTIME1=value[5];
  SAMPLESTART2=value[6];
  SAMPLELOO2=SDCARDWAVE2=value[7];
  SAMPLELEN2=value[8];
  SAMPLEINCTIME2=value[9];
  VOL1=value[10];
  VOL2=value[11];
  
  Pitch1=value[12];
  Pitch2=value[13];
  PITCHFINE1=value[14];
  PITCHFINE2=value[15];
  GLISSANDO=value[16];
  Velocity_Mode=value[17]>>4;
  NoiseLevel=value[18];
  GAIN1=value[19]>>4;
  GAIN2=value[20]>>4;
  if(value[21]==0) {value[21]=127; Fclpf=0.25f;}
  else Fclpf=mapFloat(value[21],0,127,0.001f, 0.25f);
  Qlpf=mapFloat(value[22],0,127,0.707, 10.0f); 
  Set_LPF_Parameter();
  EFX_Sat=value[23];
 
  FREQUENZ=FREQUENZ_save=value[24];
  q=q_save=value[25]<<9;
  DCFATTACK=value[26];
  DCFDECAY=value[27];
  DCFSUSTAIN=value[28]*516;
//  =value[29];
//  =value[30];
  Fcls=mapFloat(value[31],0,127,0.001,0.25); 
  peakGainls=mapFloat(value[32],0,127,0.0,10.0); 
  Set_LowShelf_Parameter();
  Fchs=mapFloat(value[33],0,127,0.032,0.5); 
  peakGainhs=mapFloat(value[34],0,127,0.0,10.0); 
  Set_HighShelf_Parameter();
  DCFKeyFollow=value[35];
  
  ATTACK=value[36];
  DECAY=value[37];
  SUSTAIN=map(value[38],0,127,1,65535);
  //SUSTAIN=value[38]*516;
  RELEASE=value[39];
//  =value[40];
  DCFATTACK=value[41];
  DCFDECAY=value[42];
  DCFSUSTAIN=value[43]*516;
//  =value[44];  
//  =value[45];
//  =value[46];
//  =value[47];

  LFO1WAVE=value[48]>>5;
  LFO1SPEED=value[49];
  LFO1DEPHT=value[50];
  LFO1SYNC=value[51]; 
  LFO2WAVE=value[52]>>5;
  LFO2SPEED=value[53];
  LFO2DEPHT=value[54];
  LFO2SYNC=value[55];
  LFO3WAVE=value[56]>>5;
  LFO3SPEED=value[57];
  LFO3DEPHT=value[58];
  LFO3SYNC=value[59];

  wet=float((value[60]/127.0f)*2.0f);
  revtime=(float)value[61]*NORM127MUL;  
  Delay_SetLevel_l(value[62]*516);
  Delay_SetLevel_r(value[63]*516);
  Delay_SetFeedback_l(value[64]*516);
  Delay_SetFeedback_r(value[65]*516); 
  StereoEFX=value[66];
  Delay_SetLength_l(value[67]);
  Delay_SetLength_r(value[68]);
  Struct_EFX=value[69];
  EFXMIDICLKDIV=value[70];
  EFX_Wide_Stereo=value[71];

  for (int i = 0; i < MAX_DELAY; i++)
   {
    delayLine_l[i] = 0;
    delayLine_r[i] = 0;
   }

  ChangeArpMode();                                        //ArpMode
  //value[73]                                             //Arp Speed
  //value[74]                                             //Arp Vel 
  if(value[75]==0) clockSync=false; else clockSync=true;  //Arp Clk
  if(value[76]==0) hold=false; else hold=true;            //Arp Hold / Hold+Transpose 
  if(value[76]>63) holdTrans=true; else holdTrans=false;
  if(value[77]<25) octave=-2;                             //Arp Octave
  else if(value[77]>=25&&value[77]<50) octave=-1; 
  else if(value[77]>=50&&value[77]<75) octave=0; 
  else if(value[77]>=75&&value[77]<100) octave=1;
  else if(value[77]>=100) octave=2; 
  //value[78]                                              //Arp Gate
  ArpSwitch=value[79]>>4;                                  //Arp Notes (on/off)
  //value[80]
  //value[81]
  //value[82]
  //value[83]     

  ChannelNote=value[84]>>3;
  ChannelCC=value[85]>>3;
  if(value[86]==0) CCon=false; else CCon=true;
  Wheel_Target=value[87]/30; if (Wheel_Target>4) Wheel_Target=4; 
  Aftertouch_Target=value[88]>>5;
  //value[89]
  chorusToMix_l=chorusToMix_r=value[90]*516; //Chorus1 Level
  LFO4SPEED=value[91]; //Chorus1 Speed
  LFO4DEPHT=value[92]; //Chorus1 Mod Depht
  chorusToMix2_l=chorusToMix2_r=value[93]*516; //Chorus2 Level
  LFO5SPEED=value[94]; //Chorus2 Speed
  LFO5DEPHT=value[95]; //Chorus2 Mod Depht

  if (holdTrans!=true) for(int i = 0; i < sizeof(notes); i++) value[96+i] = notes[i] = '\0';
  else for(int i = 0; i < sizeof(notes); i++) {notes[i] = value[i+96]; arpvel[i] = value[i+96+sizeof(notes)];}
  notesHeld=0;
  playBeat=0;
  blinkOn=false;
  arpUp=true;
  clockTick=1;
    
  LoadSample();
  WavetableChanged=true;
  ShowSample_1(); 
  ShowSample_2();
  WavetableChanged=false;    
}
