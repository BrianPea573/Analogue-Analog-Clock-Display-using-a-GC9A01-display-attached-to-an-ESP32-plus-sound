/********************
Arduino Time Sync from NTP Server using ESP WiFi module.

This is the code for the Arduino to receive the time via serial communication and then sound the chimes and strikes on the quarter hour. The design is that the Arduino requests the latest time from the ESP32, which with its WiFi capabilities has secured NTP time via the internet. The time packet sent from the ESP2 should start with the characters "AET" which the Arduino checks to ensure it is a good transmission. More elaborate checksums etc were not considered necessary. If the packet is not valid, then the Arduino will request another transmission from the ESP32. To cover the scenario where the Ardunio request is simply 'lost', a periodic check e.g. every 5 seconds is done on the Arduino side and if a valid time packet has still not been received, the Arduino will again request another transmission. This will continue until a valid time transmission is received, where upon the Arduino will update its internal clock. It can then proceed to manage the chimes and strikes using its DFPlayer.

The Arduino will continuously check to see the ESP32 sends another time packet. This covers the scenario where the ESP32 has had its power cut and has restarted. Whenever the ESP32 restarts, it will attempt to send a time packet. Although this is not entirelty necessary, it makes for a quicker start-up when both microprocessors are started together.

 *******************
  
 Pinouts used for this sketch (other pin arrangements would also work):

| TFT_eSPI  | GC9A01    | ESP32 | Arduino | DFPlayer  | Speaker |
| --------  | ------    | ----- | ------- | --------  | ------- |
|           |  VCC      | 3V3   |         |           |         |
|           |  GND      | GND   |         |           |         |
|   MOSI    | DIN/SDA   | GP23  |         |           |         |
|   SCLK    |  CLK      | GP18  |         |           |         |
|   CS      |  CS       | GP15  |         |           |         |
|   DC      |  DC       | GP2   |         |           |         |
|   RST     |  RST      | GP4   |         |           |         |
|           |           | GP16  | D5      |           |         | Note: 3.3k and 2.2k resistors in step-down shifter
|           |           | GP17  | D4      |           |         |
|           |           |       | 5V      | VCC       |         |
|           |           |       | GND     | GND       |         |
|           |           |       | D15/A1  | BUSY      |         |
|           |           |       | D16/A2  | RX        |         | Note: 1K resistor between D16/A2 and Rx
|           |           |       | D17/A3  | TX        |         |
|           |           |       |         | SPK1      | Input 1 | 8 ohm 1 watt cube speaker works well
|           |           |       |         | SPK2      | Input 2 |

*/
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include "DFRobotDFPlayerMini.h"
#include "Arduino.h"

int read_ESP_delay  = 5000;           // delay before requesting NTP time again
unsigned long next_ESP_time;         // time of next NTP time request

long unsigned moduloTime;
byte h;
time_t t;
byte busy_pin = 15;                   //DFPLayer Status LOW means playing/HIGH means not playing
byte chimes_num = 0;
byte strike_num = 0;
byte NTP_time_status = 0;             // 0=NTP time not set; 1=NTP time set
int baudRate = 9600;                  // setup a common baudrate for UNO and ESP

//variables for internet time routine
char AEDTString[11];
long AEDTTime;
char buffer[20];
int i = 0;

// setup the ESP emulated serial channel
SoftwareSerial esp(4, 5);   //RX, TX

// Initialise DFPlayer (MP3 player)
SoftwareSerial mySerial(17, 16);      // UNO RX, UNO TX for DFPlayer
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  initialiseDFPlayer();               //initialise DFPlayer
  // Start each software serial port
  Serial.begin(baudRate);
  esp.begin(baudRate);
  buffer[0] = '0';                    // initialise 1st position to ensure latest time package is used
  t = now();
  Serial.println("Arduino Receiver Starting");
  print_current_time();
  esp.print("0");                     // indicate to sender that receiver is ready for time transmission
  esp.listen();
  next_ESP_time = (millis() + read_ESP_delay); // set time of next NTP request if initial request "lost"
}

void loop() {
  while (NTP_time_status == 0) read_ESP_data();   // read NTP time from ESP
  manage_sound();                                 // manage chimes
}
      
