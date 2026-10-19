#include "battery.h"
#include "esp32-hal-adc.h"
#include "esp32-hal-gpio.h"
#include "pins_arduino.h"
#include <cstdint>

void Battery::begin() {
    pinMode(A0, INPUT);
}

float Battery::percentage() {
    static const float VOLTAGE_MIN_MAX_DIFF_INV = 1.0f / (MAX_VOLTAGE - MIN_VOLTAGE);

    uint32_t voltageRaw = 0;
    for (int i = 0; i < 16; i++) {
        voltageRaw = voltageRaw + analogReadMilliVolts(A0); // ADC with correction
    }
    float voltage = 2.0 * voltageRaw / 16 / 1000.0;

    return (voltage - MIN_VOLTAGE) * VOLTAGE_MIN_MAX_DIFF_INV * 100.0f;
}
