#include "Climate.h"

// main.cpp doesn't know or care what sensor it's (i.e. DHT11 or DHT22)
Climate climate(5); 

void setup() {
    Serial.begin(115200);
    climate.begin();
}

void loop() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        WeatherData data = climate.getData();
        if (data.valid) {
            Serial.print("Temp: ");
            Serial.print(data.temp);
            Serial.println(" °C");
            Serial.print("Hum: ");
            Serial.print(data.hum);
            Serial.println(" %");
            lastPrint = millis();
        }
    }
}
