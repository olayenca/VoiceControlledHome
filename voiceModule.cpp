#include <TFT.h>  // Arduino LCD library
#include <SPI.h>

#include <SoftwareSerial.h> //Voice Recognition Library
#include "VoiceRecognitionV3.h"

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

#define onRecord    (0)
#define offRecord   (1)
#define nameRecord   (2)

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

//instance for VR
VR myVR(2,3);

uint8_t records[3]; // saved records
uint8_t buf[64];

//led integer
int led = 8;

//serial print signature
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup() {
  //TFT SETUP
  TFTscreen.begin();
  TFTscreen.background(244, 200, 66);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(1);

  //VR SETUP

  myVR.begin(9600);
  Serial.begin(115200);
  TFTscreen.println("Voice Recognition Module \nis Ready...");

  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  delay(5000);
  //condition for no voice record found
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }

  if(myVR.load((uint8_t)onRecord) >= 0){
    Serial.println("onRecord loaded");
  }

  if(myVR.load((uint8_t)offRecord) >= 0){
    Serial.println("offRecord loaded");
  }

  if(myVR.load((uint8_t)nameRecord) >= 0){
    Serial.println("nameRecord loaded");
  }

}

void loop() {
  //TFT part
  //TFTscreen.background(226, 66, 244);
  TFTscreen.setTextSize(2);
  delay(5000);

  //VR Part
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case onRecord:
        /** turn on LED */
        digitalWrite(led, HIGH);
        TFTscreen.setTextSize(1);
        TFTscreen.println("Bulb is turned ON");
        //delay(4000);
      //  TFTscreen.stroke(244, 200, 66);
        break;
        case offRecord:
          /** turn off LED*/
          digitalWrite(led, LOW);
          TFTscreen.setTextSize(1);
          TFTscreen.println("Bulb is turned OFF");
        //  delay(4000);
        //  TFTscreen.stroke(244, 200, 66);
          break;

              case nameRecord:
                /** Prints Coco*/
                TFTscreen.setTextSize(2);
                TFTscreen.stroke(226, 66, 244);
                TFTscreen.println("COCO!!!");
                delay(4000);
                TFTscreen.println("Obviously...");
                break;
                default:
                  Serial.println("Record function undefined");
                  break;
                }
                /** voice recognized */
                printVR(buf);
              }

              //TFTscreen.background(226, 66, 244); // clear the screen before starting again
              //delay(10000);
            }
