#include "HX711.h"

// Pin assignments for the load cell (HX711)
#define LOADCELL_DOUT_PIN  2
#define LOADCELL_SCK_PIN   3

HX711 scale;

float calibration_factor = 476587; // Use the calculated calibration factor

// Pin assignments for the solenoid control
const int valvePin = 9;
const int ledPin = 13;

// Pin assignment for the "system stopped" LED
const int stopLedPin = 12;

// Pin assignment for the buzzer
const int buzzerPin = 4;

const int openPin = A1;
const int closePin = A0;

// Pin assignments for the control buttons
const int startButtonPin = 7;
const int stopButtonPin = 8;

// Weight threshold for auto-stop
const float weightLimit = 1.50; // 1.50 kg

int openValue = 187; // Default open time in ms
int closeValue = 2500; // Default close time in ms

bool systemEnabled = false; // To track if the system is running
bool startButtonPressed = false; // To track the state of the start button
bool stopButtonPressed = false; // To track the state of the stop button

void setup() {
  Serial.begin(57600);

  // Initialize the button pins
  pinMode(startButtonPin, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(stopButtonPin, INPUT_PULLUP);  // Use internal pull-up resistor

  // Initialize the stop LED pin
  pinMode(stopLedPin, OUTPUT);
  digitalWrite(stopLedPin, LOW); // Ensure the stop LED is off initially

  // Initialize the buzzer pin
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Ensure the buzzer is off initially
}

void loop() {
  // Read the button states
  bool startButtonState = digitalRead(startButtonPin) == LOW;
  bool stopButtonState = digitalRead(stopButtonPin) == LOW;

  // Check if the start button is pressed
  if (startButtonState && !startButtonPressed && !systemEnabled) {
    startButtonPressed = true; // Mark the button as pressed
    systemEnabled = true; // Enable the system

    // Initialize the load cell
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor); // Set the calibration factor
    scale.tare(); // Reset the scale to 0

    // Initialize the solenoid control pins
    pinMode(valvePin, OUTPUT);
    pinMode(ledPin, OUTPUT);

    // Turn off the stop LED when the system starts
    digitalWrite(stopLedPin, LOW);

    // Short beep twice when the start button is pressed
    tone(buzzerPin, 1000); // Start tone at 1000 Hz
    delay(100);            // Short beep duration
    noTone(buzzerPin);     // Stop the tone
    delay(100);            // Short delay between beeps
    tone(buzzerPin, 1000); // Start tone at 1000 Hz
    delay(100);            // Short beep duration
    noTone(buzzerPin);     // Stop the tone

    Serial.println("System started.");
  }

  // Check if the stop button is pressed
  if (stopButtonState && !stopButtonPressed && systemEnabled) {
    stopSystem(); // Call the function to stop the system
  }

  // Check if the weight exceeds the limit (auto-stop condition)
  if (systemEnabled && scale.is_ready()) {
    float weight = scale.get_units(10); // Get weight in kg
    Serial.print("Weight: ");
    Serial.print(weight, 2); // Display weight with 2 decimal places
    Serial.println(" kg");

    // If the weight exceeds 1.50 kg, stop the system automatically
    if (weight > weightLimit) {
      Serial.println("Weight limit exceeded. Stopping system.");
      stopSystem(); // Call the function to stop the system
    }
  }

  // Reset the button states when released
  if (!startButtonState) {
    startButtonPressed = false;
  }
  if (!stopButtonState) {
    stopButtonPressed = false;
  }

  if (systemEnabled) {
    // Reading the analog potentiometers for timing control
    openValue = analogRead(openPin);
    openValue = map(openValue, 0, 1023, 0, 4000);
    closeValue = analogRead(closePin);
    closeValue = map(closeValue, 0, 1023, 0, 8000);

    // Solenoid valve control (open)
    digitalWrite(ledPin, HIGH);
    digitalWrite(valvePin, HIGH);
    delay(openValue); // Delay for the open time

    // Solenoid valve control (close)
    digitalWrite(ledPin, LOW);
    digitalWrite(valvePin, LOW);
    delay(closeValue); // Delay for the close time
  } else {
    // Ensure the valve and LED are off when the system is disabled
    digitalWrite(valvePin, LOW);
    digitalWrite(ledPin, LOW);
  }
}

// Function to stop the system
void stopSystem() {
  stopButtonPressed = true; // Mark the stop button as pressed
  systemEnabled = false; // Disable the system

  // Ensure the valve and LED are turned off when stopping
  digitalWrite(valvePin, LOW);
  digitalWrite(ledPin, LOW);

  // Turn on the stop LED when the system stops
  digitalWrite(stopLedPin, HIGH);

  // Long beep when the system stops
  tone(buzzerPin, 1000); // Start tone at 1000 Hz
  delay(500);            // Long beep duration
  noTone(buzzerPin);     // Stop the tone

  Serial.println("System stopped.");
}
