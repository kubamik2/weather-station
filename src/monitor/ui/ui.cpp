#include "ui.h"
#include "U8g2_for_Adafruit_GFX.h"
#include "common/readings.h"
#include "monitor/display.h"
#include <cstdint>
#include <cstring>
#include <variant>
#include "reading_templates/unknown.h"


void ui::printNumber(Display &display, float value, int integerPartPadding) {
    char str[16];
    display.setFont(LARGE_FONT);
    display.printf("%*d", integerPartPadding, (int)value); // print integer part
    display.setFont(SMALL_FONT);
    snprintf(str, 16, "%.1f", value);
    display.print(strchr(str, '.')); // print float part
}

// void ui::UI::printReadings(Display &display, int16_t y, const char *label, float temperature, float humidity, float pressure) {
//     // print label
//     display.setCursor(UI::READINGS_LABEL_X_OFFSET, y + UI::READINGS_LABEL_Y_OFFSET);
//     display.setFont(u8g2_font_profont17_mf);
//     display.print(label);
//
//     // print temperature
//     display.setCursor(UI::READINGS_TEMPERATURE_X_OFFSET, y + UI::READINGS_TEMPERATURE_Y_OFFSET);
//     printNumber(display, temperature, 3);
//
//     // print humidity
//     display.setCursor(UI::READINGS_HUMIDITY_X_OFFSET, y + UI::READINGS_HUMIDITY_Y_OFFSET);
//     printNumber(display, humidity, 2);
//
//     // print pressure
//     display.setCursor(UI::READINGS_PRESSURE_X_OFFSET, y + UI::READINGS_PRESSURE_Y_OFFSET);
//     printNumber(display, pressure, 4);
//
//     // print decorations
//     display.drawBitmap(0, y, READINGS_BITMAP, display.screenWidth(), UI::READINGS_HEIGHT, 0);
// }

uint16_t getBatterySymbol(float batteryPercentage) {
    // static const char SYMBOLS[6] = { '\x0030', '\x0031', '\x0032', '\x0033', '\x0034', '\x0035'};
    static const uint16_t SYMBOLS[10] = { 0xe242, 0xe243, 0xe244, 0xe245, 0xe246, 0xe247, 0xe248, 0xe249, 0xe24a, 0xe24b };
    size_t index = (size_t)(batteryPercentage * 0.1f);
    index = min(index, (size_t)9);
    return SYMBOLS[index];
}

// void ui::UI::printHeader(Display &display, float batteryPercentage) {
//     display.setCursor(0, 20);
//     // display.setFont(u8g2_font_battery19_tn);
//     display.setFont(u8g2_font_siji_t_6x10);
//     display.printf("%c", getBatterySymbol(batteryPercentage));
// }

// ui::UI& ui::UI::setPrimary(const char *label, float temperature, float humidity, float pressure) {
//     mHasPrimary = true;
//     mPrimaryReadings = {
//         .temperature = temperature,
//         .humidty = humidity,
//         .pressure = pressure,
//         .label = label,
//     };
//
//     mUpperBorder = min<uint16_t>(mUpperBorder, PRIMARY_READINGS_Y);
//     mLowerBorder = max<uint16_t>(mLowerBorder, PRIMARY_READINGS_Y + READINGS_HEIGHT);
//     return *this;
// }
//
// ui::UI& ui::UI::setSecondary(const char *label, float temperature, float humidity, float pressure) {
//     mHasSecondary = true;
//     mSecondaryReadings = {
//         .temperature = temperature,
//         .humidty = humidity,
//         .pressure = pressure,
//         .label = label,
//     };
//     mUpperBorder = min<uint16_t>(mUpperBorder, SECONDARY_READINGS_Y);
//     mLowerBorder = max<uint16_t>(mLowerBorder, SECONDARY_READINGS_Y + READINGS_HEIGHT);
//     return *this;
// }
//
// ui::UI& ui::UI::setHeader(float batteryPercentage) {
//     mHasHeader = true;
//     mBatteryPercentage = batteryPercentage;
//     mUpperBorder = min<uint16_t>(mUpperBorder, HEADER_Y);
//     mLowerBorder = max<uint16_t>(mLowerBorder, HEADER_Y + HEADER_HEIGHT);
//     return *this;
// }
//
// void ui::UI::printInner(Display &display) {
//     display.firstPage();
//     do {
//         if (mHasHeader) {
//             this->printHeader(display, mBatteryPercentage);
//         }
//
//         if (mHasPrimary) {
//             this->printReadings(
//                 display,
//                 PRIMARY_READINGS_Y,
//                 mPrimaryReadings.label,
//                 mPrimaryReadings.temperature,
//                 mPrimaryReadings.humidty,
//                 mPrimaryReadings.pressure
//             );
//         }
//
//         if (mHasSecondary) {
//             this->printReadings(
//                 display,
//                 SECONDARY_READINGS_Y,
//                 mSecondaryReadings.label,
//                 mSecondaryReadings.temperature,
//                 mSecondaryReadings.humidty,
//                 mSecondaryReadings.pressure
//             );
//         }
//     } while (display.nextPage());
// }
//
// ui::UI& ui::UI::printPartial(Display &display) {
//     display.setPartialWindow(0, mUpperBorder, display.screenWidth(), mLowerBorder - mUpperBorder + 1);
//     this->printInner(display);
//     return *this;
// }
//
// ui::UI& ui::UI::printFull(Display &display) {
//     display.setFullWindow();
//     this->printInner(display);
//     return *this;
// }


