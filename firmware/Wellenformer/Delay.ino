/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Delay.ino
 *
 *  Description:
 *      Stereo delay effect implementation.
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

int16_t *delayLine_l;
int16_t *delayLine_r;
uint16_t delayToMix_l = 65535;
uint16_t delayToMix_r = 65535;
uint16_t delayFeedback_l = 32768;
uint16_t delayFeedback_r = 32768;

uint16_t delayIn_l = 0;
uint16_t delayIn_r = 0;
uint16_t delayOut_l = 0;
uint16_t delayOut_r = 0;

void Delay_Init(void)
{ 
    delayLine_l = (int16_t *)malloc(sizeof(int16_t) * MAX_DELAY);
    if (delayLine_l == NULL)
    {
        Serial.printf("No more heap memory!\n");
    }
    delayLine_r = (int16_t *)malloc(sizeof(int16_t) * MAX_DELAY);
    if (delayLine_r == NULL)
    {
        Serial.printf("No more heap memory!\n");
    }
    Delay_Reset();
}

void Delay_Reset(void)
{
    for (int i = 0; i < MAX_DELAY; i++)
    {
        delayLine_l[i] = 0;
        delayLine_r[i] = 0;
    }
}

void Delay_Process()
{
    delayLine_l[delayIn_l] = SYNTHTOTALSUM_L;
    delayLine_r[delayIn_r] = SYNTHTOTALSUM_R;

    delayOut_l = delayIn_l + (MAX_DELAY - delayLen_l);

    if (delayOut_l >= MAX_DELAY)
    {
        delayOut_l -= MAX_DELAY;
    }
    delayOut_r = delayIn_r + (MAX_DELAY - delayLen_r);

    if (delayOut_r >= MAX_DELAY)
    {
        delayOut_r -= MAX_DELAY;
    }    

    SYNTHTOTALSUM_L += (delayLine_l[delayOut_l] * delayToMix_l)>>16;
    SYNTHTOTALSUM_R += (delayLine_r[delayOut_r] * delayToMix_r)>>16;

    delayLine_l[delayIn_l] += (delayLine_l[delayOut_l] * delayFeedback_l)>>16;
    delayLine_r[delayIn_r] += (delayLine_r[delayOut_r] * delayFeedback_r)>>16;

    delayIn_l ++;
    delayIn_r ++;

    if (delayIn_l >= MAX_DELAY)
    {
        delayIn_l = 0;
    }
    if (delayIn_r >= MAX_DELAY)
    {
        delayIn_r = 0;
    }    
}

void Delay_SetFeedback_l(uint16_t value)
{
    delayFeedback_l = value;
}
void Delay_SetFeedback_r(uint16_t value)
{
    delayFeedback_r = value;
}

void Delay_SetLevel_l(uint16_t value)
{
    delayToMix_l = value;
}
void Delay_SetLevel_r(uint16_t value)
{
    delayToMix_r = value;
}

void Delay_SetLength_l(uint8_t value) //0...127
{
    delayLen_l = (MAX_DELAY/127)*value;
}
void Delay_SetLength_r(uint8_t value)
{
    delayLen_r = (MAX_DELAY/127)*value; 
}
