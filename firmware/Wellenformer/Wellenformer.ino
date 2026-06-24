/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Wellenformer.ino
 *
 *  Description:
 *      Main Arduino sketch, global configuration, UI, MIDI and synthesizer control.
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

#include <WiFi.h>
#include "driver/i2s.h"
#include <SPI.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include <SD.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include "XPT2046_Touchscreen.h"
#include <Preferences.h>
Preferences preferences;
#include "MapFloat.h" 
#include "samplenames.h"

#define TFT_GREY 0x5AEB
#define TFT_LTGREY 0xE71C 
#define TFT_LTGREEN 0xBFF7
#define TFT_LTORANGE 0xFE73
#define TFT_LTBLUE 0xB6DF 
#define TFT_LTRED 0xFD34
#define TFT_LTYELLOW 0xFFF8   

#define CS_PIN  22
#define TIRQ_PIN 27
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN);

uint16_t sampleRate=32000;
const uint8_t MAX_NOTES=6;
const uint8_t anzahl_parameter=128;
byte anzahl_menu=8;
uint8_t edit_page=1;
#define NORM127MUL  0.007874f
uint8_t N_Balken=1;
byte counter_alt=0;
#define AUDIO_BUFF 10
uint8_t last_file=0;
#define MAX_DELAY  20000 //625msec
uint16_t delayLen_l = MAX_DELAY-2;
uint16_t delayLen_r = MAX_DELAY-2;
#define MAX_CHORUS  2000  //62,5msec
uint16_t chorusLen_l = MAX_CHORUS-2;
uint16_t chorusLen_r = MAX_CHORUS-2;
uint16_t chorusToMix_l = 0;
uint16_t chorusToMix_r = 0;
uint16_t chorusLen2_l = MAX_CHORUS-2;
uint16_t chorusLen2_r = MAX_CHORUS-2;
uint16_t chorusToMix2_l = 0;
uint16_t chorusToMix2_r = 0;

const unsigned long pitchSAMPLE[128] PROGMEM = {4287,4542,4812,5098,5401,5722,6062,6423,6805,7209,7638,8092,8573,9083,9623,10196,10802,11444,12124,12845,13609,14418,15276,16184,17146,18166,19246,20391,21603,22888,24248,25690,27218,28836,30551,32368,34292,36331,38492,40781,43205,45775,48496,51380,54435,57672,61102,64735,68584,72662,76983,81561,86410,91549,96992,102760,108870,115344,122203,129469,137168,145324,153965,163121,172820,183097,193984,205519,217740,230687,244405,258938,274335,290648,307930,326241,345640,366193,387968,411037,435479,461374,488809,517875,548669,581295,615860,652481,691280,732385,775935,822074,870958,922747,977617,1035749,1097338,1162589,1231720,1304962,1382559,1464770,1551870,1644148,1741915,1845494,1955233,2071497,2194675,2325177,2463439,2609923,2765117,2929539,3103739,3288296,3483829,3690988,3910465,4142994,4389349,4650353,4926878,5219845,5530233,5859078,6207477,6576592};
const uint16_t ADSRtab[128] PROGMEM = {65535,52428,43690,38550,35424,32768,29789,27887,26214,23831,21845,20165,18724,16384,14563,13797,13107,10923,10082,9362,8738,8192,7710,7282,6554,5243,4369,3745,3277,2979,2731,2621,2383,2185,2016,1872,1748,1638,1542,1456,1311,874,1092,1008,936,874,819,728,655,546,485,437,374,328,291,262,238,218,202,187,175,164,154,131,109,93,82,73,66,60,56,52,48,45,43,40,38,37,35,33,32,31,30,29,28,27,26,25,25,24,23,23,22,22,21,21,20,20,20,19,19,18,18,18,18,17,17,17,16,16,16,16,16,15,15,15,15,15,14,14,14,14,14,14,13,13,13,13};
const uint8_t PITCHtab24[128] PROGMEM = {-24,-24,-23,-23,-23,-22,-22,-21,-21,-21,-20,-20,-20,-19,-19,-18,-18,-18,-17,-17,-17,-16,-16,-15,-15,-15,-14,-14,-14,-13,-13,-12,-12,-12,-11,-11,-11,-10,-10,-9,-9,-9,-8,-8,-8,-7,-7,-6,-6,-6,-5,-5,-5,-4,-4,-3,-3,-3,-2,-2,-2,-1,-1,0,0,0,1,2,3,4,4,5,5,6,7,7,8,8,9,9,10,10,11,11,12,12,12,13,13,14,14,14,15,15,15,16,16,17,17,17,17,18,18,18,19,19,19,19,20,20,20,21,21,21,21,21,22,22,22,22,23,23,23,23,23,24,24,24};
const uint8_t PITCHtab[128] PROGMEM = {-12,-12,-12,-11,-11,-11,-11,-11,-11,-10,-10,-10,-10,-10,-9,-9,-9,-9,-9,-8,-8,-8,-8,-8,-8,-7,-7,-7,-7,-7,-6,-6,-6,-6,-6,-5,-5,-5,-5,-5,-5,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,8,9,9,9,9,9,9,9,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12};
const uint16_t Finetab[128] PROGMEM = {4,5,5,5,5,6,6,6,7,7,8,8,8,9,9,10,11,11,12,13,13,14,15,16,17,18,19,20,21,22,23,25,26,28,29,31,33,35,37,39,41,44,46,49,52,55,58,61,65,69,73,77,82,87,92,97,103,109,116,123,130,138,146,154,164,173,183,194,206,218,231,245,259,275,291,308,327,346,367,388,411,436,462,489,518,549,582,616,653,692,733,776,822,871,923,978,1036,1098,1163,1232,1305,1383,1465,1552,1645,1742,1846,1956,2072,2195,2326,2464,2610,2765,2930,3104,3289,3484,3691,3911,4143,4390,4651,4927,5220,5531,5859,0};
const uint8_t VeloINC1tab[128] PROGMEM = {0,1,3,7,10,14,17,20,23,26,28,31,33,35,37,40,42,43,45,47,48,50,52,53,54,56,57,58,60,61,62,64,65,67,67,68,70,71,71,73,74,74,76,76,77,79,79,80,80,82,82,84,84,85,85,87,87,88,88,90,90,92,92,92,94,94,95,95,95,97,97,99,99,99,101,101,101,102,102,102,104,104,104,106,106,106,108,108,108,108,110,110,110,112,112,112,112,113,113,113,115,115,115,115,117,117,117,117,119,119,119,119,121,121,121,121,121,123,123,123,123,125,125,125,125,125,127,127};
const uint8_t VeloINC2tab[128] PROGMEM = {0,9,18,29,36,42,47,51,54,58,60,63,65,67,69,71,73,74,76,77,78,80,81,82,83,84,85,86,87,88,89,90,91,92,92,93,94,95,95,96,97,97,98,98,99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,108,109,109,110,110,110,111,111,112,112,112,113,113,113,114,114,114,115,115,115,116,116,116,117,117,117,117,118,118,118,119,119,119,119,120,120,120,121,121,121,121,122,122,122,122,123,123,123,123,124,124,124,124,124,125,125,125,125,126,126,126,126,126,127,127};
const uint8_t VeloINC3tab[128] PROGMEM = {0,90,90,98,100,102,103,104,105,106,107,108,109,109,110,110,111,111,112,112,113,113,113,114,114,114,115,115,115,116,116,116,116,117,117,117,117,117,118,118,118,118,118,119,119,119,119,119,120,120,120,120,120,120,120,121,121,121,121,121,121,121,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123,123,123,123,124,124,124,124,124,124,124,124,124,124,124,125,125,125,125,125,125,125,125,125,125,125,125,126,126,126,126,126,126,126,126,126,126,126,126,126,126,127,127,127,127,127,127,127,127,127,127,127};
const uint8_t VeloDEC1tab[128] PROGMEM = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,5,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,13,13,14,15,15,16,17,17,18,19,20,20,21,22,23,24,25,26,26,27,28,29,30,31,32,33,34,35,36,37,39,40,41,42,43,44,45,47,48,49,50,52,53,54,56,57,58,60,61,62,64,65,67,68,70,71,73,74,76,77,79,80,82,84,85,87,88,90,92,94,95,97,99,101,102,104,106,108,110,112,113,115,117,119,121,123,125,127};

//Envelope and DCA parameters
byte ATTACK;          
byte DECAY;           
uint16_t SUSTAIN;       
byte RELEASE;         
byte Attack_flag[MAX_NOTES];
byte Decay_flag[MAX_NOTES];
byte DCFATTACK;       
byte DCFDECAY;       
uint16_t DCFSUSTAIN;    
byte DCFRELEASE;
byte DCFAttack_flag[MAX_NOTES];
byte DCFDecay_flag[MAX_NOTES];
uint32_t ADSR2[MAX_NOTES];
int32_t DCA[MAX_NOTES];
int32_t DCF[MAX_NOTES];

uint16_t ENVcnt=100;
uint32_t mil=0;
uint32_t NoteActivationTime[MAX_NOTES];

