/******************************************************************************
 *
 *  RTAL Engineering Heritage Collection
 *
 *  Project:
 *      RTAL-WSYN-001 WELLENFORMER
 *
 *  Module:
 *      Biquad.ino
 *
 *  Description:
 *      Biquad filter definitions and DSP helper functions.
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

#define jo_type_lowpass 0
#define jo_type_highpass 1
#define bq_type_lowpass 2
#define bq_type_highpass 3
#define bq_type_bandpass 4
#define bq_type_notch 5
#define bq_type_peak 6
#define bq_type_lowshelf 7
#define bq_type_highshelf 8

void Biquad_Clac(uint8_t Type, float Fc, float Q, float peakGain)
{
    float norm;
    float V = pow(10, fabs(peakGain) / 20.0);
    float K = tan(M_PI * Fc);
    switch (Type) {
        case jo_type_lowpass:{}
        case jo_type_highpass:{}
        case bq_type_lowpass:{  
            norm = 1 / (1 + K / Q + K * K);
            a0 = K * K * norm;
            a1 = 2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;}

        case bq_type_highpass:{
            norm = 1 / (1 + K / Q + K * K);
            a0 = 1 * norm;
            a1 = -2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;            
            break;}            

        case bq_type_bandpass:{
            norm = 1 / (1 + K / Q + K * K);
            a0 = K / Q * norm;
            a1 = 0;
            a2 = -a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;            
            break;}
            
        case bq_type_notch:{
            norm = 1 / (1 + K / Q + K * K);
            a0 = (1 + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = a0;
            b1 = a1;
            b2 = (1 - K / Q + K * K) * norm;          
            break;}

        case bq_type_peak:{
            if (peakGain >= 0) {    // boost
                norm = 1 / (1 + 1/Q * K + K * K);
                a0 = (1 + V/Q * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - V/Q * K + K * K) * norm;
                b1 = a1;
                b2 = (1 - 1/Q * K + K * K) * norm;
            }
            else {    // cut
                norm = 1 / (1 + V/Q * K + K * K);
                a0 = (1 + 1/Q * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - 1/Q * K + K * K) * norm;
                b1 = a1;
                b2 = (1 - V/Q * K + K * K) * norm;               
            }
            break;}
            
        case bq_type_lowshelf:{
            if (peakGain >= 0) {    // boost
                norm = 1 / (1 + sqrt(2) * K + K * K);
                a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
                a1 = 2 * (V * K * K - 1) * norm;
                a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - sqrt(2) * K + K * K) * norm;
            }
            else {    // cut
                norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
                a0 = (1 + sqrt(2) * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - sqrt(2) * K + K * K) * norm;
                b1 = 2 * (V * K * K - 1) * norm;
                b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
            }
            break;}           
            
        case bq_type_highshelf:{
            if (peakGain >= 0) {    // boost
                norm = 1 / (1 + sqrt(2) * K + K * K);
                a0 = (V + sqrt(2*V) * K + K * K) * norm;
                a1 = 2 * (K * K - V) * norm;
                a2 = (V - sqrt(2*V) * K + K * K) * norm;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - sqrt(2) * K + K * K) * norm;
            }
            else {    // cut
                norm = 1 / (V + sqrt(2*V) * K + K * K);
                a0 = (1 + sqrt(2) * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - sqrt(2) * K + K * K) * norm;
                b1 = 2 * (K * K - V) * norm;
                b2 = (V - sqrt(2*V) * K + K * K) * norm;
            }
            break;}

        default:
        break;       
       }
}

void Biquad_Init()
{   
    a0 = 1.0;
    a1 = a2 = b1 = b2 = 0.0;
    Fc = 0.50;
    Q = 0.707;
    peakGain = 0.0;
    l_z1 = l_z2 = r_z1 = r_z2 = 0.0;
}

inline void Biquad_Process(uint16_t i)
{
    float l_inp = (float)(SYNTHTOTALSUM_L);
    float l_out = l_inp * a0 + l_z1;
    l_z1 = l_inp * a1 + l_z2 - b1 * l_out;
    l_z2 = l_inp * a2 - b2 * l_out;
    SYNTHTOTALSUM_L = (int16_t)l_out;

    float r_inp = (float)(SYNTHTOTALSUM_R);
    float r_out = r_inp * a0 + r_z1;
    r_z1 = r_inp * a1 + r_z2 - b1 * r_out;
    r_z2 = r_inp * a2 - b2 * r_out;
    SYNTHTOTALSUM_R = (int16_t)r_out;

}

//****************************************************************************************
inline void Set_LPF_Parameter()
{
  float normlpf;
  float Klpf = tan(M_PI * Fclpf);
  normlpf = 1 / (1 + Klpf / Qlpf + Klpf * Klpf);
  a0lpf = Klpf * Klpf * normlpf;
  a1lpf = 2 * a0lpf;
  a2lpf = a0lpf;
  b1lpf = 2 * (Klpf * Klpf - 1) * normlpf;
  b2lpf = (1 - Klpf / Qlpf + Klpf * Klpf) * normlpf; 
}

inline void LPF_Process()
{
    float l_inlpf = (float)(SYNTHTOTALSUM_L);
    float l_outlpf = l_inlpf * a0lpf + l_z1lpf;
    l_z1lpf = l_inlpf * a1lpf + l_z2lpf - b1lpf * l_outlpf;
    l_z2lpf = l_inlpf * a2lpf - b2lpf * l_outlpf;
    SYNTHTOTALSUM_L = (int16_t)l_outlpf;

    float r_inlpf = (float)(SYNTHTOTALSUM_R);
    float r_outlpf = r_inlpf * a0lpf + r_z1lpf;
    r_z1lpf = r_inlpf * a1lpf + r_z2lpf - b1lpf * r_outlpf;
    r_z2lpf = r_inlpf * a2lpf - b2lpf * r_outlpf;
    SYNTHTOTALSUM_R = (int16_t)r_outlpf;
}

//* HPF **********************************************************************
inline void Set_HPF_Parameter()
{
  float normhpf;
  float Khpf = tan(M_PI * Fchpf);
  normhpf = 1 / (1 + Khpf / Qhpf + Khpf * Khpf);
  a0hpf = 1 * normhpf;
  a1hpf = -2 * a0hpf;
  a2hpf = a0hpf;
  b1hpf = 2 * (Khpf * Khpf - 1) * normhpf;
  b2hpf = (1 - Khpf / Qhpf + Khpf * Khpf) * normhpf;
}

inline void HPF_Process()
{
    float l_inhpf = (float)(SYNTHTOTALSUM_L);
    float l_outhpf = l_inhpf * a0hpf + l_z1hpf;
    l_z1hpf = l_inhpf * a1hpf + l_z2hpf - b1hpf * l_outhpf;
    l_z2hpf = l_inhpf * a2hpf - b2hpf * l_outhpf;
    SYNTHTOTALSUM_L = (int16_t)l_outhpf;

    float r_inhpf = (float)(SYNTHTOTALSUM_R);
    float r_outhpf = r_inhpf * a0hpf + r_z1hpf;
    r_z1hpf = r_inhpf * a1hpf + r_z2hpf - b1hpf * r_outhpf;
    r_z2hpf = r_inhpf * a2hpf - b2hpf * r_outhpf;
    SYNTHTOTALSUM_R = (int16_t)r_outhpf;
}

//* BPF **********************************************************************
inline void Set_BPF_Parameter()
{
  float normbpf;
  float Kbpf = tan(M_PI * Fcbpf);
  normbpf = 1 / (1 + Kbpf / Qbpf + Kbpf * Kbpf);
  a0bpf = Kbpf / Qbpf * normbpf;
  a1bpf = 0;
  a2bpf = -a0bpf;
  b1bpf = 2 * (Kbpf * Kbpf - 1) * normbpf;
  b2bpf = (1 - Kbpf / Qbpf + Kbpf * Kbpf) * normbpf;
}

inline void BPF_Process()
{
    float l_inbpf = (float)(SYNTHTOTALSUM_L);
    float l_outbpf = l_inbpf * a0bpf + l_z1bpf;
    l_z1bpf = l_inbpf * a1bpf + l_z2bpf - b1bpf * l_outbpf;
    l_z2bpf = l_inbpf * a2bpf - b2bpf * l_outbpf;
    SYNTHTOTALSUM_L = (int16_t)l_outbpf;

    float r_inbpf = (float)(SYNTHTOTALSUM_R);
    float r_outbpf = r_inbpf * a0bpf + r_z1bpf;
    r_z1bpf = r_inbpf * a1bpf + r_z2bpf - b1bpf * r_outbpf;
    r_z2bpf = r_inbpf * a2bpf - b2bpf * r_outbpf;
    SYNTHTOTALSUM_R = (int16_t)r_outbpf;
}            

//* LowShelf **********************************************************************
void Set_LowShelf_Parameter()
{
      float normls;
      float Vls = pow(10, fabs(peakGainls) / 20.0);
      float Kls = tan(M_PI * Fcls);  
  if (peakGainls >= 0) {    // boost
      normls = 1 / (1 + sqrt(2) * Kls + Kls * Kls);
      a0ls = (1 + sqrt(2*Vls) * Kls + Vls * Kls * Kls) * normls;
      a1ls = 2 * (Vls * Kls * Kls - 1) * normls;
      a2ls = (1 - sqrt(2*Vls) * Kls + Vls * Kls * Kls) * normls;
      b1ls = 2 * (Kls * Kls - 1) * normls;
      b2ls = (1 - sqrt(2) * Kls + Kls * Kls) * normls;
  }
  else {    // cut
      normls = 1 / (1 + sqrt(2*Vls) * Kls + Vls * Kls * Kls);
      a0ls = (1 + sqrt(2) * Kls + Kls * Kls) * normls;
      a1ls = 2 * (Kls * Kls - 1) * normls;
      a2ls = (1 - sqrt(2) * Kls + Kls * Kls) * normls;
      b1ls = 2 * (Vls * Kls * Kls - 1) * normls;
      b2ls = (1 - sqrt(2*Vls) * Kls + Vls * Kls * Kls) * normls;
  }
}  

inline void LowShelf_Process()
{
    float l_inls = (float)(SYNTHTOTALSUM_L);
    float l_outls = l_inls * a0ls + l_z1ls;
    l_z1ls = l_inls * a1ls + l_z2ls - b1ls * l_outls;
    l_z2ls = l_inls * a2ls - b2ls * l_outls;
    SYNTHTOTALSUM_L = (int16_t)l_outls;

    float r_inls = (float)(SYNTHTOTALSUM_R);
    float r_outls = r_inls * a0ls + r_z1ls;
    r_z1ls = r_inls * a1ls + r_z2ls - b1ls * r_outls;
    r_z2ls = r_inls * a2ls - b2ls * r_outls;
    SYNTHTOTALSUM_R = (int16_t)r_outls;
}

//* HighShelf **********************************************************************
void Set_HighShelf_Parameter()
{
      float normhs;
      float Vhs = pow(10, fabs(peakGainhs) / 20.0);
      float Khs = tan(M_PI * Fchs);   
      if (peakGainhs >= 0) {    // boost
          normhs = 1 / (1 + sqrt(2) * Khs + Khs * Khs);
          a0hs = (Vhs + sqrt(2*Vhs) * Khs + Khs * Khs) * normhs;
          a1hs = 2 * (Khs * Khs - Vhs) * normhs;
          a2hs = (Vhs - sqrt(2*Vhs) * Khs + Khs * Khs) * normhs;
          b1hs = 2 * (Khs * Khs - 1) * normhs;
          b2hs = (1 - sqrt(2) * Khs + Khs * Khs) * normhs;
      }
      else {    // cut
          normhs = 1 / (Vhs + sqrt(2*Vhs) * Khs + Khs * Khs);
          a0hs = (1 + sqrt(2) * Khs + Khs * Khs) * normhs;
          a1hs = 2 * (Khs * Khs - 1) * normhs;
          a2hs = (1 - sqrt(2) * Khs + Khs * Khs) * normhs;
          b1hs = 2 * (Khs * Khs - Vhs) * normhs;
          b2hs = (Vhs - sqrt(2*Vhs) * Khs + Khs * Khs) * normhs;
      }
}

inline void HighShelf_Process()
{
    float l_inhs = (float)(SYNTHTOTALSUM_L);
    float l_ouths = l_inhs * a0hs + l_z1hs;
    l_z1hs = l_inhs * a1hs + l_z2hs - b1hs * l_ouths;
    l_z2hs = l_inhs * a2hs - b2hs * l_ouths;
    SYNTHTOTALSUM_L = (int16_t)l_ouths;

    float r_inhs = (float)(SYNTHTOTALSUM_R);
    float r_ouths = r_inhs * a0hs + r_z1hs;
    r_z1hs = r_inhs * a1hs + r_z2hs - b1hs * r_ouths;
    r_z2hs = r_inhs * a2hs - b2hs * r_ouths;
    SYNTHTOTALSUM_R = (int16_t)r_ouths;    
} 

     
