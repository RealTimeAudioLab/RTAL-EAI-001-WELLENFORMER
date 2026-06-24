/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      WideStereo.ino
 *
 *  Description:
 *      Stereo width processing.
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

inline void WideStereo_Process()
{
    float WideCoeff = EFX_Wide_Stereo * NORM127MUL;
    float MonoSign = ((float)(SYNTHTOTALSUM_L) + (float)(SYNTHTOTALSUM_R))/2.0;
    float Stereo = (float)(SYNTHTOTALSUM_L) - (float)(SYNTHTOTALSUM_R);
    Stereo = Stereo * WideCoeff;
    SYNTHTOTALSUM_L = (int16_t)(MonoSign + Stereo); 
    SYNTHTOTALSUM_R = (int16_t)(MonoSign - Stereo);
}  
 