uint8_t MIDISTATE=0;
uint8_t MIDIRUNNINGSTATUS=0;
uint8_t MIDINOTE;
uint8_t MIDIVEL;
uint8_t controller;
uint8_t Cvalue;
bool midi_clk_rcv=false;
byte ChannelNote=0;
uint8_t MIDItable[MAX_NOTES];
bool MidiThru=true;

bool Sample1Changed=false;
bool Sample2Changed=false;
bool WavetableChanged=false;
bool MenuChanged=false;
uint8_t MIDInote[MAX_NOTES];
uint8_t noteStatus[MAX_NOTES];
byte ChannelCC=0;
bool CCon=1;
uint8_t EFXMIDICLKDIV=24;
bool midi_clk_rcv_efx=false;
uint32_t efxtick_start; 
uint32_t efxclockTick;
uint32_t efxtick;
uint32_t efxtick_end;
bool EFXMIDICLKDIV_set2null=false;

float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = 120, osy = 120; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update

int old_analog =  -999; // Value last displayed
int old_digital = -999; // Value last displayed
byte value[anzahl_parameter]; 
int old_value[anzahl_parameter];
int d = 0;

bool button1=false;
bool button2=false;
bool button3=false;
bool button4=false;
bool button5=false;
bool button6=false;
bool button1_pressed=false;
bool button2_pressed=false;
bool button3_pressed=false;
bool button4_pressed=false;
bool button5_pressed=false;
bool button5_pressed_second=false;
bool button6_pressed=false;
unsigned long button_millis;
unsigned long WavetableChanged_millis;
uint8_t Pressed_ValButton;
unsigned long save_preferences_millis;
bool save_Program_changed=false;
unsigned long save_back_millis;

File myFile;
String mBstring;
byte menu=0; 

uint8_t samples1[256];
uint8_t samples1_alt[256];
uint16_t sample_1;
uint8_t sampledata[32768];
uint16_t SAMPLECNT1[MAX_NOTES];
uint32_t phaccSAMPLE1[MAX_NOTES];
uint8_t SAMPLEPOS1[MAX_NOTES];
uint32_t SAMPLESTART1;
uint32_t SAMPLELOO1;
uint32_t SAMPLELEN1;
uint16_t SAMPLEINC1[MAX_NOTES];
uint8_t SAMPLEINCTIME1;
byte SDCARDWAVE1;
byte SDCARDval;
byte SDCARDbank;
uint8_t Pitchbend;
byte Pitch1;
byte PitchSumme1;
byte PITCHFINE1;
byte GAIN1=0;

uint8_t samples2[256];
uint8_t samples2_alt[256];
uint16_t sample_2;
uint16_t SAMPLECNT2[MAX_NOTES];
uint32_t phaccSAMPLE2[MAX_NOTES];
uint8_t SAMPLEPOS2[MAX_NOTES];
uint32_t SAMPLESTART2;
uint32_t SAMPLELOO2;
uint32_t SAMPLELEN2;
uint16_t SAMPLEINC2[MAX_NOTES];
uint8_t SAMPLEINCTIME2;
uint8_t SDCARDWAVE2;
uint8_t Pitch2;
uint8_t PitchSumme2;
byte PITCHFINE2;
byte GAIN2=0;

int32_t SYNTHSUM;
int32_t SYNTHSUM_TX[AUDIO_BUFF];
int16_t SYNTHTOTALSUM_L;
int16_t SYNTHTOTALSUM_R;
int32_t SYNTHTOTAL[MAX_NOTES];

byte GLISSANDO;
uint16_t Glis_count[MAX_NOTES];
int16_t Glis_val[MAX_NOTES];

int32_t f[MAX_NOTES];
int32_t q=0;
int32_t q_save=0;
int32_t t1[MAX_NOTES];
int32_t t2[MAX_NOTES];
int32_t t3[MAX_NOTES];
int32_t t4[MAX_NOTES];
int32_t t5[MAX_NOTES];
int32_t t6[MAX_NOTES];
int32_t t7[MAX_NOTES];
int32_t t8[MAX_NOTES];
int32_t vi[MAX_NOTES];
int32_t vo[MAX_NOTES];

byte LFO1SPEED;
byte LFO1SPEED_val;
byte LFO1SPEED2_val;
byte LFO1DEPHT;
byte LFO1WAVE;
byte LFO1SYNC;
byte LFO1;
byte LFO1_alt=0;
byte LFO1Counter=0;
bool LFO1Countup=true;
bool LFO1_Led_flag=false;
unsigned long LFO1_Led_millis;
uint8_t LFO1_Led_Counter=0;
bool midi_clk_rcv_lfo1=false;
uint8_t lfo1clockTick;

byte LFO2SPEED;
uint16_t LFO2SPEED_val;
byte LFO2SPEED2_val;
byte LFO2DEPHT;
byte LFO2WAVE;
byte LFO2SYNC;
int32_t LFO2;
int32_t LFO2_alt;
byte LFO2Counter=0;
bool LFO2Countup=true;
bool LFO2_Led_flag=false;
unsigned long LFO2_Led_millis;
uint8_t LFO2_Led_Counter=0;
bool midi_clk_rcv_lfo2=false;
uint8_t lfo2clockTick;

byte LFO3SPEED;
uint16_t LFO3SPEED_val;
byte LFO3SPEED2_val;
byte LFO3DEPHT;
byte LFO3WAVE;
byte LFO3SYNC;
int32_t LFO3;
int32_t LFO3_alt;
byte LFO3Counter=0;
bool LFO3Countup=true;
bool LFO3_flag=false;
bool LFO3_Led_flag=false;
unsigned long LFO3_Led_millis;
uint8_t LFO3_Led_Counter=0;
bool midi_clk_rcv_lfo3=false;
uint8_t lfo3clockTick;

int8_t LFO4=0;  //Chorus1
uint16_t LFO4_counter=0;
byte LFO4SPEED;                        
byte LFO4DEPHT; 
uint16_t LFO4SPEED_val;
bool LFO4_flag=false;
const int8_t LFO4_Tab[510] PROGMEM = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224,223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208,207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192,191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,175,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160,159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144,143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};

int8_t LFO5=0;  //Chorus2
uint16_t LFO5_counter=0;
byte LFO5SPEED;                        
byte LFO5DEPHT; 
uint16_t LFO5SPEED_val;
bool LFO5_flag=false;

byte FREQUENZ;
byte FREQUENZ_save;
byte StereoEFX;
float wet;
float revtime;
byte PROGRAM;
bool Program_Changed=false;
byte VOL1;
byte VOL2;
byte Velocity_Mode;
uint8_t Velocity[MAX_NOTES];
byte Aftertouch_Target;
byte Wheel_Target;
byte Struct_EFX;
byte EFX_Wide_Stereo;
byte EFX_Sat;
byte DCFKeyFollow;

unsigned long scope_millis;
bool scope1_pressed=false;
bool scope2_pressed=false; 
bool scope1_fullscreen=false;
bool scope2_fullscreen=false;
bool scbutton1_pressed=false;
bool scbutton2_pressed=false;
uint8_t scbutton2_pressed_state=0;
bool scbutton3_pressed=false;
uint8_t scbutton3_pressed_state=0;
bool scbutton4_pressed=false;
bool scbutton5_pressed=false;
bool scbutton6_pressed=false;
bool scbutton7_pressed=false;
bool scbutton8_pressed=false;
bool leftarrow_pressed=false; 
bool rightarrow_pressed=false;
bool ValButton_pressed=false;
bool Wavetable_Written=false;   

bool File8b16k=false;     //16kB  8bit Samples    ".bin"
bool File8b32k=false;     //32kB  8bit Samples    ".bin"
bool File8b64k=false;     //64kB  8bit Samples    ".bin"
bool File8b32kwt=false;   //32kB  8bit Wavetable  ".bwt"
bool File16b32kwt=false;  //32kB 16bit Wavetable  ".w16"
bool File16b32k=false;    //32kB 16bit Samples    ".b16"   
bool File16b64k=false;    //64kB 16bit Samples    ".b16"

const int CHANNEL      = 0;
const int UP           = 1;
const int DOWN         = 2;
const int BOUNCE       = 3;
const int UPDOWN       = 4;
const int ONETHREE     = 5;
const int ONETHREEEVEN = 6;
const int RND          = 7;

int8_t notes[10];
int8_t arpvel[10];
int8_t switches[8][10] = {   //1=Note wird gespielt, 0=Note wird nicht gespielt
          {1,1,1,1,1,1,1,1,1,1},
          {1,0,1,0,1,1,1,1,1,1},
          {1,1,0,1,1,0,1,1,0,1},
          {1,0,0,0,0,1,1,1,1,1},
          {1,1,1,1,0,1,1,1,0,0},
          {1,0,1,0,1,0,1,0,1,0},
          {1,0,0,0,0,1,0,0,0,0},
          {1,0,0,1,1,0,0,1,0,1},};
