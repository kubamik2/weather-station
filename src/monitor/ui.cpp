#include "ui.h"
#include "U8g2_for_Adafruit_GFX.h"
#include "display.h"


void printNumber(Display &display, float value, int integerPartPadding) {
    char str[16];
    display.setFont(LARGE_FONT);
    display.printf("%*d", integerPartPadding, (int)value); // print integer part
    display.setFont(SMALL_FONT);
    snprintf(str, 16, "%.1f", value);
    display.print(strchr(str, '.')); // print float part
}

void UIBuilder::printReadings(Display &display, int16_t y, const char *label, float temperature, float humidity, float pressure) {
    // print label
    display.setCursor(UIBuilder::READINGS_LABEL_X_OFFSET, y + UIBuilder::READINGS_LABEL_Y_OFFSET);
    display.setFont(u8g2_font_profont17_mf);
    display.print(label);

    // print temperature
    display.setCursor(UIBuilder::READINGS_TEMPERATURE_X_OFFSET, y + UIBuilder::READINGS_TEMPERATURE_Y_OFFSET);
    printNumber(display, temperature, 3);

    // print humidity
    display.setCursor(UIBuilder::READINGS_HUMIDITY_X_OFFSET, y + UIBuilder::READINGS_HUMIDITY_Y_OFFSET);
    printNumber(display, humidity, 2);

    // print pressure
    display.setCursor(UIBuilder::READINGS_PRESSURE_X_OFFSET, y + UIBuilder::READINGS_PRESSURE_Y_OFFSET);
    printNumber(display, pressure, 4);

    // print decorations
    display.drawBitmap(0, y, READINGS_BITMAP, display.screenWidth(), UIBuilder::READINGS_HEIGHT, 0);
}

char getBatterySymbol(float batteryPercentage) {
    static const char SYMBOLS[6] = { '\x0030', '\x0031', '\x0032', '\x0033', '\x0034', '\x0035'};
    size_t index = (size_t)(batteryPercentage * 0.06f); // X% / 16.666
    index = min(index, (size_t)5);
    return SYMBOLS[index];
}

void UIBuilder::printHeader(Display &display, float batteryPercentage) {
    display.setCursor(0, 20);
    // display.setFont(u8g2_font_battery19_tn);
    display.setFont(u8g2_font_siji_t_6x10);
    display.printf("%c", getBatterySymbol(batteryPercentage));
}

UIBuilder& UIBuilder::withPrimary(const char *label, float temperature, float humidity, float pressure) {
    mHasPrimary = true;
    mPrimaryReadings = {
        .temperature = temperature,
        .humidty = humidity,
        .pressure = pressure,
        .label = label,
    };

    mUpperBorder = min<uint16_t>(mUpperBorder, PRIMARY_READINGS_Y);
    mLowerBorder = max<uint16_t>(mLowerBorder, PRIMARY_READINGS_Y + READINGS_HEIGHT);
    return *this;
}

UIBuilder& UIBuilder::withSecondary(const char *label, float temperature, float humidity, float pressure) {
    mHasSecondary = true;
    mSecondaryReadings = {
        .temperature = temperature,
        .humidty = humidity,
        .pressure = pressure,
        .label = label,
    };
    mUpperBorder = min<uint16_t>(mUpperBorder, SECONDARY_READINGS_Y);
    mLowerBorder = max<uint16_t>(mLowerBorder, SECONDARY_READINGS_Y + READINGS_HEIGHT);
    return *this;
}

UIBuilder& UIBuilder::withHeader(float batteryPercentage) {
    mHasHeader = true;
    mBatteryPercentage = batteryPercentage;
    mUpperBorder = min<uint16_t>(mUpperBorder, HEADER_Y);
    mLowerBorder = max<uint16_t>(mLowerBorder, HEADER_Y + HEADER_HEIGHT);
    return *this;
}

void UIBuilder::printInner(Display &display) {
    display.firstPage();
    do {
        if (mHasHeader) {
            this->printHeader(display, mBatteryPercentage);
        }

        if (mHasPrimary) {
            this->printReadings(
                display,
                PRIMARY_READINGS_Y,
                mPrimaryReadings.label,
                mPrimaryReadings.temperature,
                mPrimaryReadings.humidty,
                mPrimaryReadings.pressure
            );
        }

        if (mHasSecondary) {
            this->printReadings(
                display,
                SECONDARY_READINGS_Y,
                mSecondaryReadings.label,
                mSecondaryReadings.temperature,
                mSecondaryReadings.humidty,
                mSecondaryReadings.pressure
            );
        }
    } while (display.nextPage());
}

UIBuilder& UIBuilder::printPartial(Display &display) {
    display.setPartialWindow(0, mUpperBorder, display.screenWidth(), mLowerBorder - mUpperBorder + 1);
    this->printInner(display);
    return *this;
}

UIBuilder& UIBuilder::printFull(Display &display) {
    display.setFullWindow();
    this->printInner(display);
    return *this;
}
