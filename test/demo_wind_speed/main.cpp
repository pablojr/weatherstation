#include "WindSpeed.h"

WindSpeed anemometer(4, 60);      // GPIO4, rotating element radius

void setup() {
    Serial.begin(115200);
    anemometer.begin();
}

void loop() {
    anemometer.update();
    
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        Serial.print("Speed: ");
        Serial.print(anemometer.getSpeedMS());
        Serial.println(" m/s");
        Serial.print("RPM: ");
        Serial.println(anemometer.getRPM());
        lastPrint = millis();
    }
}