uint8_t ArpSwitch;
unsigned long tempo;
unsigned long gate;
unsigned long lastTime;
unsigned long lastTimeGate;
unsigned long blinkTime;
unsigned long arptick;
uint8_t playBeat;
uint8_t playBeatcounter;
uint8_t notesHeld;
uint8_t clockTick;
boolean blinkOn;
boolean hold;
boolean holdTrans;
boolean arpUp;
boolean clockSync;
int8_t arpmode;
int octave;
int octave_add;
bool firstNotePressed=false;
int ArpTranspose;

int16_t Noise;
byte NoiseLevel;

const int ledPin1 = 12;
const int ledPin2 = 13;
const int ledPin3 = 21;

//i2s configuration 
int i2s_num = 0; // i2s port number
i2s_config_t i2s_config = {
     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
     .sample_rate = sampleRate,
     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,    
     .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3, // high interrupt priority // 0 fuer default interrupt priority
     .dma_buf_count = 4,  //8
     .dma_buf_len = 128,  //256
     .use_apll = true       
    };
    
i2s_pin_config_t pin_config = {
    .bck_io_num = 26, //this is BCK pin
    .ws_io_num = 25, // this is LRCK pin
    .data_out_num = 14, // this is DATA output pin
    .data_in_num = -1   // this is DATA input pin
};

TaskHandle_t  Core0TaskHnd;
hw_timer_t * timer = NULL;

//******************** REVERB *****************************************
//Schroeder delays
#define l_CB0 3460/2
#define l_CB1 2988/2
#define l_CB2 3882/2
#define l_CB3 4312/2
#define l_AP0 480/2
#define l_AP1 161
#define l_AP2 46

//define pointer limits = delay time
int cf0_lim, cf1_lim, cf2_lim, cf3_lim, ap0_lim, ap1_lim, ap2_lim;

//define buffer for comb- and allpassfilters
float cfbuf0[l_CB0], cfbuf1[l_CB1], cfbuf2[l_CB2], cfbuf3[l_CB3];
float apbuf0[l_AP0], apbuf1[l_AP1], apbuf2[l_AP2];
//feedback defines as of Schroeder
float cf0_g = 0.805f, cf1_g=0.827f, cf2_g=0.783f, cf3_g=0.764f;
float ap0_g = 0.7f, ap1_g = 0.7f, ap2_g = 0.7f;
//buffer-pointer
int cf0_p=0, cf1_p=0, cf2_p=0, cf3_p=0, ap0_p=0, ap1_p=0, ap2_p=0;

float Do_Comb0(float inSample) {
  float readback = cfbuf0[cf0_p];
  float newval = readback*cf0_g + inSample;
  cfbuf0[cf0_p] = newval;
  cf0_p++;
  if (cf0_p==cf0_lim) cf0_p = 0;
  return readback;
}

float Do_Comb1(float inSample) {
  float readback = cfbuf1[cf1_p];
  float newval = readback*cf1_g + inSample;
  cfbuf1[cf1_p] = newval;
  cf1_p++;
  if (cf1_p==cf1_lim) cf1_p = 0;
  return readback;
}

float Do_Comb2(float inSample) {
  float readback = cfbuf2[cf2_p];
  float newval = readback*cf2_g + inSample;
  cfbuf2[cf2_p] = newval;
  cf2_p++;
  if (cf2_p==cf2_lim) cf2_p = 0;
  return readback;
}

float Do_Comb3(float inSample) {
  float readback = cfbuf3[cf3_p];
  float newval = readback*cf3_g + inSample;
  cfbuf3[cf3_p] = newval;
  cf3_p++;
  if (cf3_p==cf3_lim) cf3_p = 0;
  return readback;
}

float Do_Allpass0(float inSample) {
  float readback = apbuf0[ap0_p];
  readback += (-ap0_g) * inSample;
  float newval = readback*ap0_g + inSample;
  apbuf0[ap0_p] = newval;
  ap0_p++;
  if (ap0_p == ap0_lim) ap0_p=0;
  return readback;
}

float Do_Allpass1(float inSample) {
  float readback = apbuf1[ap1_p];
  readback += (-ap1_g) * inSample;
  float newval = readback*ap1_g + inSample;
  apbuf1[ap1_p] = newval;
  ap1_p++;
  if (ap1_p == ap1_lim) ap1_p=0;
  return readback;
}

float Do_Allpass2(float inSample) {
  float readback = apbuf2[ap2_p];
  readback += (-ap2_g) * inSample;
  float newval = readback*ap2_g + inSample;
  apbuf2[ap2_p] = newval;
  ap2_p++;
  if (ap2_p == ap2_lim) ap2_p=0;
  return readback;
}

float Do_Reverb(float inSample) {
  float newsample = (Do_Comb0(inSample) + Do_Comb1(inSample) + Do_Comb2(inSample) + Do_Comb3(inSample))/4.0f;
  newsample = Do_Allpass0(newsample);
  newsample = Do_Allpass1(newsample);
  newsample = Do_Allpass2(newsample);
  return newsample;
}

// ****************** BIQUAD FILTER ***********************************
float a0, a1, a2, b1, b2;
float Fc, Q, peakGain;
float l_z1, l_z2, r_z1, r_z2;

//*********************************************************************
//* Biquad LowShelf
//*********************************************************************
float Fcls, peakGainls;
float a0ls, a1ls, a2ls, b1ls, b2ls;
float l_z1ls, l_z2ls, r_z1ls, r_z2ls;

//*********************************************************************
//* Biquad HighShelf
//*********************************************************************
float Fchs, peakGainhs;
float a0hs, a1hs, a2hs, b1hs, b2hs;
float l_z1hs, l_z2hs, r_z1hs, r_z2hs;

//*********************************************************************
//* Biquad LPF
//*********************************************************************
float a0lpf, a1lpf, a2lpf, b1lpf, b2lpf;
float Fclpf, Qlpf, peakGainlpf;
float l_z1lpf, l_z2lpf, r_z1lpf, r_z2lpf;

//*********************************************************************
//* Biquad HPF
//*********************************************************************
float a0hpf, a1hpf, a2hpf, b1hpf, b2hpf;
float Fchpf, Qhpf, peakGainhpf;
float l_z1hpf, l_z2hpf, r_z1hpf, r_z2hpf;

//*********************************************************************
//* Biquad BPF
//*********************************************************************
float a0bpf, a1bpf, a2bpf, b1bpf, b2bpf;
float Fcbpf, Qbpf, peakGainbpf;
float l_z1bpf, l_z2bpf, r_z1bpf, r_z2bpf;

//*********************************************************************

