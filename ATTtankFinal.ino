
/*****************************************************************************
 Version 0.11.14.22

 Code is written for the Adafruit Feather RF52 Bluefruit LE.
 
 Pin assignments:
   11 - Clock out for 1ppb
   10 - Clock out for 24ppb
   9 - Clock out for 48ppb
   7 - CS for SPI connection to MAX72xx
  A1 - Connected to 10k pot for internal clock rate

 Written by Scott Marler. GNU General Public License v3.0, check license.txt
 for more information.
 

 **************************************************************************/
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define TPIN1 2
#define TPIN2 3
#define GPIN1 11
#define GPIN2 12
#define LED1 A0
#define LED2 A1
#define LED3 A2
#define LED4 A3
#define LED5 A5
#define LED6 A4

int leds[10] = {LED1,LED2,LED3,LED4,LED5,LED6};

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

int pos;
int center;
int Ustate = 0;
int Dstate = 0;
unsigned long Interval = 10;
unsigned long lastUpdate;

void setup(void) {
  delay(2000);
  Serial.begin(9600);
  Serial.println(F("AAT Trade Federation Battle Tank"));
  Serial.println(F("---------------------------------------"));
  randomSeed(analogRead(0));
  Serial.println(F("Initialising Motors"));
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
  
  /* Initialise the module */
  Serial.print(F("Initialising Bluetooth "));

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
   // Change device name
  Serial.println(F("Change Name to ATT"));
  if(!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=ATT"))) {
    Serial.println(F("Failed to change name"));}
  else {Serial.println(F("Name Changed"));}

}

void loop(void) {
  
  // Check for incoming commands
    ble.println("AT+BLEUARTRX");
    ble.readline();
    if (strcmp(ble.buffer, "OK") == 0) {
      return;
    } else {
  // Some data was found, its in the buffer
      Serial.print(F("[Recv] ")); Serial.println(ble.buffer);
      if((strcmp(ble.buffer, "U") == 0) && (Ustate == 0)) {
        Serial.println(ble.buffer);
        Ustate = 1;
        digitalWrite(GPIN1,HIGH);
      } else if((strcmp(ble.buffer, "D") == 0) && (Ustate == 0)) {
        Ustate = 1;
        Serial.println("D");
        digitalWrite(GPIN2,HIGH);
      } else if((strcmp(ble.buffer, "R") == 0) && (Ustate == 0)) {
        Ustate = 1;
        digitalWrite(TPIN1,HIGH);
      } else if((strcmp(ble.buffer, "L") == 0) && (Ustate == 0)) {
        Ustate = 1;
        digitalWrite(TPIN2,HIGH);
      } else if(strcmp(ble.buffer, "S") == 0) {
        Ustate = 0;
        digitalWrite(GPIN1,LOW);
        digitalWrite(GPIN2,LOW);
        digitalWrite(TPIN1,LOW);
        digitalWrite(TPIN2,LOW);
      }
      else if(strcmp(ble.buffer, "F") == 0) {
        fire();
      }
    }
    ble.waitForOK();
}

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
