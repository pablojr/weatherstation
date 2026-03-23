#ifndef WINDSPEED_H
#define WINDSPEED_H

#include <Arduino.h>

class WindSpeed {
public:
    enum ResponseMode {
        ROCK_STEADY = 5,        // Alpha = 0.05 (Very smooth, slow to change)
        RESPONSIVE = 25,        // Alpha = 0.25 (Balanced)
        FAST_REACTION = 85      // Alpha = 0.85 (Instant feedback, jumpy)
    }; // Values represent Alpha * 100

    WindSpeed(uint8_t pin, float radius_mm);
    void begin();
    void update();
    void setResponse(ResponseMode mode) { _filterAlpha = mode / 100.0f; }

    float getRPM() const { return _currentRPM; }
    float getSpeedMS() const { return (_currentRPM * 2.0 * PI * _radius_m) / 60.0; }

private:
    static void IRAM_ATTR handleInterrupt();
    static WindSpeed* _instance;

    uint8_t _pin;
    float _radius_m;
    
    // Use volatile and atomic-safe types for ISR
    volatile uint32_t _lastPulseTimeUs;
    volatile uint32_t _pulseDiffUs;
    volatile bool _newPulseReady;
    
    float _currentRPM;
    
    const uint32_t _debounceUs = 5000;    
    const uint32_t _timeoutUs = 3000000;  
    float _filterAlpha = ResponseMode::RESPONSIVE / 100.0f;
};

#endif
