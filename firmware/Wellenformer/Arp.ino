/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Arp.ino
 *
 *  Description:
 *      Arpeggiator and note scheduling functions.
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

void ArpNoteOff()
{
      noteStatus[0]=0;
      DCA[0]=0; 
      MIDItable[0]=0; 
      Attack_flag[0]=1;
      Glis_count[0]=0;
      Glis_val[0]=-24; 
      Velocity[0]=0;   
}

void handleTick(unsigned long arptick) 
{ 
  if (arptick - lastTimeGate > gate) 
     {
      ArpNoteOff();   
     }   
   
  if (clockSync || arptick - lastTime > tempo || (firstNotePressed==true && hold==false && ArpTranspose==false)) 
     {
      firstNotePressed=false;
      blinkTime = lastTimeGate = lastTime = arptick;
      digitalWrite(17, HIGH);
      blinkOn = true;    
      
      if ((hold || notesHeld > 0) && notes[0] != '\0') 
         { 
          if (notes[playBeat] == '\0') playBeat = 0;

          if(playBeat==0)
            {
             if(octave==0) octave_add=0; 
             if(octave==-1 && octave_add==-12) octave_add=0; 
             else if(octave==-1 && octave_add==0) octave_add=-12;  
             if(octave==1 && octave_add==12) octave_add=0; 
             else if(octave==1 && octave_add==0) octave_add=12;  
             if(octave==2 && octave_add==0) octave_add=12;
             else if(octave==2 && octave_add==12) octave_add=24;
             else if(octave==2 && octave_add==24) octave_add=0;  
             if(octave==-2 && octave_add==0) octave_add=-12;
             else if(octave==-2 && octave_add==-12) octave_add=-24;
             else if(octave==-2 && octave_add==-24) octave_add=0;       
            }
       
           if (LFO2SYNC!=0) LFO2Counter=0;
           byte i = 0;
           Sample1Changed=true;
           Sample2Changed=true;  
           noteStatus[i]=127;
           MIDItable[i]=octave_add+notes[playBeat]+ArpTranspose;
           SAMPLECNT1[i]=0;
           SAMPLEINC1[i]=0;
           phaccSAMPLE1[i]=0;
           SAMPLEPOS1[i]=SAMPLESTART1;
           SDCARDWAVE1=SAMPLESTART1;
           SAMPLECNT2[i]=0;
           SAMPLEINC2[i]=0;
           phaccSAMPLE2[i]=0;
           SAMPLEPOS2[i]=SAMPLESTART2; 
           SDCARDWAVE2=SAMPLESTART2;        
           DCA[i]=1000;
           DCF[i]=0;
           ADSR2[i]=0;
           DCFAttack_flag[i]=1; 
           DCFDecay_flag[i]=0;
           Attack_flag[i]=1;
           Decay_flag[i]=0;
           Glis_count[i]=0; 
           Glis_val[i]=-24;

           switch (Velocity_Mode) {
              case 0:
              Velocity[i]=127;       
              break;
              case 1:
              Velocity[i]=arpvel[playBeat];         
              break;
              case 2:
              Velocity[i]=VeloINC1tab[arpvel[playBeat]];        
              break;
              case 3:
              Velocity[i]=VeloINC2tab[arpvel[playBeat]];       
              break; 
              case 4:
              Velocity[i]=VeloINC3tab[arpvel[playBeat]];      
              break;  
              case 5:
              Velocity[i]=VeloDEC1tab[arpvel[playBeat]];       
              break;  
              case 6:   //Velocity->Resonance
              Velocity[i]=127; 
              //q=arpvel[playBeat]<<9;
              //FREQUENZ=FREQUENZ_save; 
              break;
              case 7:   //Velocity->Frequenz
              Velocity[i]=127;
              //FREQUENZ=arpvel[playBeat];
              //q=q_save;                     
              break;                
              }
          Velocity[i]=(Velocity[i]*value[74])>>7;    
          if(switches[ArpSwitch][playBeat]==0) Velocity[i]=0;             
          
          if (arpmode == UP) 
            up();
          else if (arpmode == DOWN) 
            down();
          else if (arpmode == BOUNCE) 
            bounce();
          else if (arpmode == UPDOWN) 
            upDown();
          else if (arpmode == ONETHREE)
            oneThree();
          else if (arpmode == ONETHREEEVEN)
            oneThreeEven();
          else if (arpmode == RND) 
            rnd();
         }
    }
}

void up() {
  playBeat++;
  if (notes[playBeat] == '\0')
    playBeat=0;      
}

void down() {
  if (playBeat == 0) {
    playBeat = sizeof(notes)-1;
    while (notes[playBeat] == '\0') {
      playBeat--;
    }
  }        
  else       
    playBeat--;
}

void bounce() {
  if (sizeof(notes) == 1)
    playBeat=0;
  else
    if (arpUp) {
      if (notes[playBeat+1] == '\0') {
        arpUp = false;           
        playBeat--;
      }    
      else
        playBeat++;   
    }
    else {
      if (playBeat == 0) {
        arpUp = true;
        playBeat++;
      }
      else
        playBeat--;
    }
}

void upDown() {
  if (sizeof(notes) == 1)
    playBeat=0;
  else
    if (arpUp) {
      if (notes[playBeat+1] == '\0') {
        arpUp = false;           
      }    
      else
        playBeat++;   
    }
    else {
      if (playBeat == 0) {
        arpUp = true;
      }
      else
        playBeat--;
    }
}

void oneThree() {
  if (arpUp)
    playBeat += 2;
  else
    playBeat--;

  arpUp = !arpUp;

  if (notes[playBeat] == '\0') {
    playBeat = 0;
    arpUp = true;
  }
}

void oneThreeEven() {
  if (notes[playBeat+1] == '\0') {
    playBeat = 0;
    arpUp = true;
    return;
  }
  if (arpUp)
    playBeat += 2;
  else
    playBeat--;

  arpUp = !arpUp;
}

void rnd() {
  playBeat=random(0, sizeof(notes)-1);         
}

void resetNotes() {
  for (int i = 0; i < sizeof(notes); i++) value[96] = notes[i] = '\0';
}

void ChangeArpMode()
{
  byte arpmode_alt=arpmode;
  arpmode=value[72]>>4; 
  if ((arpmode_alt>0 && arpmode==0) || (arpmode_alt==0 && arpmode>0)) 
  {
    notesHeld=0;
    playBeat=0;
    blinkOn=false;
    arpUp=true;
    clockTick=1;
    for(byte i = 0; i < MAX_NOTES; i++) 
     {
      noteStatus[i]=0; 
      Glis_count[i]=0; 
      Glis_val[i]=-24;
      DCA[i]=0; 
      MIDItable[i]=0; 
      Attack_flag[i]=1;
      Velocity[i]=0;
      digitalWrite(16, LOW);
      digitalWrite(17, LOW);                         
     }
   }
}   