void IRAM_ATTR onTimer(){ //20us IRQ

  if(midi_clk_rcv_lfo1==true && LFO1SYNC>1)
    {
     midi_clk_rcv_lfo1=false; 
     if (lfo1clockTick > LFO1SYNC)   //MidiClock geteilt durch LFO1SYNC setzt LFO1 Counter auf 0
        {  
         LFO1Counter=0;
         lfo1clockTick = 1;      
        }
     lfo1clockTick++;   
    }

  if(midi_clk_rcv_lfo2==true && LFO2SYNC>1)
    {
     midi_clk_rcv_lfo2=false; 
     if (lfo2clockTick > LFO2SYNC)   //MidiClock geteilt durch LFO2SYNC setzt LFO2 Counter auf 0
        {  
         LFO2Counter=0;
         lfo2clockTick = 1;      
        }
     lfo2clockTick++;   
    } 

  if(midi_clk_rcv_lfo3==true && LFO3SYNC>1)
    {
     midi_clk_rcv_lfo3=false; 
     if (lfo3clockTick > LFO3SYNC)   //MidiClock geteilt durch LFO3SYNC setzt LFO3 Counter auf 0
        {  
         LFO3Counter=0; 
         lfo3clockTick = 1;     
        }
     lfo3clockTick++;   
    }       

  if(midi_clk_rcv_efx==true && EFXMIDICLKDIV>0)  //20.000/32.000=625ms
    {
     midi_clk_rcv_efx=false;
     efxtick_start = micros(); 
     if (efxclockTick > EFXMIDICLKDIV)   //3(62),6(125),12(250),24(500),48(1000),72(1500),96(2000),127(2648)
        { 
         efxtick = efxtick_start - efxtick_end;
         efxtick_end = efxtick_start;
         uint16_t Delay_len = (efxtick/1000)*16;
         if(Delay_len>=MAX_DELAY) Delay_len=MAX_DELAY;
         delayLen_l=Delay_len;
         delayLen_r=Delay_len;
         efxclockTick = 1;
        }
     efxclockTick++;
    }  

  if(arpmode>0) //********** Arpeggiator *********************** 
    {
      arptick = millis();    
      if (!clockSync) {                      //internal Tempo Sync
        tempo = 6000 / (value[73] + 2);
        gate = map(value[78],0,127,1,tempo);
        handleTick(arptick);
       }
      else
       {                                      //external Tempo Sync über Midi
        gate = map(value[78],0,127,1,128);
        //if (clockTick==2 || arptick==lastTimeGate) 
        if (clockTick==2)
           {
            if (arptick - lastTimeGate > gate)  //Note off
               {
                lastTimeGate = arptick;
                ArpNoteOff(); 
               }          
           }  
         
        if(midi_clk_rcv==true)
          {
           midi_clk_rcv=false;
           arptick = millis(); 
           if (clockTick >= (int)map(value[75],0,127,1,8)*3 + 1) 
              { 
               handleTick(arptick);
               clockTick = 1;
              }
           clockTick++;
          }

        if(arptick - lastTimeGate > 100) 
               {
                ArpNoteOff();
               }                    
        }
       
      if (notesHeld > 0 && holdTrans!=true) digitalWrite(16, HIGH); 
      else if (notesHeld == 0 && holdTrans!=true) digitalWrite(16, LOW);
      if (blinkOn && arptick - blinkTime > 10) {
          blinkOn=false;
          digitalWrite(17, LOW);} 
          
     }  //*************** Arpeggiator Ende *******************  

if(!(LFO1SPEED_val--))
   {
    LFO1SPEED_val=255-(LFO1SPEED*2);
    if(LFO1WAVE==0){      //TRI
    if(LFO1Counter==0) LFO1Countup=true;
    if(LFO1Countup==true) LFO1Counter++;
    if(LFO1Counter==127) LFO1Countup=false;
    if(LFO1Countup==false) LFO1Counter--;
      LFO1=(LFO1Counter*LFO1DEPHT*2)>>8;}  
    if(LFO1WAVE==1){        //RECT
      LFO1Counter++;
    if(LFO1Counter<127) LFO1=(127*LFO1DEPHT)/256;
    else LFO1=(255*LFO1DEPHT)/256;} 
    if(LFO1WAVE==2){        //SAW
      LFO1Counter++;
      LFO1=(LFO1Counter/2*LFO1DEPHT*2)>>8;} 
    if(LFO1WAVE>=3){        //RND
      if(!(LFO1SPEED2_val--)){
        LFO1SPEED2_val=127;
        LFO1=(random(0,127)*LFO1DEPHT*2)>>8;}
     }
    LFO1_Led_Counter++; 
    if(LFO1_Led_Counter==0 && LFO1DEPHT>0)
      { 
       LFO1_Led_flag=true;
       LFO1_Led_millis=millis();   
       digitalWrite(ledPin1, HIGH);
      } 
   }
   
if(!(LFO2SPEED_val--))
   {
    LFO2SPEED_val=512-(LFO2SPEED*4);
    if(LFO2WAVE==0){        //TRI
    if(LFO2Counter==0) LFO2Countup=true;
    if(LFO2Countup==true) LFO2Counter++;
    if(LFO2Counter==255) LFO2Countup=false;
    if(LFO2Countup==false) LFO2Counter--;
      LFO2=LFO2Counter*LFO2DEPHT*2;}
    if(LFO2WAVE==1){        //RECT
    if(LFO2Counter==0) LFO2Countup=true;
    if(LFO2Countup==true) LFO2Counter++;
    if(LFO2Counter==255) LFO2Countup=false;
    if(LFO2Countup==false) LFO2Counter--;
      LFO2=LFO2Countup*255*LFO2DEPHT*2;}
    if(LFO2WAVE==2){        //SAW
      LFO2Counter++;
      LFO2=LFO2Counter*LFO2DEPHT*2;}
    if(LFO2WAVE>=3){        //RND
      if(!(LFO2SPEED2_val--)){
        LFO2SPEED2_val=127;       
        LFO2=random(0,255)*LFO2DEPHT*2;}}
    LFO2_Led_Counter++; 
    if(LFO2_Led_Counter==0 && LFO2DEPHT>0)
      { 
       LFO2_Led_flag=true;
       LFO2_Led_millis=millis();   
       digitalWrite(ledPin2, HIGH);
      }            
   }

  if(!(LFO3SPEED_val--))
     {
      LFO3SPEED_val=512-(LFO3SPEED*4);
      if(LFO3Counter==0) LFO3Countup=true;
      if(LFO3Countup==true) LFO3Counter++;
      if(LFO3Counter==255) LFO3Countup=false;
      if(LFO3Countup==false) LFO3Counter--;
      LFO3=LFO3Counter*LFO3DEPHT*2;
      LFO3_flag=true;
      LFO3_Led_Counter++; 
      if(LFO3_Led_Counter==0 && LFO3DEPHT>0)
        { 
         LFO3_Led_flag=true;
         LFO3_Led_millis=millis();   
         digitalWrite(ledPin3, HIGH);
        }  
     } 

  if(!(LFO4SPEED_val--))
     {
      LFO4SPEED_val=1024-(LFO4SPEED*8);
      LFO4=LFO4_Tab[LFO4_counter++];
      if(LFO4_counter==510) LFO4_counter=0;
      LFO4_flag=true; 
      }  

  if(!(LFO5SPEED_val--))
     {
      LFO5SPEED_val=1024-(LFO5SPEED*8);
      LFO5=LFO4_Tab[LFO5_counter++];
      if(LFO5_counter==510) LFO5_counter=0;
      LFO5_flag=true; 
      }                   
         
  if (!(ENVcnt--)) { //ENV every 2ms
    ENVcnt=100; 
    mil++;  
              
    for (uint8_t envcnt=0;envcnt<MAX_NOTES;envcnt++)
       {
        if (GLISSANDO>0 && noteStatus[envcnt]>0 && (Glis_val[envcnt]<0))
          {
           Glis_count[envcnt]++;
           if (Glis_count[envcnt]>=GLISSANDO)
              {
               Glis_count[envcnt]=0; 
               Glis_val[envcnt]++;
              }
           }
        if (GLISSANDO==0 && noteStatus[envcnt]>0) {Glis_val[envcnt]=0;} 
        
      if ((noteStatus[envcnt]>0) && (DCA[envcnt]<Velocity[envcnt]<<9) && Attack_flag[envcnt]==1){
        uint16_t temp=(ADSRtab[ATTACK]*Velocity[envcnt])>>7;
        DCA[envcnt]+=temp;
        temp=map(Velocity[envcnt],0,127,0,65535);
        if (DCA[envcnt]>=temp) {DCA[envcnt]=temp; Attack_flag[envcnt]=0; Decay_flag[envcnt]=1;}
        }
    
      if ((noteStatus[envcnt]>0) && Decay_flag[envcnt]==1){
        uint16_t temp=(ADSRtab[DECAY]*Velocity[envcnt])>>7;
        DCA[envcnt]-=temp;
        temp=(SUSTAIN*Velocity[envcnt])>>7;
        if (DCA[envcnt]<=temp) {DCA[envcnt]=temp; Decay_flag[envcnt]=0;}
        }  
  
      if ((noteStatus[envcnt]==0)&&(DCA[envcnt]>0)){
        uint16_t temp=(ADSRtab[RELEASE]*Velocity[envcnt])>>7;
        DCA[envcnt]-=temp;
        if (DCA[envcnt]<=0) 
           {
            DCA[envcnt]=0; 
            MIDItable[envcnt]=0; 
            Attack_flag[envcnt]=1;
            Glis_count[envcnt]=0;
            Glis_val[envcnt]=-24; 
            Velocity[envcnt]=0;         
           }
        }        
          
         if ((noteStatus[envcnt]>0) && (DCF[envcnt]<65535) && DCFAttack_flag[envcnt]==1){
          DCF[envcnt]+=ADSRtab[DCFATTACK];
          if (DCF[envcnt]>=65535){DCF[envcnt]=65535; DCFAttack_flag[envcnt]=0; DCFDecay_flag[envcnt]=1;}
          ADSR2[envcnt]=65535-DCF[envcnt];
          }
      
        if ((noteStatus[envcnt]>0)&& DCFDecay_flag[envcnt]==1){
          DCF[envcnt]-=ADSRtab[DCFDECAY];
          if (DCF[envcnt]<=DCFSUSTAIN){DCF[envcnt]=DCFSUSTAIN; DCFDecay_flag[envcnt]=0;}        
          ADSR2[envcnt]=65535-DCF[envcnt];
          }
            
        if ((noteStatus[envcnt]==0)&&(DCF[envcnt]>0)){
          DCF[envcnt]-=ADSRtab[DCFRELEASE];
          if (DCF[envcnt]<=0){DCF[envcnt]=0; DCFAttack_flag[envcnt]=1;}
          }
        } 
     }
} 

