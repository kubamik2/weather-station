#include "display.h"
#include "GxEPD2.h"
#include <cstdio>

const uint32_t SCREEN_WIDTH = 128;
const uint32_t SCREEN_HEIGHT = 296;

#define LARGE_FONT u8g2_font_profont29_mf
#define SMALL_FONT u8g2_font_profont17_mf
#define LABEL_FONT u8g2_font_8x13B_mf

void Display::begin(bool clear) {
    mDriver.epd2.selectSPI(SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));
    mDriver.init(115200, clear);
    mU8g2Fonts.begin(mDriver);

    mDriver.setTextColor(GxEPD_BLACK);
    mDriver.setRotation(0); // Vertical orientation
    mU8g2Fonts.setFontDirection(0);
    mU8g2Fonts.setFontMode(1);
    mU8g2Fonts.setForegroundColor(GxEPD_BLACK);
    mU8g2Fonts.setBackgroundColor(GxEPD_WHITE);

    mU8g2Fonts.setFont(LARGE_FONT);
    mLargeFontWidth = mU8g2Fonts.getUTF8Width(" ");
    mLargeFontHeight = mU8g2Fonts.getFontAscent() - mU8g2Fonts.getFontDescent();
    mU8g2Fonts.setFont(SMALL_FONT);
    mSmallFontWidth = mU8g2Fonts.getUTF8Width(" ");
    mSmallFontHeight = mU8g2Fonts.getFontAscent() - mU8g2Fonts.getFontDescent();
    mU8g2Fonts.setFont(LABEL_FONT);
    mLabelFontWidth = mU8g2Fonts.getUTF8Width(" ");
    mLabelFontHeight = mU8g2Fonts.getFontAscent() - mU8g2Fonts.getFontDescent();
}

uint16_t Display::screenWidth() const noexcept {
    return DISPLAY_CLASS::WIDTH;
}

uint16_t Display::screenHeight() const noexcept {
    return DISPLAY_CLASS::HEIGHT;
}

void Display::alignCursorCenter(int width) {
    mU8g2Fonts.setCursor((screenWidth() - width) / 2, mU8g2Fonts.getCursorY());
}

void Display::alignCursorLeft() {
    mU8g2Fonts.setCursor(0, mU8g2Fonts.getCursorY());
}

void Display::alignCursorRight(int width) {
    mU8g2Fonts.setCursor(screenWidth() - width, mU8g2Fonts.getCursorY());
}

void Display::printCenter(const char *str) {
    int16_t width = getUTF8Width(str);
    mU8g2Fonts.setCursor((screenWidth() - width) / 2, getCursorY());
    print(str);
}
