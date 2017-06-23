#include <Homie.h>

#define PIN_GREEN 4
#define PIN_BUTTON 0

int relayState = HIGH; 
bool stateChange = false; 

int buttonState;                     // the current reading from the input pin
int lastButtonState = LOW;           // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

HomieNode switchNode("switch", "switch");

bool switchHandler(HomieRange range, String value) {
  if (value != "true" && value != "false") return false;
  
  if (value == "true") {
    analogWrite(PIN_GREEN, 256);
    switchNode.setProperty("on").send("true");
    Serial.println("GREEN is on");
  } else if (value == "false") {
    analogWrite(PIN_GREEN, 0);
    switchNode.setProperty("on").send("false");
    Serial.println("GREEN is off");
  } else {
    return false;
  }
  return true;
}



void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode(PIN_GREEN, OUTPUT);
 
  Homie_setFirmware("homie-rgb-pwm", "0.0.1");
  switchNode.advertise("on").settable(switchHandler);
  Homie.setup();
}

void loop() {
  int reading = digitalRead(PIN_BUTTON);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
       if (buttonState == HIGH) {
        stateChange = true;
        relayState = !relayState;
      }
    }
  }
  lastButtonState = reading;
  if (stateChange) { 
    analogWrite(PIN_GREEN, (relayState == HIGH)? 256 : 0);
    switchNode.setProperty("on").send( (relayState == HIGH)? "true" : "false" );
    stateChange = false;
  }  
  Homie.loop();
