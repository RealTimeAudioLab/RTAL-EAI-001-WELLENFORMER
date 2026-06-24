/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Chorus.ino
 *
 *  Description:
 *      Stereo chorus delay-line effect.
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

int16_t *chorusLine_l;
int16_t *chorusLine_r;
uint16_t chorusIn_l = 0;
uint16_t chorusIn_r = 0;
uint16_t chorusOut_l = 0;
uint16_t chorusOut_r = 0;

int16_t *chorusLine2_l;
int16_t *chorusLine2_r;
uint16_t chorusIn2_l = 0;
uint16_t chorusIn2_r = 0;
uint16_t chorusOut2_l = 0;
uint16_t chorusOut2_r = 0;

void Chorus_Init(void)
{ 
    chorusLine_l = (int16_t *)malloc(sizeof(int16_t) * MAX_CHORUS);
    if (chorusLine_l == NULL)
    {
        Serial.printf("No more heap memory!\n");
    }
    chorusLine_r = (int16_t *)malloc(sizeof(int16_t) * MAX_CHORUS);
    if (chorusLine_r == NULL)
    {
        Serial.printf("No more heap memory!\n");
    }

    chorusLine2_l = (int16_t *)malloc(sizeof(int16_t) * MAX_CHORUS);
    if (chorusLine2_l == NULL)
    {
        Serial.printf("No more heap memory!\n");
    }
    chorusLine2_r = (int16_t *)malloc(sizeof(int16_t) * MAX_CHORUS);
    if (chorusLine2_r == NULL)
    {
        Serial.printf("No more heap memory!\n");
    }
    Chorus_Reset();    
}

void Chorus_Reset(void)
{
    for (int i = 0; i < MAX_CHORUS; i++)
    {
        chorusLine_l[i] = 0;
        chorusLine_r[i] = 0;
        chorusLine2_l[i] = 0;
        chorusLine2_r[i] = 0;        
    }
}

inline void Chorus_Process()
{      
    chorusLine_l[chorusIn_l] = SYNTHTOTALSUM_L>>1;
    chorusLine_r[chorusIn_r] = SYNTHTOTALSUM_R>>1;
    chorusLine2_l[chorusIn_l] = SYNTHTOTALSUM_L>>1;
    chorusLine2_r[chorusIn_r] = SYNTHTOTALSUM_R>>1;

    chorusOut_l = chorusIn_l + (1 + MAX_CHORUS - chorusLen_l);
    chorusOut2_l = chorusIn2_l + (1 + MAX_CHORUS - chorusLen2_l);

    if (chorusOut_l >= MAX_CHORUS) chorusOut_l -= MAX_CHORUS;
    if (chorusOut2_l >= MAX_CHORUS) chorusOut2_l -= MAX_CHORUS;
  
    chorusOut_r = chorusIn_r + (1 + MAX_CHORUS - chorusLen_r);
    chorusOut2_r = chorusIn2_r + (1 + MAX_CHORUS - chorusLen2_r);

    if (chorusOut_r >= MAX_CHORUS) chorusOut_r -= MAX_CHORUS;
    if (chorusOut2_r >= MAX_CHORUS) chorusOut2_r -= MAX_CHORUS;      

    SYNTHTOTALSUM_L += ((chorusLine_l[chorusOut_l] * chorusToMix_l)>>16) + ((chorusLine2_l[chorusOut2_l] * chorusToMix2_l)>>16);
    SYNTHTOTALSUM_R += ((chorusLine_r[chorusOut_r] * chorusToMix_r)>>16) + ((chorusLine2_r[chorusOut2_r] * chorusToMix2_r)>>16);

    chorusIn_l ++;
    chorusIn_r ++;
    chorusIn2_l ++;
    chorusIn2_r ++;

    if (chorusIn_l >= MAX_CHORUS) chorusIn_l = 0;
    if (chorusIn_r >= MAX_CHORUS) chorusIn_r = 0;
    if (chorusIn2_l >= MAX_CHORUS) chorusIn2_l = 0;
    if (chorusIn2_r >= MAX_CHORUS) chorusIn2_r = 0;      
}

void Chorus_SetLevel_l(uint16_t value)
{
    chorusToMix_l = value;
}
void Chorus_SetLevel_r(uint16_t value)
{
    chorusToMix_r = value;
}

void Chorus_SetLength_l(uint8_t value)
{
    chorusLen_l = (MAX_CHORUS/127)*value;
}
void Chorus_SetLength_r(uint8_t value)
{
    chorusLen_r = (MAX_CHORUS/127)*value; 
}

void Chorus2_SetLevel_l(uint16_t value)
{
    chorusToMix2_l = value;
}
void Chorus2_SetLevel_r(uint16_t value)
{
    chorusToMix2_r = value;
}

void Chorus2_SetLength_l(uint8_t value)
{
    chorusLen2_l = (MAX_CHORUS/127)*value;
}
void Chorus2_SetLength_r(uint8_t value)
{
    chorusLen2_r = (MAX_CHORUS/127)*value; 
}
