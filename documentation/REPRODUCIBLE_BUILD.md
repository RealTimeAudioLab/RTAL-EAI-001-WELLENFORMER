# Reproducible Build Guide

## 1. Install Arduino IDE

Install **Arduino IDE 1.8.13**.

## 2. Install ESP32 Arduino Core

Install **ESP32 Arduino Core 1.0.4** through the Arduino Boards Manager.

## 3. Select Board

```text
Tools -> Board -> ESP32 Arduino -> ESP32 Dev Module
```

## 4. Select Partition Scheme

```text
Tools -> Partition Scheme -> Huge APP
```

## 5. Disable PSRAM

```text
Tools -> PSRAM -> Disabled
```

## 6. Open Sketch

Open:

```text
firmware/Wellenformer/Wellenformer.ino
```

Arduino IDE should show the additional `.ino` files as tabs.

## 7. Build

Use **Verify/Compile** first before uploading.

## 8. Upload

Upload to the ESP32 Dev Module. If upload errors occur, reduce the upload speed.

## Notes

This guide intentionally documents the original environment. It is not a guarantee that the project will compile without changes under newer ESP32 cores.
