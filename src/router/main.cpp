#include "clib/u8g2.h"
#include "common/readings.h"
#include "WiFi.h"
#include "common/communication.h"
#include <Adafruit_BME280.h>
#include "U8g2lib.h"

Adafruit_BME280 bme;

U8G2_SH1106_128X64_NONAME_1_HW_I2C display { U8G2_R0 };

Router& router = Router::getInstance();

uint8_t broadcastAddress[] = {0x58, 0xE6, 0xC5, 0x15, 0xB3, 0x38};
void setup() {
    Serial.begin(9600);
    Serial.print("Hello World!");
    display.begin();
    display.setI2CAddress(0x78);

    if (router.init() != ESP_OK) {
        Serial.println("ESP-NOW: couldn't initialize"); while (1)
          delay(1000);
    }

    unsigned status = bme.begin(0x76);
    // if (!status) {
    //   while (1) {
    //     display.firstPage();
    //     do {
    //       display.setFont(u8g2_font_ncenB14_tr);
    //       display.drawStr(0,20,"no bme280");
    //     } while ( display.nextPage() );
    //     delay(1000);
    //   }
    // }
}

void loop() {
    Serial.println(WiFi.macAddress());
    float temp = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.seaLevelForAltitude(358.0f, bme.readPressure()) / 100.0f;
    Readings &readings = router.getLastReading(0);
    readings.setReading("temperature", temp);
    readings.setReading("humidity", humidity);
    readings.setReading("pressure", pressure);
    display.firstPage();
    do {
      display.setFont(u8g2_font_profont22_mf);
      display.setCursor(0, 15);
      display.printf("%.1f C", temp);
      display.setCursor(0, 37);
      display.printf("%.1f %%", humidity);
      display.setCursor(0, 59);
      display.printf("%.1f hPa", pressure);
    } while ( display.nextPage() );
    delay(60000);
}
