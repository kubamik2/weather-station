#include "battery.h"
#include <algorithm>

// void Battery::begin() {
    // pinMode(PIN_VBAT, INPUT);
    // pinMode(VBAT_ENABLE, OUTPUT);
    // digitalWrite(VBAT_ENABLE, LOW);
    // analogReadResolution(12);
    // analogReference(AR_INTERNAL_3_0);
// }

// Returns the mean of collected readings
// float Battery::readPercentage() {
//     float mean = 0.0;
//
//     for (int i = 0; i < MAX_READING_COUNT; i++) {
//         mean += mReadings[i];
//     }
//
//     return mean / (float)MAX_READING_COUNT;
// }
//
// void Battery::collectReadings() {
//     static constexpr float VOLTAGE_CONVERSION_MULTIPLIER = 2.91f * 3.0f / 4095.0f; // Resistance divider * reference voltage / resolution
//     enableReadingMode();
//     delay(500);
//     for (int i = 0; i < READING_COUNT_PER_COLLECT; i++) {
//         // Read raw value from adc
//         uint32_t raw = analogRead(PIN_VBAT);
//
//         // Convert raw value to voltage
//         float voltage = (float)raw * VOLTAGE_CONVERSION_MULTIPLIER;
//
//         // Convert voltage to percetange
//         float percentage = (voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE) * 100.0f;
//
//         // Store result in circular buffer
//         mReadings[mNextReadingIndex] = percentage;
//         mNextReadingIndex = (mNextReadingIndex + 1) % MAX_READING_COUNT;
//         mReadingCount = std::min(mReadingCount, MAX_READING_COUNT);
//         delay(100);
//     }
//     disableReadingMode();
// }
//
// void Battery::enableReadingMode() {
//     pinMode(VBAT_ENABLE, LOW);
// }
//
// void Battery::disableReadingMode() {
//     pinMode(VBAT_ENABLE, HIGH);
// }
