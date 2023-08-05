// Enable debug prints
#define MY_DEBUG
// Enable and select radio type attached 
#define MY_RADIO_RF24
//#define MY_RADIO_RFM69
//#define MY_RS485
#define MY_NODE_ID 7      // Changer per your Mysensors Node number
#define CHILD_ID_LIGHT_1 0
#define CHILD_ID_LIGHT_2 1
#define CHILD_ID_LIGHT_3 2
#define CHILD_ID_LIGHT_4 3
#define CHILD_ID_LIGHT_5 4
#define CHILD_ID_LIGHT_6 5
#define SN "Relay"
#define SV "1.0"
#define RELAY_PIN_1 3 // Relay 1 connected to D3
#define RELAY_PIN_2 4 // Relay 2 connected to D4

#include <SPI.h>
#include <MySensors.h>  

#define LIGHT_SENSOR_1_ANALOG_PIN A0
#define LIGHT_SENSOR_2_ANALOG_PIN A1
#define LIGHT_SENSOR_3_ANALOG_PIN A2
#define LIGHT_SENSOR_4_ANALOG_PIN A3
#define LIGHT_SENSOR_5_ANALOG_PIN A4
#define LIGHT_SENSOR_6_ANALOG_PIN A5

MySensor gw;
MyMessage msgRelay1(1, V_STATUS); // Child ID 1 for Relay 1
MyMessage msgRelay2(2, V_STATUS); // Child ID 2 for Relay 2
MyMessage msgLight1(CHILD_ID_LIGHT_1, V_LIGHT_LEVEL);
MyMessage msgLight2(CHILD_ID_LIGHT_2, V_LIGHT_LEVEL);
MyMessage msgLight3(CHILD_ID_LIGHT_3, V_LIGHT_LEVEL);
MyMessage msgLight4(CHILD_ID_LIGHT_4, V_LIGHT_LEVEL);
MyMessage msgLight5(CHILD_ID_LIGHT_5, V_LIGHT_LEVEL);
MyMessage msgLight6(CHILD_ID_LIGHT_6, V_LIGHT_LEVEL);

void presentation() {
  // Send the presentation to the controller
  sendSketchInfo("Dehumidifier Controller", "1.0");
  
  // Present the two relays as binary switches
  present(1, S_BINARY); // Child ID 1 for Relay 1
  present(2, S_BINARY); // Child ID 2 for Relay 2
  
  // Present the six light sensors as light level sensors
  present(CHILD_ID_LIGHT_1, S_LIGHT_LEVEL);
  present(CHILD_ID_LIGHT_2, S_LIGHT_LEVEL);
  present(CHILD_ID_LIGHT_3, S_LIGHT_LEVEL);
  present(CHILD_ID_LIGHT_4, S_LIGHT_LEVEL);
  present(CHILD_ID_LIGHT_5, S_LIGHT_LEVEL);
  present(CHILD_ID_LIGHT_6, S_LIGHT_LEVEL);
}

void setup()
{
  gw.begin(incomingMessage);
  gw.sendSketchInfo(SN, SV);
  
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  
  gw.present(1, S_BINARY); // Child ID 1
  gw.present(2, S_BINARY); // Child ID 2
  
  gw.send(msgRelay1.set(0), true); // Send initial value for Relay 1
  gw.send(msgRelay2.set(0), true); // Send initial value for Relay 2
}

void loop() {
  // Check light levels
  int lightLevel1 = analogRead(LIGHT_SENSOR_1_ANALOG_PIN); 
  int lightLevel2 = analogRead(LIGHT_SENSOR_2_ANALOG_PIN); 
  int lightLevel3 = analogRead(LIGHT_SENSOR_3_ANALOG_PIN); 
  int lightLevel4 = analogRead(LIGHT_SENSOR_4_ANALOG_PIN); 
  int lightLevel5 = analogRead(LIGHT_SENSOR_5_ANALOG_PIN); 
  int lightLevel6 = analogRead(LIGHT_SENSOR_6_ANALOG_PIN); 
  
  // Invert the light sensor values: Map the range 0 to 1023 to 100 to 0
  lightLevel1 = map(lightLevel1, 0, 1023, 100, 0);
  lightLevel2 = map(lightLevel2, 0, 1023, 100, 0);
  lightLevel3 = map(lightLevel3, 0, 1023, 100, 0);
  lightLevel4 = map(lightLevel4, 0, 1023, 100, 0);
  lightLevel5 = map(lightLevel5, 0, 1023, 100, 0);
  lightLevel6 = map(lightLevel6, 0, 1023, 100, 0);
  
  Serial.print("Light1: ");
  Serial.println(lightLevel1);
  Serial.print("Light2: ");
  Serial.println(lightLevel2);
  Serial.print("Light3: ");
  Serial.println(lightLevel3);
  Serial.print("Light4: ");
  Serial.println(lightLevel4);
  Serial.print("Light5: ");
  Serial.println(lightLevel5);
  Serial.print("Light6: ");
  Serial.println(lightLevel6);
  
  send(msgLight1.set(lightLevel1));
  send(msgLight2.set(lightLevel2));
  send(msgLight3.set(lightLevel3));
  send(msgLight4.set(lightLevel4));
  send(msgLight5.set(lightLevel5));
  send(msgLight6.set(lightLevel6));

  // Add any other code for the loop here
  delay(1000); // Adjust the delay as needed to control the update rate of the light sensor values
}

// Move this curly brace to the correct position
void loop(){
  gw.process();
}

void incomingMessage(const MyMessage &message)
{
  if (message.type == V_STATUS && message.sensor == 1) {
    // Change relay 1 state.
    digitalWrite(RELAY_PIN_1, message.getBool() ? HIGH : LOW);
    gw.send(msgRelay1.set(message.getBool() ? 1 : 0));
  } else if (message.type == V_STATUS && message.sensor == 2) {
    // Change relay 2 state.
    digitalWrite(RELAY_PIN_2, message.getBool() ? HIGH : LOW);
    gw.send(msgRelay2.set(message.getBool() ? 1 : 0));
  }
}
