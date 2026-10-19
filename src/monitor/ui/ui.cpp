#include "ui.h"
#include "U8g2_for_Adafruit_GFX.h"
#include "common/readings.h"
#include "monitor/display.h"
#include "reading_templates/unknown.h"
#include <cstdint>
#include <cstring>
#include <variant>

void ui::printNumber(Display &display, float value, int integerPartPadding) {
    char str[16];
    display.setFont(LARGE_FONT);
    display.printf("%*d", integerPartPadding, (int)value); // print integer part
    display.setFont(SMALL_FONT);
    snprintf(str, 16, "%.1f", value);
    display.print(strchr(str, '.')); // print float part
}

uint16_t getBatterySymbol(float batteryPercentage) {
    static const uint16_t SYMBOLS[10] = {0xe242, 0xe243, 0xe244, 0xe245, 0xe246,
                                         0xe247, 0xe248, 0xe249, 0xe24a, 0xe24b};
    size_t index = (size_t)(batteryPercentage * 0.1f);
    index = min(index, (size_t)9);
    return SYMBOLS[index];
}

ui::UIBuilder &ui::UIBuilder::setPrimary(const char *label, Readings readings) {
    mPrimaryReadings = ui::UIElement::ReadingsList(label, std::move(readings));
    mUpperLeft[0] = std::min<int16_t>(mUpperLeft[0], PRIMARY_READINGS_POS[0]);
    mUpperLeft[1] = std::min<int16_t>(mUpperLeft[1], PRIMARY_READINGS_POS[1]);
    mLowerRight[0] = std::max<int16_t>(mLowerRight[0], PRIMARY_READINGS_POS[0] + mPrimaryReadings.getWidth());
    mLowerRight[1] = std::max<int16_t>(mLowerRight[1], PRIMARY_READINGS_POS[1] + mPrimaryReadings.getHeight());
    return *this;
}

ui::UIBuilder &ui::UIBuilder::setSecondary(const char *label, Readings readings) {
    mSecondaryReadings = ui::UIElement::ReadingsList(label, std::move(readings));
    mUpperLeft[0] = std::min<int16_t>(mUpperLeft[0], SECONDARY_READINGS_POS[0]);
    mUpperLeft[1] = std::min<int16_t>(mUpperLeft[1], SECONDARY_READINGS_POS[1]);
    mLowerRight[0] = std::max<int16_t>(mLowerRight[0], SECONDARY_READINGS_POS[0] + mSecondaryReadings.getWidth());
    mLowerRight[1] = std::max<int16_t>(mLowerRight[1], SECONDARY_READINGS_POS[1] + mSecondaryReadings.getHeight());
    return *this;
}

ui::UIBuilder &ui::UIBuilder::setHeader(float batteryPercentage) {
    mHeader = ui::UIElement::Header(batteryPercentage);
    mUpperLeft[0] = std::min<int16_t>(mUpperLeft[0], HEADER_POS[0]);
    mUpperLeft[1] = std::min<int16_t>(mUpperLeft[1], HEADER_POS[1]);
    mLowerRight[0] = std::max<int16_t>(mLowerRight[0], HEADER_POS[0] + mHeader.getWidth());
    mLowerRight[1] = std::max<int16_t>(mLowerRight[1], HEADER_POS[1] + mHeader.getHeight());
    return *this;
}

void ui::UIBuilder::printPartial(Display &display) {
    display.setPartialWindow(
        mUpperLeft[0], mUpperLeft[1], mLowerRight[0] - mUpperLeft[0] + 1, mLowerRight[1] - mUpperLeft[1] + 1
    );
    Serial.printf("%d, %d, %d, %d\n", mUpperLeft[0], mUpperLeft[1], mLowerRight[0], mLowerRight[1]);
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

#define ENUM_FORWARD_RETURNING(functionName, args...)                                                                  \
    std::visit(                                                                                                        \
        [&](auto &&arg) {                                                                                              \
            using T = std::decay_t<decltype(arg)>;                                                                     \
            if constexpr (std::is_same_v<T, ui::UIElement::ReadingsList>)                                              \
                result = arg.functionName(args);                                                                       \
            else if constexpr (std::is_same_v<T, ui::UIElement::Header>)                                               \
                result = arg.functionName(args);                                                                       \
            else if constexpr (std::is_same_v<T, std::monostate>)                                                      \
                result = 0;                                                                                            \
            else                                                                                                       \
                static_assert(false, "non-exhaustive visitor");                                                        \
        },                                                                                                             \
        mInner                                                                                                         \
    );

#define ENUM_FORWARD(functionName, args...)                                                                            \
    std::visit(                                                                                                        \
        [&](auto &&arg) {                                                                                              \
            using T = std::decay_t<decltype(arg)>;                                                                     \
            if constexpr (std::is_same_v<T, ui::UIElement::ReadingsList>)                                              \
                arg.functionName(args);                                                                                \
            else if constexpr (std::is_same_v<T, ui::UIElement::Header>)                                               \
                arg.functionName(args);                                                                                \
            else if constexpr (std::is_same_v<T, std::monostate>) {                                                    \
            } else                                                                                                     \
                static_assert(false, "non-exhaustive visitor");                                                        \
        },                                                                                                             \
        mInner                                                                                                         \
    );

int16_t ui::UIElement::getHeight() {
    int16_t result;
    ENUM_FORWARD_RETURNING(getHeight)
    return result;
}

int16_t ui::UIElement::getWidth() {
    int16_t result;
    ENUM_FORWARD_RETURNING(getWidth)
    return result;
}

void ui::UIElement::draw(Display &display, int16_t x, int16_t y){ENUM_FORWARD(draw, display, x, y)}

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

FixedNameMap<ui::ReadingTemplate, Readings::MAX_READING_NAME_SIZE, ui::UIElement::ReadingsList::MAX_TEMPLATES>
    ui::UIElement::ReadingsList::TEMPLATES;

ui::UIElement::ReadingsList::ReadingsList(const char *label, Readings readings)
    : mReadings(readings) {
    strncpy(this->mLabel, label, MAX_LABEL_SIZE);
}

void ui::UIElement::ReadingsList::setTemplate(const char *readingName, ReadingTemplate readingTemplate) {
    TEMPLATES.set(readingName, readingTemplate);
}

int16_t ui::UIElement::Header::getWidth() {
    return WIDTH;
}

int16_t ui::UIElement::Header::getHeight() {
    return HEIGHT;
}

void ui::UIElement::Header::draw(Display &display, int16_t x, int16_t y) {
    display.setFont(u8g2_font_siji_t_6x10);
    display.drawGlyph(BATTERY_SYMBOL_OFFSET[0], BATTERY_SYMBOL_OFFSET[1], getBatterySymbol(batteryPercentage));
}