void setup(void) {
  WiFi.mode(WIFI_OFF);            
  delay(1); 
  btStop();
  delay(1); 
  Serial.begin(115200);  
  delay(100);   
  Serial2.begin(31250);
  Serial2.setRxBufferSize(1024);
  delay(100);     

  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
  i2s_set_sample_rates((i2s_port_t)i2s_num, sampleRate);
  i2s_zero_dma_buffer((i2s_port_t)i2s_num);
  delay(100);    
  
  xTaskCreatePinnedToCore(CoreTask0, "GUITask", 8000, NULL, 0, &Core0TaskHnd, 0);   
  
  timer = timerBegin(0,80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer,20,true);
  timerAlarmEnable(timer);  

  for (uint8_t i=0;i<MAX_NOTES;i++) {
    MIDItable[i]=0;
    noteStatus[i]=0;
    f[i]=65535;
    SAMPLECNT1[i]=0;
    phaccSAMPLE1[i]=0;
    SAMPLEPOS1[i]=0;
    SAMPLEINC1[i]=0;
    SAMPLECNT2[i]=0;
    phaccSAMPLE2[i]=0;
    SAMPLEPOS2[i]=0;
    SAMPLEINC2[i]=0;
    DCA[i]=0;
    DCF[i]=0;    
    ADSR2[i]=0;
    Attack_flag[i]=1;
    Decay_flag[i]=0;
    DCFAttack_flag[i]=1;
    DCFDecay_flag[i]=0;
    NoteActivationTime[i]=0;
    Glis_count[i]=0;
    Glis_val[i]=-24;
    Velocity[i]=0;        
  }
  PROGRAM=0;
  Pitchbend=0;

  for (uint8_t i=0;i<anzahl_parameter;i++) {  
      value[i]=0; 
      old_value[i]=-1;}  

  value[0]=SDCARDbank=2;    //******** DCO1 ************
  value[1]=SDCARDval=0;
  value[2]=SAMPLESTART1=80;
  value[3]=SAMPLELOO1=SDCARDWAVE1=80;
  value[4]=SAMPLELEN1=93;
  value[5]=SAMPLEINCTIME1=91;
  value[6]=SAMPLESTART2=29;
  value[7]=SAMPLELOO2=SDCARDWAVE2=29;
  value[8]=SAMPLELEN2=45;
  value[9]=SAMPLEINCTIME2=33;
  value[10]=VOL1=127;
  value[11]=VOL2=127;

  value[12]=Pitch1=63;    //*********** DCO2 ***********
  value[13]=Pitch2=63;
  value[14]=PITCHFINE1=64; 
  value[15]=PITCHFINE2=64;
  value[16]=GLISSANDO=0;
  value[17]=Velocity_Mode=0;  //Velocity Target (fixed, velo, tab1-tab6)
  value[18]=NoiseLevel=0;
  value[19]=GAIN1=0;
  value[20]=GAIN2=0;
  value[21]=0;                //LPF Cutoff
  value[22]=0;                //LFP Resonance
  value[23]=0;                //Saturation

  value[24]=FREQUENZ=127; //************* DCF ***********
  value[25]=q=0;     
  value[26]=DCFATTACK=0;
  value[27]=DCFDECAY=0;
  value[28]=127; DCFSUSTAIN=65535;
  value[29]=0;  //DCF RELEASE
  value[30]=0;  //DCF ADSR DEPTH
  value[31]=0;  //FLS (Filter Low Shelf)
  value[32]=0;  //GLS
  value[33]=0;  //FHS (Filter High Shelf)
  value[34]=0;  //GHS
  value[35]=0;  //DCFKeyFollow

  value[36]=ATTACK=0; //************** ADSR ******************
  value[37]=DECAY=0;
  value[38]=127; SUSTAIN=65535;
  value[39]=RELEASE=100;
  value[40]=0;  //ADSR Depth
  value[41]=DCFATTACK=0;
  value[42]=DCFDECAY=0;
  value[43]=127; DCFSUSTAIN=65535;
  value[44]=0;  //DCF RELEASE
  value[45]=0;  //DCF ADSR DEPTH
  value[46]=0;
  value[47]=0; 

  value[48]=LFO1WAVE=0; //************** LFO *****************
  value[49]=LFO1SPEED=63;
  value[50]=LFO1DEPHT=0;
  value[51]=LFO1SYNC=0;  
  value[52]=LFO2WAVE=0;
  value[53]=LFO2SPEED=63;
  value[54]=LFO2DEPHT=0;
  value[55]=LFO2SYNC=0; 
  value[56]=LFO3WAVE=0;
  value[57]=LFO3SPEED=0;
  value[58]=LFO3DEPHT=0;
  value[59]=LFO3SYNC=0;        
      
  value[60]=63; wet = 0.5f; //define wet 0.0 <-> 1.0
  value[61]=127; revtime = 1.0f; //define time delay 0.0 <-> 1.0 (max)
  value[62]=32768;
  value[63]=32768;
  value[64]=32768;
  value[65]=32768; 
  value[66]=StereoEFX=0;         //Stereo rechts/links EFX
  value[67]=65535;
  value[68]=65535;
  value[69]=0;                   //0=DELAY->REVERB  >0=DELAY/REVERB
  value[70]=0;                   //EFX MIDI DELAY TIME SYNC
  value[71]=0;                   //EFX Wide Stereo

  value[72]=arpmode=1;           //Arp Mode  //************ ARP ************
  value[73]=10;                  //Arp Speed
  value[74]=127;                 //Arp Vel 
  value[75]=0; clockSync=false;  //Arp Clk
  value[76]=hold=holdTrans=0;    //Arp Hold / Hold+Transpose 
  value[77]=63;                  //Arp Octave
  value[78]=gate=63;             //Arp Gate
  value[79]=ArpSwitch=0;         //Arp Notes (on/off)
  value[80]=0;
  value[81]=0;
  value[82]=0;
  value[83]=0;     

  value[84]=ChannelNote=0;       //Note Channel  //************ OPTIONS ************
  value[85]=ChannelCC=0;         //CC Channel
  value[86]=127; CCon=true;      //CC on/off
  value[87]=Wheel_Target=0;      //Wheel target
  value[88]=Aftertouch_Target=0; //Aftertouch target 
  value[89]=0;
  value[90]=0;                   //Chorus1 Level
  value[91]=LFO4SPEED=0;         //Chorus1 Speed
  value[92]=LFO4DEPHT=0;         //Chorus1 Mod Depht
  value[93]=0;                   //Chorus2 Level
  value[94]=LFO5SPEED=0;         //Chorus2 Speed
  value[95]=LFO5DEPHT=0;         //Chorus2 Mod Depht     

  cf0_lim = (int)(revtime*l_CB0);
  cf1_lim = (int)(revtime*l_CB1);
  cf2_lim = (int)(revtime*l_CB2);
  cf3_lim = (int)(revtime*l_CB3);
  ap0_lim = (int)(revtime*l_AP0);
  ap1_lim = (int)(revtime*l_AP1);
  ap2_lim = (int)(revtime*l_AP2);

  blinkTime = lastTime = millis();
  notesHeld=0;
  playBeat=0;
  blinkOn=false;
  arpUp=true;
  tempo=400;
  clockTick=1; 
  octave=0;
  octave_add=0;

  for (uint8_t audio_buff_count=0; audio_buff_count<AUDIO_BUFF; audio_buff_count++)
     {
      SYNTHSUM_TX[audio_buff_count]=0;
     }

  Biquad_Init();
  Fcls=0.05f;  //LowShelf Init, Fcls=Grenzfrequenz/sampleRate //1,6kHz
  peakGainls=0.0f;  //Init mit 0db
  Set_LowShelf_Parameter();
  Fchs=0.16f;  //HighShelf Init, Fchs=Grenzfrequenz/sampleRate //5000Hz
  peakGainhs=0.0f;
  Set_HighShelf_Parameter();   
  Fclpf=0.25f; //0.001f, 0.25f //LPF Init
  Qlpf=0.707f; //0.707, 10.0f
  Set_LPF_Parameter(); 
     
  Delay_Init();
  
  Chorus_Init();
  Chorus_SetLevel_l(0);
  Chorus_SetLevel_r(0);
  Chorus_SetLength_l(1);
  Chorus_SetLength_r(1);
  Chorus2_SetLevel_l(0);
  Chorus2_SetLevel_r(0);
  Chorus2_SetLength_l(1);
  Chorus2_SetLength_r(1);  

  timerAlarmDisable(timer);
  preferences.begin("config", false);
  PROGRAM=preferences.getUInt("program", 0);
  if(PROGRAM>127) PROGRAM=0;
  preferences.end();
  timerAlarmEnable(timer);
  button2_pressed=true;
}

