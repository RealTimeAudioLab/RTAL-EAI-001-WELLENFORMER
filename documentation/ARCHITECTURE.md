# Architecture Overview

WELLENFORMER is organized as an Arduino multi-tab sketch. The main file `Wellenformer.ino` contains the central configuration and global firmware structure. Additional `.ino` files provide functional modules.

## Firmware Modules

| File | Role |
|---|---|
| `Wellenformer.ino` | Main sketch and global configuration |
| `CoreTask.ino` | Core task / UI and background handling |
| `Arp.ino` | Arpeggiator functions |
| `Biquad.ino` | Filter and DSP helper functions |
| `Chorus.ino` | Stereo chorus effect |
| `Delay.ino` | Stereo delay effect |
| `Fader.ino` | Display fader and meter drawing |
| `LoadParameter.ino` | Parameter loading |
| `Scope.ino` | Scope and waveform visualization |
| `SdCard.ino` | SD-card file handling |
| `WideStereo.ino` | Stereo width processing |
| `samplenames.h` | Program/sample name table in flash memory |

## Design Notes

The project reflects a classic Arduino/ESP32 development style. The code is intentionally preserved in this form for historical authenticity.
