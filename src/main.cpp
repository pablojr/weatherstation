#include <Arduino.h>
#include "WindDirection.h"

// Create the wind direction sensor object
WindDirection sensorWindDirection;

void setup() {
    Serial.begin(115200);
    
    Serial.println("Initializing Wind Direction Sensor...");
    
    // Try to initialize; stay in loop if magnet isn't found
    while (!sensorWindDirection.begin()) {
        Serial.println("Error: Magnet not detected or I2C error. Retrying...");
        delay(2000);
    }

    Serial.println("Wind Direction Sensor Ready.");
}

void loop() {
    uint16_t rawValue = sensorWindDirection.readAngle();
    int degrees = sensorWindDirection.getDegrees();

    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print(" | Degrees: ");
    Serial.println(degrees);

    delay(1000); // Read every half second
}
