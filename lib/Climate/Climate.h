#ifndef CLIMATE_H
#define CLIMATE_H

#include <DHT.h>

// --- MAINTENANCE AREA: Change 'DHT11' to 'DHT22' here when you upgrade ---
#define ACTIVE_SENSOR_TYPE DHT22

struct WeatherData {
    float temp;
    float hum;
    bool valid;
};

class Climate {
private:
    DHT _dht;

public:
    // Only takes the pin, hiding the "Type" from main.cpp
    Climate(uint8_t pin) : _dht(pin, ACTIVE_SENSOR_TYPE) {}

    void begin() {
        _dht.begin();
    }

    WeatherData getData() {
        float t = _dht.readTemperature();
        float h = _dht.readHumidity();
        
        if (isnan(t) || isnan(h)) return {0, 0, false};
        return {t, h, true};
    }
};

#endif
