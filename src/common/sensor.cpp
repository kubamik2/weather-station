#include "sensor.h"

Sensor::Sensor(float height)
    : mHeight(height) {
}

Sensor::Readings Sensor::getReadings() {
    return {
        .temperature = mBME.readTemperature(),
        .humidity = mBME.readHumidity(),
        .pressure = mBME.seaLevelForAltitude(mHeight, mBME.readPressure()) / 100.0f,
    };
}
