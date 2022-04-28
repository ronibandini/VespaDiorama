// Vespa Clock Diorama
// Roni Bandini @RoniBandini 
// Argentina, April 2022
// Amp GPIO 25, GND
// Get Firebeetle from https://www.dfrobot.com/product-2195.html?tracking=60f51c53af980

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include "engine.h"
#include "XT_DAC_Audio.h"

Servo servo1;

int servoPin = 2;
int laserPin=14;
int lightOn=0;
int pinAudio=25;

int demoMode=1;

XT_Wav_Class Sound(engine);                                         
XT_DAC_Audio_Class DacAudio(pinAudio,0);    
uint32_t DemoCounter=0;    

// WiFi
const char* ssid     = "SSIHERE";
const char* password = "PASSWORDHERE";

// NTP data
const long utcOffsetInSeconds = -10800; // -3x60x60
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  
  servo1.attach(servoPin);
  pinMode(laserPin, OUTPUT);
  
  delay(1000);
  Serial.begin(115200); 
  Serial.println("Vespa Diorama started...");         

  Serial.print(" Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

  // reset wheel position
  servo1.write(180);
  timeClient.begin(); 
  delay(2000);
  
}

void wheel1(){

 Serial.println("Wheel forward");  
 for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees);
        delay(10);
    }  
}

void wheel2(){
   Serial.println("Wheel back"); 
   for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        servo1.write(posDegrees);
        delay(50);
    }
}

void engineWheel(){

  Serial.println("Engine and wheel");
  
  int posDegrees=180;
  
  // forward  
  while (posDegrees>=0)
  {               
    DacAudio.FillBuffer(); 
      
    if(Sound.Playing==false)       
    DacAudio.Play(&Sound);       

    servo1.write(posDegrees);
    posDegrees--;
    delay(20);   

    // wait until wav ends
    if (posDegrees==1){

      int myCounter=1;
      while (myCounter<19){

        DacAudio.FillBuffer(); 
      
        if(Sound.Playing==false)       
        DacAudio.Play(&Sound);

        delay(20);
        myCounter++;
      }
      
    }
  
  }
 
}
  
void loop() {

  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  
  if (timeClient.getMinutes()==0 or demoMode==1){
      
      Serial.print("Hour changed, engine");
      engineWheel();    

    // put wheel back
    delay(4000);
    wheel2();
    delay(4000);

  } // 0 minutes
  

         
}