void loop() {
  if (Serial2.available()) processMIDI(Serial2.read()); 

  if(LFO1_Led_flag==true && millis()-LFO1_Led_millis>100)
    {
     LFO1_Led_flag=false; 
     digitalWrite(ledPin1, LOW);     
    }
  if(LFO2_Led_flag==true && millis()-LFO2_Led_millis>100)
    {
     LFO2_Led_flag=false; 
     digitalWrite(ledPin2, LOW);     
    }
  if(LFO3_Led_flag==true && millis()-LFO3_Led_millis>100)
    {
     LFO3_Led_flag=false; 
     digitalWrite(ledPin3, LOW);     
    }        
       
  if(save_Program_changed==true)
    {      
      if(millis()-save_preferences_millis>1000)
       {  
        timerAlarmDisable(timer);
        preferences.begin("config", false);
        preferences.putUInt("program", PROGRAM);
        preferences.end();
        timerAlarmEnable(timer);
        save_Program_changed=false;
       }
   }
   
  if(LFO3_flag==true)  //LFO für Biquad LPF Cutoff
     {
      LFO3_flag=false;
      uint8_t Freq_sum = (value[21]+(LFO3>>9));
      if(Freq_sum>127) Freq_sum=127;
      Fclpf=mapFloat(Freq_sum,0,127,0.001f, 0.25f);
      Set_LPF_Parameter();    
     }

  if(LFO4_flag==true && LFO4DEPHT>0){ //LFO für Chorus1
      float value = 0.5f + ((LFO4 * NORM127MUL)/2.0f)*LFO4DEPHT*NORM127MUL; //Wert schwankt um 0.5f herum bis 0.0f und 1.0f je nach DEPHT
      if(value<0.0f) value=0.0f;
      if(value>1.0f) value=1.0f; 
      chorusLen_r = uint16_t(MAX_CHORUS*value);
      chorusLen_l = MAX_CHORUS - uint16_t(MAX_CHORUS*value);
      LFO4_flag=false;} 

  if(LFO5_flag==true && LFO5DEPHT>0){ //LFO für Chorus2
      float value = 0.5f + ((LFO5 * NORM127MUL)/2.0f)*LFO5DEPHT*NORM127MUL; //Wert schwankt um 0.5f herum bis 0.0f und 1.0f je nach DEPHT
      if(value<0.0f) value=0.0f;
      if(value>1.0f) value=1.0f; 
      chorusLen2_r = uint16_t(MAX_CHORUS*value);
      chorusLen2_l = MAX_CHORUS - uint16_t(MAX_CHORUS*value);
      LFO5_flag=false;}           
 
  if(EFXMIDICLKDIV_set2null==true){
    EFXMIDICLKDIV_set2null=false;
    Delay_SetLength_l(value[67]);
    Delay_SetLength_r(value[68]);
     }

  for (uint8_t audio_buff_count=0; audio_buff_count<AUDIO_BUFF; audio_buff_count++)
     {     
      SYNTHSUM=0;
      SYNTHSUM_TX[audio_buff_count]=SYNTHSUM;
      SYNTHTOTALSUM_L=0;
      SYNTHTOTALSUM_R=0;
      if(arpmode==0) digitalWrite(16, LOW);
      PitchSumme1=Pitchbend+PITCHtab24[Pitch1];
      PitchSumme2=Pitchbend+PITCHtab24[Pitch2];
      
      for (byte i=0; i<MAX_NOTES; i++)
        {      
         if (DCA[i])
            {
             Noise=(random(-32767,32767)*NoiseLevel)>>12; 
             if(arpmode==0) digitalWrite(16, HIGH);        
             SAMPLECNT1[i]=phaccSAMPLE1[i]>>16 & 0x00FF;
             SYNTHTOTAL[i]=((sampledata[(SAMPLEPOS1[i]*256) + SAMPLECNT1[i]])<<8)-32768;
             SYNTHTOTAL[i]=(SYNTHTOTAL[i]*DCA[i])>>(22-GAIN1);
             SYNTHTOTAL[i]=(SYNTHTOTAL[i]*VOL1)>>6;                                 
             MIDInote[i]=MIDItable[i]-12+PitchSumme1+Glis_val[i];
             f[i]=( (FREQUENZ*512) - LFO2 - ADSR2[i] - ((127-MIDInote[i])*DCFKeyFollow*4) ) & 0xffff;
             phaccSAMPLE1[i]+=(pitchSAMPLE[MIDInote[i]]+(Finetab[MIDInote[i]]*(PITCHFINE1-64+LFO1)));
             if (SAMPLECNT1[i]>=255) 
                 {
                  SAMPLEINC1[i]++;
                  if(SAMPLEINC1[i]>SAMPLEINCTIME1)
                     {            
                      if (SAMPLEPOS1[i]<SAMPLELEN1) 
                        {
                          SAMPLEPOS1[i]+=1; 
                          SAMPLEINC1[i]=0;  
                          if (counter_alt>=i) 
                            {
                             SDCARDWAVE1++; 
                             Sample1Changed=true;
                            } 
                        }
                      else 
                        {
                         SAMPLEPOS1[i]=SAMPLELOO1;
                         if (counter_alt>=i) 
                           {
                            SDCARDWAVE1=SAMPLELOO1; 
                            Sample1Changed=true;
                           }
                         }                                                                                                                                                                                                  
                   }
               }
             SAMPLECNT2[i]=phaccSAMPLE2[i]>>16 & 0x00FF; 
             int32_t Voice2=((sampledata[(SAMPLEPOS2[i]*256) + SAMPLECNT2[i]])<<8)-32768;
             Voice2=(Voice2*DCA[i])>>(22-GAIN2);
             Voice2=(Voice2*VOL2)>>6;
             SYNTHTOTAL[i]+=Voice2; 
             SYNTHTOTAL[i]+=(Noise*DCA[i])>>16;
             MIDInote[i]=MIDItable[i]-12+PitchSumme2+Glis_val[i];
             phaccSAMPLE2[i]+=(pitchSAMPLE[MIDInote[i]]+(Finetab[MIDInote[i]]*(PITCHFINE2-64+LFO1)));
             if (SAMPLECNT2[i]>=255) 
                 {
                  SAMPLEINC2[i]++;
                  if(SAMPLEINC2[i]>SAMPLEINCTIME2)
                     {            
                      if (SAMPLEPOS2[i]<SAMPLELEN2) 
                        {
                          SAMPLEPOS2[i]+=1; 
                          SAMPLEINC2[i]=0; 
                          if (counter_alt>=i) 
                            {
                             SDCARDWAVE2++; 
                             Sample2Changed=true;
                            } 
                        }
                      else 
                        {
                         SAMPLEPOS2[i]=SAMPLELOO2;
                         if (counter_alt>=i) 
                           {
                            SDCARDWAVE2=SAMPLELOO2; 
                            Sample2Changed=true;
                           }
                         }
                       }                                                                                                                                                                                                    
                   }
             filter(i);
             counter_alt=i;
           }
        }
      int16_t SYNTHTOTALSUM_L_save=SYNTHTOTALSUM_L;
      int16_t SYNTHTOTALSUM_R_save=SYNTHTOTALSUM_R;
      Chorus_Process();    
      Delay_Process();
      float sum;
      if(Struct_EFX>0) sum = (float)(SYNTHTOTALSUM_L_save+SYNTHTOTALSUM_R_save);
      else sum = (float)(SYNTHTOTALSUM_L+SYNTHTOTALSUM_R); 
      int16_t Reverb = (int16_t)(wet*Do_Reverb(sum));
      SYNTHTOTALSUM_L = SYNTHTOTALSUM_L + Reverb;
      SYNTHTOTALSUM_R = SYNTHTOTALSUM_R + Reverb;
      LowShelf_Process();
      HighShelf_Process(); 
      LPF_Process();
      WideStereo_Process();     
      SYNTHSUM = SYNTHTOTALSUM_L<<16 | (SYNTHTOTALSUM_R & 0xFFFF);
      SYNTHSUM_TX[audio_buff_count]=SYNTHSUM;
    }
  i2s_write_bytes((i2s_port_t)i2s_num, &SYNTHSUM_TX[0], AUDIO_BUFF * sizeof(uint32_t), portMAX_DELAY);            
}

inline void filter(uint8_t i){
     vi[i]=SYNTHTOTAL[i];
     vo[i] = ((vo[i] * (65535 - f[i])) + (t1[i] * f[i]))/65535; //+3db
     t1[i] = ((t1[i] * (65535 - f[i])) + (t2[i] * f[i]))/65535; //+6db
     t2[i] = ((t2[i] * (65535 - f[i])) + (t3[i] * f[i]))/65535; //+9db
     t3[i] = ((t3[i] * (65535 - f[i])) + (t4[i] * f[i]))/65535; //+12db
     t4[i] = ((t4[i] * (65535 - f[i])) + (t5[i] * f[i]))/65535; //+15db
     t5[i] = ((t5[i] * (65535 - f[i])) + (t6[i] * f[i]))/65535; //+18db
     t6[i] = ((t6[i] * (65535 - f[i])) + (t7[i] * f[i]))/65535; //+21db
     t7[i] = ((t7[i] * (65535 - f[i])) + (t8[i] * f[i]))/65535; //+24db
     t8[i] = vi[i]-((vo[i]*q)/65535);              //resonance feedback
     if (i%2) {SYNTHTOTALSUM_L+=vo[i]; SYNTHTOTALSUM_R+=(vo[i]*(StereoEFX*516))>>16;}
     else {SYNTHTOTALSUM_L+=(vo[i]*(StereoEFX*516))>>16; SYNTHTOTALSUM_R+=vo[i];}
}

//******************* MIDI ********************************************************

