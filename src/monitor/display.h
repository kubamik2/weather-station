#pragma once

#include "common.h"
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <SPI.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <cstdint>

class Display {
public:
    Display()
        : mDriver(DISPLAY_CLASS{CS, DC, RST, BUSY}) {
    }
    void begin(bool clear);
    void printIndoor(float temperature, float humidity, float pressure);
    void printOutdoor(float temperature, float humidity, float pressure);
    void printBattery();
    uint16_t screenWidth() const noexcept;
    uint16_t screenHeight() const noexcept;

    template <typename... Args> void printf(const char *format, Args &&...args) {
        mU8g2Fonts.printf(format, std::forward<Args>(args)...);
    }

    template <typename... Args> void printfCenter(const char *format, Args &&...args) {
        static const size_t MAX_LEN = 64;
        char str[MAX_LEN];
        snprintf(str, MAX_LEN, format, std::forward<Args>(args)...);
        int16_t width = getUTF8Width(str);
        mU8g2Fonts.setCursor((screenWidth() - width) / 2, getCursorY());
        print(str);
    }

    void printCenter(const char *str);

    void print(const char *str) {
        mU8g2Fonts.print(str);
    }

    int16_t getCursorX() {
        return mU8g2Fonts.getCursorX();
    }

    int16_t getCursorY() {
        return mU8g2Fonts.getCursorY();
    }

    void setCursor(int16_t x, int16_t y) {
        mU8g2Fonts.setCursor(x, y);
    }

    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
        mDriver.drawBitmap(x, y, bitmap, w, h, color);
    }

    void firstPage() {
        mDriver.firstPage();
    }

    bool nextPage() {
        return mDriver.nextPage();
    }

    void setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        mDriver.setPartialWindow(x, y, w, h);
    }

    void setFullWindow() {
        mDriver.setFullWindow();
    }

    int16_t getUTF8Width(const char *str) {
        return mU8g2Fonts.getUTF8Width(str);
    }

    void setFont(const uint8_t *font) {
        mU8g2Fonts.setFont(font);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        mDriver.drawPixel(x, y, color);
    }

    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) {
        mDriver.drawRoundRect(x0, y0, w, h, radius, color);
    }

    int8_t getFontAscent() {
        return mU8g2Fonts.getFontAscent();
    }

    int8_t getFontDescent() {
        return mU8g2Fonts.getFontDescent();
    }

    void clearScreen(uint8_t value = 0xFF) {
        mDriver.clearScreen(value);
    }

    void hibernate() {
        mDriver.hibernate();
    }

    void drawGlyph(int16_t x, int16_t y, uint16_t e) {
        mU8g2Fonts.drawGlyph(x, y, e);
    }

private:
    void alignCursorCenter(int width);
    void alignCursorLeft();
    void alignCursorRight(int width);

    GxEPD2_BW<DISPLAY_CLASS, DISPLAY_CLASS::HEIGHT> mDriver;
    U8G2_FOR_ADAFRUIT_GFX mU8g2Fonts;
    int16_t mLargeFontWidth;
    int16_t mSmallFontWidth;
    int16_t mLabelFontWidth;

    int16_t mLargeFontHeight;
    int16_t mSmallFontHeight;
    int16_t mLabelFontHeight;

    static constexpr int16_t LABEL_OFFSET_Y = 14;
    static constexpr int16_t READINGS_LINE_OFFSET_Y = 4;
    static constexpr int16_t BORDER_OFFSET = 3;
};
