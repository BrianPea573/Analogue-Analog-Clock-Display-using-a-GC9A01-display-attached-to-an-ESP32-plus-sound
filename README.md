# GC9A01 Analogue (Analog) Clock Display with sound
This project builds on an earlier project 'Analogue (Analog) Clock Display using a GC9A01 display attached to an ESP32' but adds the sounds of chimes and strikes as per a large clock such as Big Ben.
An ESP32 using its WiFi capabilities acquires NTP time, transmits this time to an Arduino with a DFPlayer to make sounds, and then the ESP32 displays an analogue clock on the GC9A01 display unit.
Since the time of the Ardunio is synchronised (at least initially) with the ESP, the timing of the chimes and strikes by the Arduino will very closely match the display attached to the ESP32. Of course, over a long period of time, the clocks of the Arduino and ESP may deviate given the limitations of accuracy of their internal clocks.
Rather than repeat the details of the earlier project, please refer to that project for basic information about displaying an analogue clock face. The information below is the additional instructions for adding sound.

## What you will need (in addition to that specified in earlier project):
- Arduino Uno (or similar)
- DFPlayer mini MP3 module
- resistors: 3.3K, 2.2K, 1K
- jumper wires
- unpowered speaker (8 ohm 1 watt cube speaker, as found in model railroading, works well)

## STEP 1: Set up Arduino with DFPlayer
1. There many tutorials that will fully explain how to connect a DFPlayer to an Arduino.
1. Although any number of different pins can be used to attach the DFPlayer to the Arduino, I prefer to avoid the normal digital pins up to D13 so they can be used for other purposes. See below for the pins that I have used.

## STEP 2: Connect the Arduino and ESP32 for serial communication
- Again, there are any number of tutorials on this topic.

## STEP 3: Download the various files and load them on your PC within your Arduino IDE file directory as follows:
1. In a folder called GC9A01A_Clock_ESP32_Sound_GitHub, load the following files:
    - BigBen_16bit.h
    - GC9A01A_Clock_ESP32_Sound_GitHub.ino
    - hourHand.h
    - minuteHand.h
    - NTP_Time.h
    - secondHand.h
1. In a folder called Arduino_Clock_Chimes, load the following files:
    - Arduino_Clock_Chimes.ino
1. In the folder Arduino_Clock_Chimes:
    - create a sub-folder called mp3
1. In the sub-folder called mp3, load the following files:
    - 0001.mp3
    - 0002.mp3
    - 0003.mp3
    - 0004.mp3
    - 0005.mp3
    - 0006.mp3

## STEP 4: Customise GC9A01A_Clock_ESP32_Sound_GitHub.ino
- Edit this file and enter your WiFi credentials (name and password)

## STEP 5: Suggested pinouts
- Below is how I connected the various components although other mappings would be valid, noting that some GPIO pins have restrictions:

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

## Known Limitation
Although the code has been designed to handle interruptions to power to either or both of the Arduino and ESP32 boards, there is a known problem with Arduino resets. The board is reset when the Arduino IDE opens the serial port, or by opening the serial monitor. Opening the serial port from the USB host PC or even by reconnecting 5V power often resets the board, but not always. If the serial ports do not open normally, the Reset button on the board will reset the board.