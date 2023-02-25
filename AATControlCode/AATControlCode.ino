/*****************************************************************************
 Version 0.2.24.23

 Code is written for the Adafruit Feather 32u4 Bluefruit LE and includes Adafruit Bluetooth libraries.
 
 This code is for a modified Star Wars Legion AAT to motorize the turret, side guns and LEDs in the
 missle slots. Control the AAT using AdaFruit Bluefruit LE Connect from an iOS or Android devices. 
 Once connected to the ATT, select Controller -> Control Pad. The up and down arrows control the side
 guns, the left and right arrows control the turret and the 1 button controls the LEDs.

 Written by Scott Marler. GNU General Public License v3.0, check license.txt
 for more information.
 **************************************************************************/

//Libraries
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
//Configuration file for Bluetooth 
#include "BluefruitConfig.h"
//Constants used in the code
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define TPIN1 11
#define TPIN2 12
#define GPIN1 6
#define GPIN2 5
#define LED1 A0
#define LED2 A1
#define LED3 A2
#define LED4 A3
#define LED5 A4
#define LED6 A5
//Array used for LEDs
int leds[10] = {LED1,LED2,LED3,LED4,LED5,LED6};
//Setup Bluetooth
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

extern uint8_t packetbuffer[];

//Setup
void setup(void) {
  delay(500);
  Serial.begin(115200);
  randomSeed(analogRead(0));
//Setup GPIO pins for thier use
  pinMode(GPIN1,OUTPUT);
  pinMode(GPIN2,OUTPUT);
  pinMode(TPIN1,OUTPUT);
  pinMode(TPIN2,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);
  delay(100);
  
  /* Initialise the Bluetooth module */
  Serial.print(F("Initializing Bluetooth "));

  if ( !ble.begin(VERBOSE_MODE) ){
    error(F("Couldn't find Bluetooth interface."));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE ) {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

   // Change device name advertized for Bluetooth
  Serial.println(F("Change Name to AAT"));
  if(!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=AAT"))) {
    Serial.println(F("Failed to change name"));}
  else {Serial.println(F("Name Changed"));}

  Serial.println("Requesting Bluetooth info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Ready for controller connection"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) ) {
    // Change Mode LED Activity
    Serial.println(F("******************************"));
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    Serial.println(F("******************************"));
  }
  Serial.println( F("Switching to DATA mode."));
  ble.setMode(BLUEFRUIT_MODE_DATA);
}

//Main loop
void loop(void) {
  // Wait for packet

  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  // When button pushed perform operation requested
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';

    if(pressed) {
      switch(buttnum) {
        case 1:
          fire();
          break;
        case 5:
          digitalWrite(GPIN1,HIGH);
          break;
        case 6:
          digitalWrite(GPIN2,HIGH);
          break;
        case 7:
          digitalWrite(TPIN2, HIGH);
          break;
        case 8:
          digitalWrite(TPIN1,HIGH);
          break;

        default:
          stopAllMotors();
          break;
      }
    } else {
      stopAllMotors();
    }

    Serial.print ("Button "); Serial.print(buttnum);
    if (pressed) {
      Serial.println(" pressed");
    } else {
      Serial.println(" released");
    }
  }
}

//Function to stop all motors
void stopAllMotors() {
  digitalWrite(GPIN1,LOW);
  digitalWrite(GPIN2,LOW);
  digitalWrite(TPIN1,LOW);
  digitalWrite(TPIN2,LOW);
}

//Function to simulate missle fire
int fire() {
  boolean c = true;
  int n = 0;
  int l[10];
  for(int i=0;i<10;i++) {
    l[i]=0;
  }
  while(n < 9) {
    int t = random(0,10);
    for(int x=0;x<10;x++) {
      if(t == l[x]) {c = false;break;}
    }
    if(c) {l[n] = t;n++;}
    c = true;
  }
  for(int i=0;i<10;i++) {
    digitalWrite(leds[l[i]],HIGH);
    delay(100);
    digitalWrite(leds[l[i]],LOW);
  }
}