void processMIDI(uint8_t MIDIRX) {
if ((MIDIRX>0xEF)&&(MIDIRX<0xF8)) { MIDIRUNNINGSTATUS=0; MIDISTATE=0; if(MidiThru==true) Serial2.write(MIDIRX); return; } 
if (MIDIRX==0xF8) {
  midi_clk_rcv=true;
  midi_clk_rcv_efx=true; 
  midi_clk_rcv_lfo1=true;
  midi_clk_rcv_lfo2=true;
  midi_clk_rcv_lfo3=true;
  if(MidiThru==true) Serial2.write(MIDIRX);
  return;
}
if (MIDIRX>0xF8) {if(MidiThru==true) Serial2.write(MIDIRX); return;}
if (MIDIRX & 0x80) {
  MIDIRUNNINGSTATUS=MIDIRX;
  MIDISTATE=1;
  if(MidiThru==true) Serial2.write(MIDIRX);
  return;
}
if (MIDIRX < 0x80) {
  if (!MIDIRUNNINGSTATUS) return;
  if (MIDISTATE==1) {
    MIDINOTE=MIDIRX;
    if(MidiThru==true) Serial2.write(MIDIRX);
    if((MIDIRUNNINGSTATUS >= 0xC0) && (MIDIRUNNINGSTATUS <= 0xCF) && CCon>0) {handleMIDIPC(MIDINOTE); MIDISTATE=1;}
    else MIDISTATE++;
    return;
  }
if (MIDISTATE==2) {
  MIDIVEL=MIDIRX;
  if(MidiThru==true) Serial2.write(MIDIRX);
  MIDISTATE=1;
  if (MIDIRUNNINGSTATUS==0x80+ChannelNote) handleMIDInoteOFF(MIDIRUNNINGSTATUS,MIDINOTE,MIDIVEL);
  if (MIDIRUNNINGSTATUS==0x90+ChannelNote && MIDIVEL==0) handleMIDInoteOFF(MIDIRUNNINGSTATUS,MIDINOTE,MIDIVEL);
  if (MIDIRUNNINGSTATUS==0x90+ChannelNote && MIDIVEL>0) handleMIDInoteON(MIDIRUNNINGSTATUS,MIDINOTE,MIDIVEL);
  if (MIDIRUNNINGSTATUS==0xB0+ChannelCC && CCon>0) handleMIDICC(MIDINOTE,MIDIVEL);
  if (MIDIRUNNINGSTATUS==0xD0+ChannelNote && Aftertouch_Target>0) handleMIDIAT(MIDIRUNNINGSTATUS,MIDINOTE,MIDIVEL);
  if (MIDIRUNNINGSTATUS==0xE0+ChannelCC && CCon>0) handleMIDIpitchbend(MIDINOTE,MIDIVEL);
  }
 }
}

void handleMIDIPC(uint8_t Cvalue){
    PROGRAM=Cvalue;
    Program_Changed=true;
}

void handleMIDIAT(uint8_t channel, uint8_t note, uint8_t velocity){
    if(Aftertouch_Target==1) FREQUENZ=velocity;
    if(Aftertouch_Target==2) q=velocity<<9;
    if(Aftertouch_Target==3) {
      if(velocity>50){
         for (byte i=0; i<MAX_NOTES; i++) {SAMPLEPOS1[i]=0; SAMPLEPOS2[i]=0;}}
      }
}

void handleMIDIpitchbend(uint8_t controller, uint8_t Cvalue){
    Pitchbend=PITCHtab[Cvalue];
}

void handleMIDICC(uint8_t controller, uint8_t Cvalue){
      
     if(controller==0x01) 
       {
        if(Wheel_Target==1) LFO1DEPHT=Cvalue;
        if(Wheel_Target==2) LFO2DEPHT=Cvalue;
        if(Wheel_Target==3) FREQUENZ=Cvalue;
        if(Wheel_Target==4) q=Cvalue<<9;
        } 
        
      if(controller==0x0E)    //BPM
        {
         GLISSANDO=value[20]=Cvalue; 
         for(byte i = 0; i < MAX_NOTES; i++)
            {
             Glis_count[i]=0;
             Glis_val[i]=-24;   
            }
         }

      if(edit_page==0) //wenn OSC1 SHAPE = 0
         { 
          if(controller==0x56) {}                                      //ENV3 AMOUNT
          if(controller==0x59) {value[62]=Cvalue;} //ENV3 ATTACK
          if(controller==0x5A) {value[63]=Cvalue;} //ENV3 DECAY
          if(controller==0x4D) {value[64]=Cvalue;} //ENV3 SUSTAIN
          if(controller==0x4E) {value[65]=Cvalue;} //ENV3 RELEASE
         }         

      if(edit_page==1) //wenn OSC1 SHAPE = 1
         { 
          if(controller==0x56) {} //ENV3 AMOUNT
          if(controller==0x59) {LFO1WAVE=Cvalue>>5; value[48]=Cvalue;} //ENV3 ATTACK
          if(controller==0x5A) {LFO1SPEED=value[49]=Cvalue;} //ENV3 DECAY
          if(controller==0x4D) {LFO1DEPHT=value[50]=Cvalue;} //ENV3 SUSTAIN
          if(controller==0x4E) {LFO1SYNC=value[51]=Cvalue;} //ENV3 RELEASE 
         }
      else if (edit_page==2) //wenn OSC1 SHAPE = 2
         {
          if(controller==0x56) {} //ENV3 AMOUNT
          if(controller==0x59) {LFO2WAVE=Cvalue>>5; value[52]=Cvalue;} //ENV3 ATTACK
          if(controller==0x5A) {LFO2SPEED=value[53]=Cvalue;} //ENV3 DECAY
          if(controller==0x4D) {LFO2DEPHT=value[54]=Cvalue;} //ENV3 SUSTAIN
          if(controller==0x4E) {LFO2SYNC=value[55]=Cvalue;} //ENV3 RELEASE 
         }
      else if (edit_page==3) //wenn OSC1 SHAPE = 3
         {
          if(controller==0x56) {} //ENV3 AMOUNT
          if(controller==0x59) {SAMPLESTART1=value[2]=Cvalue;} //ENV3 ATTACK
          if(controller==0x5A) {SAMPLELOO1=SDCARDWAVE1=value[3]=Cvalue;} // ShowSample_1();} //ENV3 DECAY
          if(controller==0x4D) {SAMPLELEN1=value[4]=Cvalue;} //ENV3 SUSTAIN
          if(controller==0x4E) {SAMPLEINCTIME1=value[5]=Cvalue;} //ENV3 RELEASE 
         }
      else if (edit_page==4) //wenn OSC2 SHAPE = 0
         {
          if(controller==0x56) {} //ENV3 AMOUNT
          if(controller==0x59) {LFO3WAVE=Cvalue>>5; value[56]=Cvalue;} //ENV3 ATTACK
          if(controller==0x5A) {LFO3SPEED=value[57]=Cvalue;} //ENV3 DECAY
          if(controller==0x4D) {LFO3DEPHT=value[58]=Cvalue;} //ENV3 SUSTAIN
          if(controller==0x4E) {LFO3SYNC=value[59]=Cvalue;} //ENV3 RELEASE 
         }         
      else if (edit_page==51) //wenn OSC1 SHAPE = 4
         {
          if(controller==0x56) {} //ENV3 AMOUNT
          if(controller==0x59) {SAMPLESTART2=value[6]=Cvalue;} //ENV3 ATTACK
          if(controller==0x5A) {SAMPLELOO2=SDCARDWAVE2=value[7]=Cvalue;} // ShowSample_2();} //ENV3 DECAY
          if(controller==0x4D) {SAMPLELEN2=value[8]=Cvalue;} //ENV3 SUSTAIN
          if(controller==0x4E) {SAMPLEINCTIME2=value[9]=Cvalue;} //ENV3 RELEASE     
         }     
             
      if(controller==0x6A) {VOL1=value[10]=Cvalue;} //FILTER ENV AMT
      if(controller==0x6d) {DCFATTACK=value[26]=Cvalue;} //FILTER ATTACK 
      if(controller==0x6E) {DCFDECAY=value[27]=Cvalue;} //FILTER DECAY                       
      if(controller==0x6F) {value[28]=Cvalue; if (Cvalue<127) DCFSUSTAIN=((Cvalue+1)<<9); else DCFSUSTAIN=65535;} //FILTER SUSTAIN 
      if(controller==0x70) {value[29]=Cvalue;} //FILTER RELEASE 
      
      if(controller==0x73) {VOL2=value[11]=Cvalue;} //AMP AMOUNT                                     
      if(controller==0x76) {ATTACK=value[36]=Cvalue;} //AMP ATTACK           
      if(controller==0x77) {DECAY=value[37]=Cvalue;} //AMP DECAY       
      if(controller==0x4B) {value[38]=Cvalue; if (Cvalue<127) SUSTAIN=((Cvalue+1)<<9); else SUSTAIN=65535;} //AMP SUSTAIN 
      if(controller==0x4c) {RELEASE=value[39]=Cvalue;Cvalue;} //AMP RELEASE 
       
      if(controller==0x14) {SDCARDval=value[1]=Cvalue; WavetableChanged=true;} //OSC1 FREQ //if(Cvalue<32) SDCARDval=value[1]=Cvalue; else SDCARDval=value[1]=31;
      if(controller==0x16) {edit_page=Cvalue;} //OSC1 SHAPE (0=OFF, 1-4 = Wellenformen)
      if(controller==0x17) {Pitch1=value[12]=Cvalue;} //GLIDE1 //SDCARDval=value[1]=Cvalue/4; LoadSample(); ShowSample_1();
      if(controller==0x1E) {PITCHFINE1=value[14]=Cvalue;} //SUB OSC1    
  
      if(controller==0x18) {SDCARDval=value[1]=Cvalue; WavetableChanged=true;} //OSC2 FREQ
      if(controller==0x1A) {edit_page=Cvalue+4;} //OSC2 SHAPE (0=OFF, 1-4 = Wellenformen)              
      if(controller==0x1B) {Pitch2=value[13]=Cvalue;} //GLIDE2
      if(controller==0x1F) {PITCHFINE2=value[15]=Cvalue;} //SUB OSC2
                                  
      if(controller==0x1C) //MIXsound
        {
         value[60]=Cvalue; 
         wet=float(value[60]/127.0f);        
        } 
      if(controller==0x34)  //FEEDBACK/EXT VOL 
        {
         value[61]=Cvalue; 
         revtime=float((value[61]+5)/132.0f);
         cf0_p=0, cf1_p=0, cf2_p=0, cf3_p=0, ap0_p=0, ap1_p=0, ap2_p=0;
         cf0_lim = (int)(revtime*l_CB0);
         cf1_lim = (int)(revtime*l_CB1);
         cf2_lim = (int)(revtime*l_CB2);
         cf3_lim = (int)(revtime*l_CB3);
         ap0_lim = (int)(revtime*l_AP0);
         ap1_lim = (int)(revtime*l_AP1);
         ap2_lim = (int)(revtime*l_AP2);         
        } 
      if(controller==0x1D) //NOISE
        {
         NoiseLevel=value[18]=Cvalue;
        } 
       
//      if(controller==0x66) {FREQUENZ=FREQUENZ_save=value[24]=Cvalue;} //FILTER FREQ
//      if(controller==0x67) {q=q_save=Cvalue<<9; value[25]=Cvalue;} //FILTER RES 

      if(controller==0x66) {value[21]=Cvalue; Fclpf=mapFloat(Cvalue,0,127,0.001f, 0.25f); Set_LPF_Parameter();} //FILTER FREQ SUMME
      if(controller==0x67) {value[22]=Cvalue; Qlpf=mapFloat(Cvalue,0,127,0.707, 10.0f); Set_LPF_Parameter();} //FILTER RES SUMME         
      if(controller==0x69) {StereoEFX=value[66]=Cvalue;} //FILTER AUDIO MOD     
}

