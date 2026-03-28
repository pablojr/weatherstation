#ifndef CLIMATE_H
#define CLIMATE_H

#include <DHT.h>
#include <Adafruit_BMP280.h>
#define BMP280_I2C_ADDRESS 0x76     // using I2C interface

// --- MAINTENANCE AREA: Change 'DHT11' to 'DHT22' here when you upgrade ---
#define DHT_ACTIVE_SENSOR_TYPE DHT22

struct WeatherData {
    float temp;
    float hum;
    float press;
    bool valid;
};

class Climate {
private:
    DHT _dht;
    Adafruit_BMP280 _bmp;   // use I2C interface

public:
    // Only takes the pin, hiding the "Type" from main.cpp
    Climate(uint8_t pin) : _dht(pin, DHT_ACTIVE_SENSOR_TYPE) {}

    bool begin() {
        _dht.begin();
        bool status = _bmp.begin(BMP280_I2C_ADDRESS);
        return status;
    }

    WeatherData getData() {
        // average temperature from both sensors
        float t = (_dht.readTemperature() + _bmp.readTemperature()) / 2;
        float h = _dht.readHumidity();      // from DHT sensor
        float p = _bmp.readPressure();      // fron BMP sensor
        if (isnan(t) || isnan(h) || isnan(p)) {
            return {0, 0, 0, false};
        } else {
            return {t, h, p, true};
        }
    }
};

#endif
