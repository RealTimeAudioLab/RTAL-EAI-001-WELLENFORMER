# Original Development Environment

This document records the original development setup used for the WELLENFORMER firmware.

The purpose is reproducibility. The project is preserved as a historical Arduino/ESP32 implementation and should first be built with the original toolchain before attempting ports to newer environments.

## Arduino IDE

| Setting | Value |
|---|---|
| Arduino IDE | 1.8.13 |

## ESP32 Board Package

| Setting | Value |
|---|---|
| ESP32 Arduino Core | 1.0.4 |

## Board Settings

| Arduino Menu Item | Value |
|---|---|
| Board | ESP32 Dev Module |
| Partition Scheme | Huge APP |
| PSRAM | Disabled / No PSRAM |

## Recommended Historical Settings

The following settings are typical for the documented environment and should be verified against the original local Arduino setup when available:

| Arduino Menu Item | Suggested Value |
|---|---|
| CPU Frequency | 240 MHz |
| Flash Frequency | 80 MHz |
| Flash Mode | QIO |
| Upload Speed | 921600 baud or lower if upload errors occur |
| Core Debug Level | None |

## Important Compatibility Note

This firmware was not prepared as a modern ESP32 Core 2.x/3.x project. The reference environment is:

```text
Arduino IDE 1.8.13
ESP32 Arduino Core 1.0.4
ESP32 Dev Module
Huge APP
No PSRAM
```

Newer ESP32 board packages may require code changes, library changes or different build settings.
