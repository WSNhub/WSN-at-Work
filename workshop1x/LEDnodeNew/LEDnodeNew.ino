/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 * 
 * DESCRIPTION
 * Example sketch showing how to control physical relays. 
 * This example will remember relay state after power failure.
 * http://www.mysensors.org/build/relay
 */ 

//send: 22-22-0-0 s=0,c=1,t=35,pt=4,l=4,sg=0,st=ok:54
//read: 0-0-22 s=1,c=1,t=2,pt=2,l=2,sg=0:0
//Incoming change for sensor:1, New status: 0
//send: 22-22-0-0 s=0,c=1,t=35,pt=4,l=4,sg=0,st=fail:55
//send: 22-22-0-0 s=0,c=1,t=35,pt=4,l=4,sg=0,st=ok:56
//read: 0-0-22 s=1,c=1,t=2,pt=2,l=2,sg=0:1
//Incoming change for sensor:1, New status: 1
//send: 22-22-0-0 s=0,c=1,t=35,pt=4,l=4,sg=0,st=ok:57

#include <MySigningNone.h>
#include <MyTransportNRF24.h>
#include <MyTransportRFM69.h>
#include <MyHwATMega328.h>
#include <MySensor.h>
#include <SPI.h>

#define LED_PIN  2  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

// NRFRF24L01 radio driver (set low transmit power by default) 
MyTransportNRF24 radio(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);  
//MyTransportRFM69 radio;
// Message signing driver (none default)
//MySigningNone signer;
// Select AtMega328 hardware profile
MyHwATMega328 hw;
// Construct MySensors library
MySensor gw(radio, hw);
const int NodeId = 22;  // CHANGE !!
MyMessage FlowMsg(0 /* presentation */ ,V_VOLUME);
long teller = 0;

void setup()  
{   
  // Initialize library and add callback for incoming messages
  gw.begin(incomingMessage, NodeId, false);
  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("LED-node", "1.0");

   // Register all sensors to gw (they will be created as child devices)
   gw.present(0, S_WATER);       
   gw.present(1, S_LIGHT);
   // Then set relay pins in output mode
   pinMode(LED_PIN, OUTPUT);   
   // Set relay to last known state (using eeprom storage) 
   digitalWrite(LED_PIN, gw.loadState(1 /*sensor*/)?RELAY_ON:RELAY_OFF);
}

void loop() 
{
  teller++; 
  gw.send(FlowMsg.set(teller)); 
  // Alway process incoming messages whenever possible
  gw.wait(1000);
}

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) {
     // Change relay state
     digitalWrite(LED_PIN, message.getBool()?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     gw.saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}