void handleMIDInoteON(uint8_t channel, uint8_t note, uint8_t velocity) {

  if(arpmode>0)   //ARP MODE START
    {
      if (holdTrans==true)    //Hold + Transpose
         {
          ArpTranspose=note-60; //Basisnote C3
          digitalWrite(16, HIGH);
          return;  
         }
      else
         {
          ArpTranspose=0; 
          if (notesHeld==0) resetNotes();
          notesHeld++;
          if (notesHeld==1) {firstNotePressed=true; octave_add=0;}
          for (int i = 0; i < sizeof(notes); i++) 
              {
                if (notes[i] == note) return;   // already in arpeggio
                else if (notes[i] != '\0') continue;
                else 
                   {
                     for (int j = sizeof(notes); j > i; j--)
                     value[i+96] = notes[i] = note;
                     value[i+96+sizeof(notes)] = arpvel[i] = velocity;
                     return;        
                    }
               }
           }    
     } //ARP MODE ENDE   

    int32_t slot = -1;  // a slot of -1 means that no slot has been selected 
    if (LFO1SYNC==1) LFO1Counter=0;
    if (LFO2SYNC==1) LFO2Counter=0;
    if (LFO3SYNC==1) LFO3Counter=0;
    for(byte i = 0; i < MAX_NOTES; i++)
       {
        if(MIDItable[i]==0)
          { 
           Sample1Changed=true;
           Sample2Changed=true;  
           noteStatus[i]=velocity;
           MIDItable[i]=note;
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
           DCF[i]=1000;
           ADSR2[i]=0;
           DCFAttack_flag[i]=1; 
           DCFDecay_flag[i]=0;
           Attack_flag[i]=1;
           Decay_flag[i]=0;
           NoteActivationTime[i]=mil;
           Glis_count[i]=0; 
           Glis_val[i]=-24;           
           slot=i;
           switch (Velocity_Mode) {
              case 0:
              Velocity[i]=127;
              FREQUENZ=FREQUENZ_save; 
              q=q_save;        
              break;
              case 1:
              Velocity[i]=velocity;
              FREQUENZ=FREQUENZ_save; 
              q=q_save;          
              break;
              case 2:
              Velocity[i]=VeloINC1tab[velocity];
              FREQUENZ=FREQUENZ_save; 
              q=q_save;        
              break;
              case 3:
              Velocity[i]=VeloINC2tab[velocity];
              FREQUENZ=FREQUENZ_save; 
              q=q_save;        
              break; 
              case 4:
              Velocity[i]=VeloINC3tab[velocity];
              FREQUENZ=FREQUENZ_save; 
              q=q_save;       
              break;  
              case 5:
              Velocity[i]=VeloDEC1tab[velocity];
              FREQUENZ=FREQUENZ_save; 
              q=q_save;       
              break;  
              case 6:
              Velocity[i]=127;
              q=velocity<<9;
              FREQUENZ=FREQUENZ_save; 
              break;
              case 7:
              Velocity[i]=127;
              FREQUENZ=velocity;
              q=q_save;       
              break;                                       
              }           
           return;
          }
        }

  if (slot == -1) 
    {
      slot = 0;                                   // to make sure slot will always be a valid value
      uint32_t lowestTime = 0xFFFFFFFF;           // initialize to max uint32_t value
      for (byte n=0; n < MAX_NOTES; n++) 
      {
        if (NoteActivationTime[n] < lowestTime)
        {
          lowestTime = NoteActivationTime[n];     // update the lowestTime
          slot = n;                               // update the current found slot
        }
      }
     Attack_flag[slot]=1;
     noteStatus[slot]=velocity;
     MIDItable[slot]=note;
     DCA[slot]=1000;
     DCF[slot]=1000;
     ADSR2[slot]=0;
     DCFAttack_flag[slot]=1; 
     DCFDecay_flag[slot]=0;
     SAMPLECNT1[slot]=0;
     SAMPLEINC1[slot]=0;
     phaccSAMPLE1[slot]=0;
     SAMPLEPOS1[slot]=SAMPLESTART1;
     SDCARDWAVE1=SAMPLESTART1;
     SAMPLECNT2[slot]=0;
     SAMPLEINC2[slot]=0;
     phaccSAMPLE2[slot]=0;
     SAMPLEPOS2[slot]=SAMPLESTART2; 
     Glis_count[slot]=0; 
     Glis_val[slot]=-24;
     switch (Velocity_Mode) {
        case 0:
        Velocity[slot]=127;
        FREQUENZ=FREQUENZ_save; 
        q=q_save;       
        break;
        case 1:
        Velocity[slot]=velocity; 
        FREQUENZ=FREQUENZ_save; 
        q=q_save;              
        break;
        case 2:
        Velocity[slot]=VeloINC1tab[velocity];
        FREQUENZ=FREQUENZ_save; 
        q=q_save;            
        break;
        case 3:
        Velocity[slot]=VeloINC2tab[velocity];
        FREQUENZ=FREQUENZ_save; 
        q=q_save;              
        break; 
        case 4:
        Velocity[slot]=VeloINC3tab[velocity];
        FREQUENZ=FREQUENZ_save; 
        q=q_save;             
        break;  
        case 5:
        Velocity[slot]=VeloDEC1tab[velocity];
        FREQUENZ=FREQUENZ_save; 
        q=q_save;              
        break;  
        case 6:
        Velocity[slot]=127;
        q=velocity<<9;
        FREQUENZ=FREQUENZ_save;       
        break;
        case 7:
        Velocity[slot]=127;
        FREQUENZ=velocity;
        q=q_save;       
        break;                                       
      }     
    }
     NoteActivationTime[slot]=mil;        
}

void handleMIDInoteOFF(uint8_t channel, uint8_t note, uint8_t velocity) {

  if(arpmode>0)
    {
     if (holdTrans==true || notesHeld==0)   //Arp Hold + Transpose
        {
         digitalWrite(16, LOW); 
         // tue nichts wenn Note Off         
        }
      else
        {
          notesHeld--;
          for (int i = 0; i < sizeof(notes); i++) 
              {
               if (!hold && notes[i] >= note) 
                  { 
                   if (i < sizeof(notes)) {value[i+96] = notes[i] = notes[i+1]; arpvel[i] = arpvel[i+1];}
                  }
               }
          if (notesHeld == 0) noteStatus[0]=0;          
        }
      }
        
    for(byte i = 0; i < MAX_NOTES; i++){
        if(MIDItable[i]==note){noteStatus[i]=0;} // Glis_count[i]=0; Glis_val[i]=-24;}
       }          
}
