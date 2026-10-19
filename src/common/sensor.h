#pragma once
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <cstdint>

class Sensor {
public:
    typedef struct {
        float temperature;
        float humidity;
        float pressure;
    } Readings;

    Sensor(float height);
    Readings getReadings();

private:
    Adafruit_BME280 mBME;
    float mHeight;
    static constexpr uint8_t BME_I2S_ADDRESS = 0x76;
};
