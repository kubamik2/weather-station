#pragma once
#include "../display.h"
#include "common/fixedNameMap.h"
#include "common/readings.h"
#include <cstdint>
#include <variant>

#define LARGE_FONT u8g2_font_profont29_mf
#define SMALL_FONT u8g2_font_profont17_mf

namespace ui {
struct ReadingTemplate {
    uint16_t width;
    uint16_t height;
    void (*draw)(Display &display, int16_t x, int16_t y, float value);
};

void printNumber(Display &display, float value, int integerPartPadding);

class UIElement {
public:
    class ReadingsList {
    public:
        ReadingsList(const char *label, Readings readings);
        int16_t getHeight();
        int16_t getWidth();
        void draw(Display &display, int16_t x, int16_t y);
        static void setTemplate(const char *readingName, ReadingTemplate readingTemplate);

    private:
        static constexpr size_t MAX_TEMPLATES = 32;
        static constexpr size_t MAX_LABEL_SIZE = 16;
        static constexpr uint8_t LABEL_BITMAP[368] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 85, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 85, 0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 85, 0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 85, 0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 85, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 85, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0,  0, 0,  0, 0,  0, 0,  0
        };
        static constexpr size_t LABEL_WIDTH = 128;
        static constexpr size_t LABEL_HEIGHT = 23;
        static constexpr size_t LABEL_TEXT_X_OFFSET = 12;
        static constexpr size_t LABEL_TEXT_Y_OFFSET = 17;

        static FixedNameMap<ReadingTemplate, Readings::MAX_READING_NAME_SIZE, MAX_TEMPLATES> TEMPLATES;
        Readings mReadings;
        char mLabel[MAX_LABEL_SIZE];
    };

    class Header {
    public:
        Header(float batteryPercentage)
            : batteryPercentage(batteryPercentage) {
        }
        int16_t getHeight();
        int16_t getWidth();
        void draw(Display &display, int16_t x, int16_t y);

    private:
        static constexpr int16_t BATTERY_SYMBOL_OFFSET[2] = {2, 10};
        float batteryPercentage;
    };

    UIElement()
        : mInner(std::monostate()) {
    }
    UIElement(ReadingsList readingsList)
        : mInner(readingsList) {
    }
    UIElement(Header header)
        : mInner(header) {
    }
    int16_t getHeight();
    int16_t getWidth();
    void draw(Display &display, int16_t x, int16_t y);

private:
    std::variant<ReadingsList, Header, std::monostate> mInner;
};

class UIBuilder {
public:
    UIBuilder &setPrimary(const char *label, Readings readings);
    UIBuilder &setSecondary(const char *label, Readings readings);
    UIBuilder &setHeader(float batteryPercentage);
    void printPartial(Display &display);
    void printFull(Display &display);

private:
    static constexpr int16_t PRIMARY_READINGS_POS[2] = {0, 36};
    static constexpr int16_t SECONDARY_READINGS_POS[2] = {0, 156};
    static constexpr int16_t HEADER_POS[2] = {0, 0};

    UIElement mPrimaryReadings;
    UIElement mSecondaryReadings;
    UIElement mHeader;

    int16_t mUpperLeft[2] = {1000, 1000};
    int16_t mLowerRight[2] = {0, 0};

    void printReadings(Display &display, Readings &readings);
};
} // namespace ui