//
// void ui::UI::setReadingTemplate(const char *readingName, ReadingTemplate readingTemplate) {
//     mReadingTemplates.set(readingName, readingTemplate);
// }
//
// std::optional<const ui::ReadingTemplate*> ui::UI::getReadingTemplate(const char *readingName) {
//     return mReadingTemplates.get(readingName);
// }

ui::UIBuilder& ui::UIBuilder::setPrimary(const char *label, Readings readings) {
    mPrimaryReadings = ui::UIElement::ReadingsList(label, std::move(readings));
    mUpperLeft[0] = std::min<int16_t>(mUpperLeft[0], PRIMARY_READINGS_POS[0]);
    mUpperLeft[1] = std::min<int16_t>(mUpperLeft[1], PRIMARY_READINGS_POS[1]);
    mLowerRight[0] = std::max<int16_t>(mLowerRight[0], PRIMARY_READINGS_POS[0] + mPrimaryReadings.getWidth());
    mLowerRight[1] = std::max<int16_t>(mLowerRight[1], PRIMARY_READINGS_POS[1] + mPrimaryReadings.getHeight());
    return *this;
}

ui::UIBuilder& ui::UIBuilder::setSecondary(const char *label, Readings readings) {
    mSecondaryReadings = ui::UIElement::ReadingsList(label, std::move(readings));
    mUpperLeft[0] = std::min<int16_t>(mUpperLeft[0], SECONDARY_READINGS_POS[0]);
    mUpperLeft[1] = std::min<int16_t>(mUpperLeft[1], SECONDARY_READINGS_POS[1]);
    mLowerRight[0] = std::max<int16_t>(mLowerRight[0], SECONDARY_READINGS_POS[0] + mSecondaryReadings.getWidth());
    mLowerRight[1] = std::max<int16_t>(mLowerRight[1], SECONDARY_READINGS_POS[1] + mSecondaryReadings.getHeight());
    return *this;
}

ui::UIBuilder& ui::UIBuilder::setHeader(float batteryPercentage) {
    mHeader = ui::UIElement::Header(batteryPercentage);
    mUpperLeft[0] = std::min<int16_t>(mUpperLeft[0], HEADER_POS[0]);
    mUpperLeft[1] = std::min<int16_t>(mUpperLeft[1], HEADER_POS[1]);
    mLowerRight[0] = std::max<int16_t>(mLowerRight[0], HEADER_POS[0] + mHeader.getWidth());
    mLowerRight[1] = std::max<int16_t>(mLowerRight[1], HEADER_POS[1] + mHeader.getHeight());
    return *this;
}

void ui::UIBuilder::printPartial(Display &display) {
    display.setPartialWindow(mUpperLeft[0], mUpperLeft[1], mLowerRight[0] - mUpperLeft[0] + 1, mLowerRight[1] - mUpperLeft[1] + 1);
    display.firstPage();
    do {
        mPrimaryReadings.draw(display, PRIMARY_READINGS_POS[0], PRIMARY_READINGS_POS[1]);
        mSecondaryReadings.draw(display, SECONDARY_READINGS_POS[0], SECONDARY_READINGS_POS[1]);
        mHeader.draw(display, HEADER_POS[0], HEADER_POS[1]);
    } while (display.nextPage());
}

void ui::UIBuilder::printFull(Display &display) {
    display.setFullWindow();
    display.firstPage();
    do {
        mPrimaryReadings.draw(display, PRIMARY_READINGS_POS[0], PRIMARY_READINGS_POS[1]);
        mSecondaryReadings.draw(display, SECONDARY_READINGS_POS[0], SECONDARY_READINGS_POS[1]);
        mHeader.draw(display, HEADER_POS[0], HEADER_POS[1]);
    } while (display.nextPage());
}

