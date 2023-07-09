# Analogue-Analog-Clock-Display-using-a-GC9A01-display-attached-to-an-ESP32-with-sound
This project builds on an earlier project 'Analogue (Analog) Clock Display using a GC9A01 display attached to an ESP32' but adds the sounds of chimes and strikes as per a large clock such as Big Ben.
An ESP32 using its WiFi capabilities acquires NTP time, transmits this time to an Arduino with a DFPlayer to make sounds, and then displays an analogue clock on the GA9A01 display unit.
Rather than repeat the details of the earlier project, please refer to that project for basic information about displaying an analogue clock face. The information is the additional instructions for adding sound.

## What you will need (in addition to that specified in earlier project):
- Arduino Uno (or similar)
- DFPlayer mini MP3 module
- resistors: 3.3K, 2.2K 1K
- jumper wires
- unpowered speaker (8 ohm 1 watt cube speaker, as found in model railroading works well)

## STEP 1: Set up Arduino with DFPlayer
1. There many tutorials that will fully explain how to connect a DFPlayer to an Arduino.
1. Although any number of different pins can be used to attach the DFPlayer to the Arduino, I prefer to avoid the normal digital pins up to D13 so they can be used for other purposes. See below for the pins that I have used.

## STEP 2: Connect the Arduino and ESP32 for serial communication
1. Again, there are any tutorials on this topic.

## STEP 3: Suggested pinouts
1. Below is a suggested pinout table but other pins can be used.


| TFT_eSPI  | GC9A01    | ESP32 | Arduino | DFPlayer  | Speaker |
| --------  | ------    | ----- | ------- | --------  | ------- |
|           |  VCC      | 3V3   |         |           |         |
|           |  GND      | GND   |         |           |         |
|   MOSI    | DIN/SDA   | GP23  |         |           |         |
|   SCLK    |  CLK      | GP18  |         |           |         |
|   CS      |  CS       | GP15  |         |           |         |
|   DC      |  DC       | GP2   |         |           |         |
|   RST     |  RST      | GP4   |         |           |         |
|           |           | GND   | GND     |           |         |
|           |           | GP16  | D5      |           |         | Note: 3.3k and 2.2k resistors in step-down shifter
|           |           | GP17  | D4      |           |         |
|           |           |       | 5V      | VCC       |         |
|           |           |       | GND     | GND       |         |
|           |           |       | D15/A1  | BUSY      |         |
|           |           |       | D16/A2  | RX        |         | Note: 1K resistor between D16/A2 and Rx
|           |           |       | D17/A3  | TX        |         |
|           |           |       |         | SPK1      | Input 1 | 8 ohm 1 watt cube speaker works well
|           |           |       |         | SPK2      | Input 2 |

