#include "Adafruit_BME280.h"
#include "SPI.h"
#include "common/communication.h"
#include "common/readings.h"
#include "monitor/display.h"
#include "monitor/ui/ui.h"
#include "ui/reading_templates/humidity.h"
#include "ui/reading_templates/pressure.h"
#include "ui/reading_templates/temperature.h"

int cnt;

Adafruit_BME280 bme;
Display display;

RTC_DATA_ATTR bool first_boot = true;
static std::array<uint8_t, ESP_NOW_ETH_ALEN> ROUTER_ADDRESS = {0x10, 0xBD, 0xA3, 0x9D, 0x44, 0xC8};
esp_now_peer_info_t peerInfo;

EndDevice &endDevice = EndDevice::getInstance();

void setup() {
    ui::UIElement::ReadingsList::setTemplate("temperature", TEMPERATURE_READING_TEMPLATE);
    ui::UIElement::ReadingsList::setTemplate("humidity", HUMIDITY_READING_TEMPLATE);
    ui::UIElement::ReadingsList::setTemplate("pressure", PRESSURE_READING_TEMPLATE);
    SPI.begin();
    Serial.begin(9600);
    pinMode(D6, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    if (endDevice.init(ROUTER_ADDRESS, 1) != ESP_OK) {
        Serial.println("ESP-NOW: couldn't initialize");
        while (1)
            delay(1000);
    }

    unsigned status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x");
        Serial.println(bme.sensorID(), 16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1)
            delay(10);
    }
    esp_sleep_enable_timer_wakeup(60 * 1000000ULL);
    display.begin(first_boot);
    if (first_boot) {
        delay(2000);
    }
    first_boot = false;
}

void loop() {
    uint32_t t = millis() / 1000;
    float temp = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.seaLevelForAltitude(358.0f, bme.readPressure()) / 100.0f;
    Readings readings;
    readings.setReading("temperature", temp);
    readings.setReading("humidity", humidity);
    readings.setReading("pressure", pressure);
    if (endDevice.sendReadings(readings) != ESP_OK) {
        Serial.println("EndDevice: failed to send readings");
    }

    uint32_t Vbatt = 0;
    for (int i = 0; i < 16; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0); // ADC with correction
    }
    float Vbattf = 2.0 * Vbatt / 16 / 1000.0;
    char s[16];
    snprintf(s, 16, "%.1f", Vbattf);

    if (endDevice.requestReadings(0) != ESP_OK) {
        Serial.println("EndDevice: failed to send request");
    }

    auto r = endDevice.receiveReadings();
    size_t tries = 10;
    while (!r) {
        if (tries == 0)
            break;
        r = endDevice.receiveReadings();
        tries--;
        delay(50);
    }

    ui::UIBuilder ui;
    ui.setHeader(100.0f).setPrimary("Indoor", std::move(readings));
    if (r) {
        readings = std::move(r.value());
        ui.setSecondary("Outdoor", std::move(r.value()));
    } else {
        Readings empty;
        ui.setSecondary("Outdoor DOWN", empty);
    }

    ui.printFull(display);

    display.hibernate();
    esp_deep_sleep_start();
}
