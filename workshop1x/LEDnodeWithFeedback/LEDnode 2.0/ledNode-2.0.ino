/*
 LED node 2.0, 2016-feb-16, by Jo Simons
*/
#include <DigitalIO.h>
#include <DigitalPin.h>
#include <MySensor.h>  
#include <SPI.h>

#define CNT_ID         0  // counter sensor id
#define LED_ID         1  // LED sensor id
#define LED_PIN        2  // LED pin
#define LED_OFF        0  // LED off state
#define LED_ON         1  // LED on state 
#define LED_BLINK      2  // LED blinking indicator, ored with state
#define NODE_ID       31  // CHANGE TO YOUR NODE-ID !!!

MySensor      gw;
MyMessage     eventMsg(CNT_ID, V_VOLUME);
MyMessage     statusMsg(LED_ID, V_LIGHT);

unsigned long lastUpdate = 0;
int           cntValue = 0;
int           ledState = LED_OFF;
  
// initialise the node
void setup() {
  Serial.begin(115200);
  Serial.println("start serial");
  
  // startup and initialize MySensors library. 
  gw.begin(handleIncMsg, NODE_ID, false);  
  gw.sendSketchInfo("ledNode", "2.0"); 
  gw.present(CNT_ID, S_WATER); // -> V_VOLUME  
  gw.present(LED_ID, S_LIGHT); // <> V_LIGHT
  
  // initialize the LED with reloaded state from EEPROM
  pinMode(LED_PIN, OUTPUT);      
  setLED(ledState = gw.loadState(LED_ID));

  Serial.println("init done");
  gw.process();
} // setup

// periodically processing
void loop(){
  unsigned long now = millis(); 
  
  if ((now - lastUpdate) > 1000) {
    // update counter every second
    gw.send(eventMsg.setSensor(CNT_ID).set(++cntValue)); 
    Serial.println(cntValue);
    lastUpdate = now;

    // handle blinking LED
    if (ledState & LED_BLINK) 
      setLED(ledState = ((ledState & LED_ON) ? LED_OFF : LED_ON) | LED_BLINK);
  }

  // Alway process messages whenever possible
  gw.process();
} // loop

// set LED and send feedback
void setLED(int nState) {
  // write to output
  digitalWrite(LED_PIN, nState & LED_ON);

  // send on/off feedback to gateway
  gw.send(statusMsg.setSensor(LED_ID).set(nState & LED_ON));

  // save full state to EEPROM and report
  gw.saveState(LED_ID, nState);
  Serial.print("LED state=");
  Serial.println(nState);
} // setLED

// handle command from controller
void handleIncMsg(const MyMessage &incMsg) {
  if ((incMsg.type == V_LIGHT) && (incMsg.sensor == LED_ID)) {
    Serial.print("LED command=");
    Serial.print(incMsg.getInt());

    switch (incMsg.getInt()) {
      case 2:  ledState = LED_ON; 
      break;
      case 3:  ledState = LED_OFF; 
      break;
      case 6:  ledState =  (ledState & LED_ON) ? LED_OFF : LED_ON; 
      break;
      case 9:  ledState = ((ledState & LED_ON) ? LED_OFF : LED_ON) | LED_BLINK; 
      break;
      default: Serial.print("... unknown command");
      break;
    } // switch
    
    Serial.println("");
    setLED(ledState);
  } 
} // handleIncMsg


