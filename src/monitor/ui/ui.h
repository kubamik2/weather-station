#pragma once
#include "common/readings.h"
#include "../display.h"
#include <cstdint>
#include <variant>
#include "common/fixedNameMap.h"

#define LARGE_FONT  u8g2_font_profont29_mf
#define SMALL_FONT  u8g2_font_profont17_mf

namespace ui {
struct ReadingTemplate {
    uint16_t width;
    uint16_t height;
    void(*draw)(Display &display, int16_t x, int16_t y, float value);
};

void printNumber(Display &display, float value, int integerPartPadding);

// class UIDraw;
// class UI {
// public:
//     static constexpr size_t MAX_READING_TEMPLATE_COUNT = 16;
//     UI& setPrimary(const char *label, float temperature, float humidity, float pressure);
//     UI& setSecondary(const char *label, float temperature, float humidity, float pressure);
//     UI& setHeader(float batteryPercentage);
//     UI& printPartial(Display &display);
//     UI& printFull(Display &display);
//
//     void setReadingTemplate(const char *readingName, ReadingTemplate readingTemplate);
//     std::optional<const ReadingTemplate*> getReadingTemplate(const char *readingName);
//
//     UIDraw beginDraw(const Display& display);
// private:
//     friend class UIDraw;
//
//     Readings mPrimaryReadings;
//     bool mHasPrimary = false;
//     Readings mSecondaryReadings;
//     bool mHasSecondary = false;
//     bool mHasHeader = false;
//     float mBatteryPercentage;
//
//     uint16_t mUpperBorder = 0;
//     uint16_t mLowerBorder = 0;
//
//     FixedNameMap<ReadingTemplate, Readings::MAX_READING_NAME_SIZE, MAX_READING_TEMPLATE_COUNT> mReadingTemplates;
//
//     void printInner(Display &display);
//
//     static void printReadings(Display &display, int16_t y, const char *label, float temperature, float humidity, float pressure);
//     static void printHeader(Display &display, float batteryPercentage);
//
//     static constexpr uint8_t READINGS_BITMAP[1760] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,224,240,0,0,0,85,0,56,0,0,0,0,0,0,0,3,49,248,0,0,0,0,0,68,0,0,0,0,0,0,0,3,51,12,0,0,0,85,0,68,0,0,0,0,0,0,0,1,227,12,0,0,0,0,0,92,0,0,0,0,0,0,0,0,195,0,0,0,0,85,0,68,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,68,0,0,0,0,0,0,0,0,3,0,0,0,0,85,0,92,0,0,0,0,0,0,0,0,3,12,0,0,0,0,0,68,0,0,0,0,0,0,0,0,3,12,0,0,0,85,0,68,0,0,0,0,0,0,0,0,1,248,0,0,0,0,0,92,0,0,0,0,0,0,0,0,0,240,0,0,0,85,0,68,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,68,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,0,0,0,0,0,0,0,0,0,0,0,0,0,85,1,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,253,0,0,0,0,0,0,0,0,0,0,0,0,0,85,1,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,249,0,0,0,0,0,0,0,0,0,0,0,0,0,85,1,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,204,0,0,0,85,0,16,0,0,0,0,0,0,0,0,1,236,0,0,0,0,0,16,0,0,0,0,0,0,0,0,3,108,0,0,0,85,0,56,0,0,0,0,0,0,0,0,3,88,0,0,0,0,0,56,0,0,0,0,0,0,0,0,1,240,0,0,0,85,0,124,0,0,0,0,0,0,0,0,0,96,0,0,0,0,0,124,0,0,0,0,0,0,0,0,0,248,0,0,0,85,0,254,0,0,0,0,0,0,0,0,1,172,0,0,0,0,0,254,0,0,0,0,0,0,0,0,3,108,0,0,0,85,1,255,0,0,0,0,0,0,0,0,3,120,0,0,0,0,1,255,0,0,0,0,0,0,0,0,3,48,0,0,0,85,3,255,128,0,0,0,0,0,0,0,0,0,0,0,0,0,3,255,128,0,0,0,0,0,0,0,0,0,0,0,0,85,7,255,192,0,0,0,0,0,0,0,0,0,0,0,0,0,7,255,192,0,0,0,0,0,0,0,0,0,0,0,0,85,15,255,224,0,0,0,0,0,0,0,0,0,0,0,0,0,15,255,224,0,0,0,0,0,0,0,0,0,0,0,0,85,15,255,160,0,0,0,0,0,0,0,0,0,0,0,0,0,15,255,160,0,0,0,0,0,0,0,0,0,0,0,0,85,15,255,32,0,0,0,0,0,0,0,0,0,0,0,0,0,15,255,96,0,0,0,0,0,0,0,0,0,0,0,0,85,7,254,64,0,0,0,0,0,0,0,0,0,0,0,0,0,7,252,192,0,0,0,0,0,0,0,0,0,0,0,0,85,3,255,128,0,0,0,0,0,0,0,0,0,0,0,0,0,1,255,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,15,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,224,0,0,0,0,0,0,0,0,12,3,240,0,85,1,128,96,0,0,0,0,0,0,0,0,12,3,248,0,0,3,192,96,0,0,0,0,0,0,0,0,12,3,12,0,85,7,224,96,0,0,0,0,0,0,0,0,15,195,12,63,0,15,243,224,0,0,0,0,0,0,0,0,15,227,248,127,85,1,131,224,0,0,0,0,0,0,0,0,12,51,240,195,0,1,128,96,0,0,0,0,0,0,0,0,12,51,0,195,85,1,128,96,0,0,0,0,0,0,0,0,12,51,0,199,0,1,128,96,0,0,0,0,0,0,0,0,12,51,0,207,85,1,128,96,0,0,0,0,0,0,0,0,12,51,0,123,0,1,143,224,0,0,0,0,0,0,0,0,12,51,0,51,85,1,143,224,0,0,0,0,0,0,0,0,0,0,0,0,0,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,85,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,0,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,85,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,0,1,131,224,0,0,0,0,0,0,0,0,0,0,0,0,85,1,131,224,0,0,0,0,0,0,0,0,0,0,0,0,0,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,85,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,0,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,85,1,128,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,224,0,0,0,0,0,0,0,0,0,0,0,0,85,0,15,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//     static constexpr uint16_t READINGS_LABEL_X_OFFSET = 12;
//     static constexpr uint16_t READINGS_LABEL_Y_OFFSET = 13;
//     static constexpr uint16_t READINGS_TEMPERATURE_X_OFFSET = 37;
//     static constexpr uint16_t READINGS_TEMPERATURE_Y_OFFSET = 44;
//     static constexpr uint16_t READINGS_HUMIDITY_X_OFFSET = 53;
//     static constexpr uint16_t READINGS_HUMIDITY_Y_OFFSET = 75;
//     static constexpr uint16_t READINGS_PRESSURE_X_OFFSET = 37;
//     static constexpr uint16_t READINGS_PRESSURE_Y_OFFSET = 108;
//     static constexpr uint16_t READINGS_HEIGHT = 110;
//
//     static constexpr uint16_t PRIMARY_READINGS_Y = 36;
//     static constexpr uint16_t SECONDARY_READINGS_Y = 156;
//
//     static constexpr uint16_t HEADER_HEIGHT = 20;
//
//     static constexpr uint16_t HEADER_Y = 0;
// };
class UIElement {
public:
    class ReadingsList {
    public:
        ReadingsList(const char *label, Readings readings);
        int16_t getHeight();
        int16_t getWidth();
        void draw(Display& display, int16_t x, int16_t y);
        static void setTemplate(const char *readingName, ReadingTemplate readingTemplate);
    private:
        static constexpr size_t MAX_TEMPLATES = 32;
        static constexpr size_t MAX_LABEL_SIZE = 16;
        static constexpr uint8_t LABEL_BITMAP[368] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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
        Header(float batteryPercentage) : batteryPercentage(batteryPercentage) {}
        int16_t getHeight();
        int16_t getWidth();
        void draw(Display& display, int16_t x, int16_t y);
    private:
        static constexpr int16_t BATTERY_SYMBOL_OFFSET[2] = { 2, 10 };
        float batteryPercentage;
    };

    UIElement() : mInner(std::monostate())  {}
    UIElement(ReadingsList readingsList) : mInner(readingsList)  {}
    UIElement(Header header) : mInner(header)  {}
    int16_t getHeight();
    int16_t getWidth();
    void draw(Display& display, int16_t x, int16_t y);
private:
    std::variant<ReadingsList, Header, std::monostate> mInner;
};

class UIBuilder {
public:
    UIBuilder& setPrimary(const char *label, Readings readings);
    UIBuilder& setSecondary(const char *label, Readings readings);
    UIBuilder& setHeader(float batteryPercentage);
    void printPartial(Display &display);
    void printFull(Display &display);
private:
    static constexpr int16_t PRIMARY_READINGS_POS[2] = { 0, 36 };
    static constexpr int16_t SECONDARY_READINGS_POS[2] = { 0, 156 };
    static constexpr int16_t HEADER_POS[2] = { 0, 0 };

    UIElement mPrimaryReadings;
    UIElement mSecondaryReadings;
    UIElement mHeader;

    int16_t mUpperLeft[2] = { 1000, 1000 };
    int16_t mLowerRight[2] = { 0, 0 };

    void printReadings(Display &display, Readings &readings);
};
} // namespace ui