void read_ESP_data() {
  
  if (esp.available() >= 13) {                    // read data only when all data received
    i = 0;
    // read the incoming data
    while (esp.available() > 0) {
      buffer[i] = esp.read();                     // read all data in the buffer
      i = i+1;
    }

  // if data is available, parse it and check it is a AET token from the ESP
  if ((buffer[0] == 'A') && (buffer[1] == 'E') && (buffer[2] == 'T')) {
    // tell ESP that good data packet was received
    esp.print("1");
    // if data sent is the AET token, take it
    for (i=0; i<10; i++) {
      AEDTString[i] = buffer[i+3];
    }
    AEDTString[10] = '\0';
    // print the AET time on the monitor
    Serial.println("TIME FROM ESP: ");
    for (i=0; i<10; i++) {
      Serial.print(AEDTString[i]);
    }
    AEDTTime = atol(AEDTString);          // convert string to long integer
    setTime(AEDTTime + 2);                // set internal clock a little ahead to allow time to run MP3 player
    print_current_time();                 // print local time
    NTP_time_status = 1;                  // indicate NTP time has been set
    chimes_num = 1;    
    strike_num = 0;
    manageDFPlayer();                     // play short chimes to indicate all is well at startup
  }
  else {
    esp.print("0"); 
    Serial.println("Data no good");
    delay(100);
    }
  }
  else {
    if (millis() >= next_ESP_time) {                // check if time to request another time transmission
      next_ESP_time = millis() + read_ESP_delay;    // set time for next check
      esp.print("0");                               // indicate to sender that receiver requires another time transmission
    }
  }
} 
    
void manage_sound() {
      t = now();
      h = hour(t);
      moduloTime = (t % (60*60));
      if (moduloTime == 0) hourChimes();
      if (moduloTime == 15*60) quarterhourChimes();
      if (moduloTime == 30*60) halfhourChimes();
      if (moduloTime == 45*60) threequarterhourChimes();
//      }
      manageDFPlayer();
}
  
void hourChimes() {
  Serial.println("Hourly Chimes");
  chimes_num = 4;
  if (h >12) h = h -12;
  if (h == 0) h = 12;
  strike_num = h;
  delay(1100);
}
 
void quarterhourChimes() {
  Serial.println("Quarterly Chimes");
  chimes_num = 1;    
  strike_num = 0;
  delay(1100);
}

void halfhourChimes() {
  Serial.println("Half Hourly Chimes");
  chimes_num = 2;
  strike_num = 0;
  delay(1100);
}

void threequarterhourChimes() {
  Serial.println("Three Quarter Hourly Chimes");
  chimes_num = 3;
  strike_num = 0;
  delay(1100);
}  
  
void print_current_time() {
  t = now();
  Serial.print(day(t));
  Serial.print ("-");
  Serial.print(month(t));
  Serial.print("-");
  Serial.print(year(t));
  Serial.print(" ");
  if(hour(t) < 10) Serial.print(" ");
  Serial.print(hour(t));
  Serial.print(":");
  if(minute(t) < 10) Serial.print("0");
  Serial.print(minute(t));
  Serial.print(":");
  if(second(t) < 10) Serial.print("0");
  Serial.println(second(t));
}
      
void initialiseDFPlayer() {                     //initialise DFPlayer
  mySerial.begin (9600);
  myDFPlayer.begin(mySerial);                   //set softwareSerial for DFPlayer-mini mp3 module
  myDFPlayer.reset();                           //Reset the module
  myDFPlayer.volume(30);                        //Set volume value. From 0 to 30 
  pinMode (busy_pin, INPUT_PULLUP);             //initialise DFPLayer busy pin
}
      
void manageDFPlayer () {
    if ((digitalRead(busy_pin)) == HIGH) {      //check if the DFPlayer has finished playing
      if (chimes_num != 0) {
        myDFPlayer.playMp3Folder(chimes_num);
        chimes_num = 0;                         // indicate chimes playing is complete
      }
      else if (strike_num > 0) {
        if (strike_num == 1) myDFPlayer.playMp3Folder(6);
        else myDFPlayer.playMp3Folder(5);
        strike_num = strike_num -1;             // decrement number of strikes still to play
      }
   }
    delay(100);                                 //delay a little bit before checking again
}
