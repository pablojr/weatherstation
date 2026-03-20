#include <Wire.h>

class WindDirection {
private:
    static const uint8_t AS5600_I2C_ADDR = 0x36;
    static const uint16_t AS5600_RESOLUTION = 4096;
    static const uint8_t REG_STATUS  = 0x0B;
    static const uint8_t REG_ANGLE_RAW = 0x0C;
    static const uint8_t MAGNET_DETECT_BIT = 0x20;

    // Jitter filter settings
    static const uint8_t JITTER_FILTER_WINDOW_SIZE = 10; 
    uint16_t readings[JITTER_FILTER_WINDOW_SIZE];
    uint8_t readIndex = 0;
    uint32_t total = 0;

    bool isMagnetDetected() {
        Wire.beginTransmission(AS5600_I2C_ADDR);
        Wire.write(REG_STATUS);
        if (Wire.endTransmission(false) != 0) {
            return false;
        }
        Wire.requestFrom(AS5600_I2C_ADDR, (uint8_t)1);
        if (Wire.available() == 1) {
            return (Wire.read() & MAGNET_DETECT_BIT);   // MD bit of STATUS register should be 1
        }
        return false;
    }

public:
    WindDirection() {
        for (uint8_t i = 0; i < JITTER_FILTER_WINDOW_SIZE; i++) readings[i] = 0;
    }

    bool begin() {
        Wire.begin();
        // Returns true if communication is established and magnet is seen
        return isMagnetDetected();
    }

    uint16_t readAngle() {
        Wire.beginTransmission(AS5600_I2C_ADDR);
        Wire.write(REG_ANGLE_RAW);
        if (Wire.endTransmission(false) != 0) {
            return 0;
        }
        Wire.requestFrom(AS5600_I2C_ADDR, (uint8_t)2);
        if (Wire.available() == 2) {
            uint8_t highByte = Wire.read();
            uint8_t lowByte = Wire.read();
            uint16_t raw = ((uint16_t)(highByte & 0x0F) << 8) | lowByte;

            // Update Moving Average (to smooth jitter due to turbulence)
            total = total - readings[readIndex];
            readings[readIndex] = raw;
            total = total + readings[readIndex];
            readIndex = (readIndex + 1) % JITTER_FILTER_WINDOW_SIZE;
        
            return (uint16_t)(total / JITTER_FILTER_WINDOW_SIZE);
        }
        return -1; // No data available
    }

    int getDegrees() {
        int32_t angle = readAngle();
        if (angle < 0) {
            return -1; 
        }
        return (int)((angle * 360L) / AS5600_RESOLUTION);
    }
};
