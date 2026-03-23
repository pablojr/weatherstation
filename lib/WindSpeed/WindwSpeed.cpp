#include "WindSpeed.h"

WindSpeed* WindSpeed::_instance = nullptr;

WindSpeed::WindSpeed(uint8_t pin, float radius_mm) 
    : _pin(pin), _lastPulseTimeUs(0), _pulseDiffUs(0), _newPulseReady(false), _currentRPM(0.0) {
    _radius_m = radius_mm / 1000.0;
    _instance = this;
}

void WindSpeed::begin() {
    pinMode(_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pin), handleInterrupt, FALLING);
}

void IRAM_ATTR WindSpeed::handleInterrupt() {
    uint32_t now = micros();
    uint32_t diff = now - _instance->_lastPulseTimeUs;

    if (diff > _instance->_debounceUs) {
        _instance->_pulseDiffUs = diff;
        _instance->_lastPulseTimeUs = now;
        _instance->_newPulseReady = true;
    }
}

void WindSpeed::update() {
    uint32_t now = micros();

    // 1. Process new pulse data (Math happens here, NOT in ISR)
    if (_newPulseReady) {
        _newPulseReady = false; 
        
        float instantRPM = 60000000.0 / _pulseDiffUs;

        // Exponential Filter
        _currentRPM = (instantRPM * _filterAlpha) + (_currentRPM * (1.0 - _filterAlpha));
    }

    // 2. Handle timeout (wind stopped)
    if (now - _lastPulseTimeUs > _timeoutUs) {
        _currentRPM = 0.0;
    }
}
