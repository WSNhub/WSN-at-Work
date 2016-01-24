/*
 LED Node 
*/

#include <DigitalIO.h>
#include <DigitalPin.h>
#include <MySensor.h>  
#include <SPI.h>

#define LED_PIN    2  // LED pin
#define ON   1  // GPIO value to write to turn on attached relay
#define OFF  0  // GPIO value to write to turn off attached relay

const int NodeId = 30;  // CHANGE !!
MySensor sensor;
MyMessage FlowMsg(0 /* presentation */ ,V_VOLUME);
double teller = 0;
unsigned long lastUpdate=0;
  
void setup() {
  Serial.begin(115200);
  Serial.println("start serial");
  
  // initialize the LED pin as an output:
  pinMode(LED_PIN, OUTPUT);      
  digitalWrite(LED_PIN, LOW);
  
  // Startup and initialize MySensors library. 
  sensor.begin(incomingMessage, NodeId, false);  
  digitalWrite(LED_PIN, HIGH);   
  sensor.sendSketchInfo("test Node", "1.0"); 
  sensor.present(0, S_WATER);   
  sensor.present(1, S_LIGHT);  
  
  // Fetch status
  // Set relay to last known state (using eeprom storage) 
  digitalWrite(LED_PIN, sensor.loadState(1)?ON:OFF);
  Serial.println("init done");
}

void loop(){
  
  unsigned long now = millis(); 
  // update counter every ...
  if (now-lastUpdate > 1000) {
     teller++; 
     sensor.send(FlowMsg.setSensor(0).set(teller,0)); 
     Serial.println(teller);
     lastUpdate = now;
  }
  // Alway process incoming messages whenever possible
  sensor.process();
}

void incomingMessage(const MyMessage &message) {
  // from controller.
  if (message.type==V_LIGHT) {
     // Change relay state
     digitalWrite(LED_PIN, message.getBool()?ON:OFF);
     // Store state in eeprom
     sensor.saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}