#define ENUM_FORWARD_RETURNING(functionName, args...) std::visit([&](auto&& arg) { \
        using T = std::decay_t<decltype(arg)>; \
        if constexpr (std::is_same_v<T, ui::UIElement::ReadingsList>) \
            result = arg.functionName(args); \
        else if constexpr (std::is_same_v<T, ui::UIElement::Header>) \
            result = arg.functionName(args); \
        else if constexpr (std::is_same_v<T, std::monostate>) \
            result = 0; \
        else \
            static_assert(false, "non-exhaustive visitor"); \
    }, mInner); \

#define ENUM_FORWARD(functionName, args...) std::visit([&](auto&& arg) { \
        using T = std::decay_t<decltype(arg)>; \
        if constexpr (std::is_same_v<T, ui::UIElement::ReadingsList>) \
            arg.functionName(args); \
        else if constexpr (std::is_same_v<T, ui::UIElement::Header>) \
            arg.functionName(args); \
        else if constexpr (std::is_same_v<T, std::monostate>) {} \
        else \
            static_assert(false, "non-exhaustive visitor"); \
    }, mInner); \

int16_t ui::UIElement::getHeight() {
    int16_t result;
    ENUM_FORWARD(getHeight)
    return result;
}

int16_t ui::UIElement::getWidth() {
    int16_t result;
    ENUM_FORWARD(getWidth)
    return result;
}

void ui::UIElement::draw(Display& display, int16_t x, int16_t y) {
    ENUM_FORWARD(draw, display, x, y)
}

int16_t ui::UIElement::ReadingsList::getWidth() {
    int16_t width = LABEL_WIDTH;
    for (auto it = mReadings.cbegin(); it != mReadings.cend(); it++) {
        if (auto readingTemplate = TEMPLATES.get(it->name.data())) {
            width = std::max<int16_t>(width, readingTemplate.value()->width);
        } else {
            width = std::max<int16_t>(width, UNKNOWN_READING_TEMPLATE.width);
        }
    }
    return width;
}

int16_t ui::UIElement::ReadingsList::getHeight() {
    int16_t height = LABEL_HEIGHT;
    for (auto it = mReadings.cbegin(); it != mReadings.cend(); it++) {
        if (auto readingTemplate = TEMPLATES.get(it->name.data())) {
            height += readingTemplate.value()->height;
        } else {
            height += UNKNOWN_READING_TEMPLATE.height;
        }
    }
    return height;
}

void ui::UIElement::ReadingsList::draw(Display &display, int16_t x, int16_t y) {
    // Draw label
    display.drawBitmap(x, y, LABEL_BITMAP, LABEL_WIDTH, LABEL_HEIGHT, 0);
    display.setCursor(x + LABEL_TEXT_X_OFFSET, y + LABEL_TEXT_Y_OFFSET);
    display.setFont(u8g2_font_profont17_mf);
    display.print(mLabel);
    y += LABEL_HEIGHT;

    // Draw readings
    for (auto it = mReadings.cbegin(); it != mReadings.cend(); it++) {
        if (auto readingTemplate = TEMPLATES.get(it->name.data())) {
            (*readingTemplate)->draw(display, x, y, it->item);
            y += (*readingTemplate)->height;
            display.setCursor(x, y);
        } else {
            UNKNOWN_READING_TEMPLATE.draw(display, x, y, 0.0);
            y += UNKNOWN_READING_TEMPLATE.height;
            display.setCursor(x, y);
        }
    }
}

FixedNameMap<ui::ReadingTemplate, Readings::MAX_READING_NAME_SIZE, ui::UIElement::ReadingsList::MAX_TEMPLATES> ui::UIElement::ReadingsList::TEMPLATES;

ui::UIElement::ReadingsList::ReadingsList(const char *label, Readings readings) : mReadings(readings) {
    strncpy(this->mLabel, label, MAX_LABEL_SIZE);
}

void ui::UIElement::ReadingsList::setTemplate(const char *readingName, ReadingTemplate readingTemplate) {
    TEMPLATES.set(readingName, readingTemplate);
}

int16_t ui::UIElement::Header::getWidth() {
    return 0;
}

int16_t ui::UIElement::Header::getHeight() {
    return 0;
}

void ui::UIElement::Header::draw(Display &display, int16_t x, int16_t y) {
    // display.setCursor(BATTERY_SYMBOL_OFFSET[0], BATTERY_SYMBOL_OFFSET[1]);
    // display.setFont(u8g2_font_battery19_tn);
    display.setFont(u8g2_font_siji_t_6x10);
    display.drawGlyph(BATTERY_SYMBOL_OFFSET[0], BATTERY_SYMBOL_OFFSET[1], getBatterySymbol(batteryPercentage));
}
