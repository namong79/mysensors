// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached 
#define MY_RADIO_NRF24

#include <MySensors.h>  
#include <Wire.h>
#include "Adafruit_HTU21DF.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

// Set this offset if the sensor has a permanent small offset to the real temperatures
#define SENSOR_TEMP_OFFSET 0

// Sleep time between sensor updates (in milliseconds) 
// Update sensor values per 1 Min

static const uint64_t UPDATE_INTERVAL = 60000;

// Force sending an update of the temperature after n sensor reads, so a controller showing the
// timestamp of the last update doesn't show something like 3 hours in the unlikely case, that
// the value didn't change since;
// i.e. the sensor would force sending an update every UPDATE_INTERVAL*FORCE_UPDATE_N_READS [ms]
static const uint8_t FORCE_UPDATE_N_READS = 10;

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1

#define MY_NODE_ID 3

float lastTemp;
float lastHum;
uint8_t nNoUpdatesTemp;
uint8_t nNoUpdatesHum;
bool metric = true;

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

void presentation()  
{ 
  // Send the sketch version information to the gateway
  sendSketchInfo("HTU2x Temp/Humidity Sensor", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("HTU21D-F test");

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
    }
}

void loop(){
  // Get temperature from HTU21DF library
  float temperature = (htu.readTemperature());
 if (temperature != lastTemp || nNoUpdatesTemp == FORCE_UPDATE_N_READS) {
    // Only send temperature if it changed since the last measurement or if we didn't send an update for n times
    lastTemp = temperature;
    
    // Reset no updates counter
    nNoUpdatesTemp = 0;
    temperature += SENSOR_TEMP_OFFSET;
    send(msgTemp.set(temperature, 1));

    #ifdef MY_DEBUG
    Serial.print("T: ");
    Serial.println(temperature);
    #endif
  } 
  else {
    // Increase no update counter if the temperature stayed the same
    nNoUpdatesTemp++;
  }

  // Get humidity from HTU21DF library
  float humidity = (htu.readHumidity());
  if (humidity != lastHum || nNoUpdatesHum == FORCE_UPDATE_N_READS) {
    // Only send humidity if it changed since the last measurement or if we didn't send an update for n times
    lastHum = humidity;
    // Reset no updates counter
    nNoUpdatesHum = 0;
    send(msgHum.set(humidity, 1));

    #ifdef MY_DEBUG
    Serial.print("H: ");
    Serial.println(humidity);
    #endif
}
  else {
    // Increase no update counter if the humidity stayed the same
    nNoUpdatesHum++;
  }

  // Sleep for a while to save energy
  sleep(UPDATE_INTERVAL); 
}