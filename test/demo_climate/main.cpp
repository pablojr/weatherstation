#include "Climate.h"

// main.cpp doesn't know or care what sensor it's (i.e. DHT11 or DHT22)
Climate climate(5);         // GPIO5

void setup() {
    Serial.begin(115200);
    delay(2000);
    if (!climate.begin()) {
        Serial.println(F("Could not find a valid BMP280 sensor,"
            " check wiring or try a different I2C address!"));        
    };
}

void loop() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        WeatherData data = climate.getData();
        if (data.valid) {
            Serial.print("Temp: ");
            Serial.print(data.temp, 1);
            Serial.println(" °C");
            Serial.print("Hum: ");
            Serial.print(data.hum, 0);
            Serial.println(" %");
            Serial.print("Press: ");
            Serial.print(data.press / 100.0F, 1);
            Serial.println(" hPa");
            lastPrint = millis();
        }
    }
}
