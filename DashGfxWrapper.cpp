#include "DashGfxWrapper.hpp"

#include <Arduino.h>

#include "font.h"

const GfxColorScheme IDashGfxWrapper::m_colorScheme;

bool DashRA8875GfxWrapper::setup()
{
    Serial.print("TFT init... ");
    /* Initialize the display using 'RA8875_480x80', 'RA8875_480x128', 'RA8875_480x272' or 'RA8875_800x480' */
    if (!m_tft.begin(RA8875_800x480))
    {
        Serial.println(" error.");
        return false;
    }

    // Reverse scan direction (register 0x20, bit 2 and 3) for both X and Y axis to rotate image 180 degrees
    // (related to physical composition of the device).
    m_tft.writeReg(0x20, 0x0C);

    m_tft.displayOn(true);
    m_tft.GPIOX(true);                              // Enable TFT - display enable tied to GPIOX
    m_tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    setBrightness(brightness());

    fillScreen(RA8875_BLACK);

    constexpr char *msg = "INIT";
    textWrite(0, 0, 1, colorScheme().textColor, colorScheme().textBackground, msg);

    // Load font into RA8775 controller memory
    // Built-int RA8775 font is Latin-1. Custom font is required for cyrillic support.
    loadFont(&epd_bitmap_font[0], 960, 32, ' ', 212);

    Serial.println(" done.");

    return true;
}

int DashRA8875GfxWrapper::width()
{
    return m_tft.width();
}

int DashRA8875GfxWrapper::height()
{
    return m_tft.height();
}

void DashRA8875GfxWrapper::fillScreen(unsigned int color)
{
    for (int i = 0; i < 5; ++i)
    {
        m_tft.fillScreen(color);
    }
}

void DashRA8875GfxWrapper::drawLine(int x0, int y0, int x1, int y1, unsigned int color)
{
    m_tft.drawLine(x0, y0, x1, y1, color);
}

void DashRA8875GfxWrapper::drawRect(int x, int y, int w, int h, unsigned int color)
{
    m_tft.drawRect(x, y, w, h, color);
}

void DashRA8875GfxWrapper::fillRect(int x, int y, int w, int h, unsigned int color)
{
    m_tft.fillRect(x, y, w, h, color);
}

void DashRA8875GfxWrapper::drawCircle(int x, int y, int r, unsigned int color)
{
    m_tft.drawCircle(x, y, r, color);
}

void DashRA8875GfxWrapper::fillCircle(int x, int y, int r, unsigned int color)
{
    m_tft.fillCircle(x, y, r, color);
}

void DashRA8875GfxWrapper::drawRoundRect(int x, int y, int w, int h, int r,
                                         unsigned int color)
{
    m_tft.drawRoundRect(x, y, w, h, r, color);
}

void DashRA8875GfxWrapper::fillRoundRect(int x, int y, int w, int h, int r,
                                         unsigned int color)
{
    m_tft.fillRoundRect(x, y, w, h, r, color);
}

void DashRA8875GfxWrapper::textWrite(int x, int y, int scale, int color, int bkgnd, const char *buf)
{
    const int len = strlen(buf);

    m_tft.textMode();
    // m_tft.writeReg(0x21, 0x80);     // Reg 0x21 bit 7 - CGRAM select
    m_tft.textSetCursor(x, y);
    m_tft.textEnlarge(scale);
    m_tft.textColor(color, bkgnd);
    m_tft.textWrite(buf, len);
    m_tft.graphicsMode();
}

void DashRA8875GfxWrapper::userTextWrite(int x, int y, int scale, int color, int bkgnd, const char *buf)
{
    int len = strlen(buf);

    m_tft.textMode();
    m_tft.textSetCursor(x, y);
    m_tft.textEnlarge(scale);
    m_tft.textColor(color, bkgnd);

    uint8_t oldRegState = m_tft.readReg(0x21);
    uint8_t temp = oldRegState;
    bitSet(temp, 7); // Select CGRAM.
    m_tft.writeReg(0x21, temp);

    temp = m_tft.readReg(0x41);
    temp &= 0b11110011; // Select memory bank.
    m_tft.writeReg(0x41, temp);

    m_tft.writeCommand(RA8875_MRWC);
    for (int i = 0; i < len; ++i)
    {
        m_tft.writeData(buf[i]);
        if (scale >= 1) // Scale==2 doesn't work without this.
            delay(1);
    }
    m_tft.writeReg(0x21, oldRegState);

    m_tft.graphicsMode();
}

void DashRA8875GfxWrapper::print(int x, int y, int color, int bkgnd,
                                 const char *buf)
{
    if (buf == nullptr)
    {
        return;
    }

    m_tft.setCursor(x, y);
    m_tft.setTextColor(color, bkgnd);
    for (int i = 0; buf[i] != 0; ++i)
    {
        m_tft.write(buf[i]);
    }
}

void DashRA8875GfxWrapper::loadUserChar(const uint8_t symbol[], uint8_t address)
{
    uint8_t reg0x41 = m_tft.readReg(0x41);
    uint8_t reg0x40 = m_tft.readReg(0x40);
    uint8_t temp;

    temp = reg0x40;
    temp &= ~(1 << 7); // Select graphics mode - clear bit 7.
    m_tft.writeReg(0x40, temp);

    m_tft.writeReg(0x23, address);

    uint8_t reg0x21 = m_tft.readReg(0x21);
    temp = reg0x21;
    temp &= ~(1 << 7); // Clear bit 7.
    m_tft.writeReg(0x21, temp);

    temp = reg0x41;
    temp &= ~(1 << 3); // Clear bit 3.
    temp |= (1 << 2);  // Set bit 2.

    m_tft.writeReg(0x41, temp);

    m_tft.writeCommand(0x02);
    for (uint8_t i = 0; i < 16; ++i)
    {
        m_tft.writeData(symbol[i]);
    }

    // Restore registers.
    m_tft.writeReg(0x21, reg0x21);
    m_tft.writeReg(0x40, reg0x40);
    m_tft.writeReg(0x41, reg0x41);
}

void DashRA8875GfxWrapper::drawUserChar(uint8_t symbolAddrs)
{
    m_tft.textMode();

    uint8_t oldRegState = m_tft.readReg(0x21);
    uint8_t temp = oldRegState;
    bitSet(temp, 7); // set to CGRAM.
    m_tft.writeReg(0x21, temp);

    temp = m_tft.readReg(0x41);
    temp &= 0b11110010; // Clear bits 0, 2, 3.
    m_tft.writeReg(0x41, temp);

    m_tft.writeCommand(RA8875_MRWC);
    m_tft.writeData(symbolAddrs);

    m_tft.writeReg(0x21, oldRegState);
}

void DashRA8875GfxWrapper::loadFont(const unsigned char *font, int width, int height, int first_char, int char_count)
{
    int chars_per_row = width / 8;

    for (int char_idx = 0; char_idx < char_count; ++char_idx)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "Loading font: char %d", first_char + char_idx);
        textWrite(0, 32, 1, colorScheme().textColor, colorScheme().textBackground, buf);

        unsigned char char_data[16];
        int char_row = char_idx / chars_per_row;
        int char_col = char_idx % chars_per_row;

        for (int row = 0; row < 16; ++row)
        {
            // Each row in the bitmap is width/8 bytes
            const unsigned char *row_ptr = font + (char_row * 16 + row) * (width / 8);
            char_data[row] = pgm_read_byte_near(row_ptr + char_col);
        }

        loadUserChar(char_data, first_char + char_idx);
    }
    m_tft.writeReg(0x21, 0x80); // Reg 0x21 bit 7 - CGRAM select
    m_tft.writeReg(0x41, 0x00); // Switch back to display memory
}

void DashRA8875GfxWrapper::drawPixels(uint16_t *p, uint32_t count, int16_t x, int16_t y)
{
    m_tft.drawPixels(p, count, x, y);
}

uint8_t DashRA8875GfxWrapper::brightness()
{
    return m_brightness;
}

void DashRA8875GfxWrapper::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
    m_tft.PWM1out(m_brightness);
}

void DashRA8875GfxWrapper::setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h,
                                           uint8_t mode)
{
    m_tft.setScrollWindow(x, y, w, h, mode);
}

void DashRA8875GfxWrapper::scrollX(int16_t dist)
{
    m_tft.scrollX(dist);
}

void DashRA8875GfxWrapper::scrollY(int16_t dist)
{
    m_tft.scrollY(dist);
}

void DashRA8875GfxWrapper::setActiveWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    if (w == 0)
    {
        w = m_tft.width();
    }

    if (h == 0)
    {
        h = m_tft.height();
    }

    /* Set active window X */
    m_tft.writeReg(RA8875_HSAW0, x & 0xFF); // horizontal start point
    m_tft.writeReg(RA8875_HSAW1, x >> 8);
    m_tft.writeReg(RA8875_HEAW0, (w - 1) & 0xFF); // horizontal end point
    m_tft.writeReg(RA8875_HEAW1, (w - 1) >> 8);

    /* Set active window Y */
    m_tft.writeReg(RA8875_VSAW0, y & 0xFF); // vertical start point
    m_tft.writeReg(RA8875_VSAW1, y >> 8);
    m_tft.writeReg(RA8875_VEAW0, (h - 1) & 0xFF); // vertical end point
    m_tft.writeReg(RA8875_VEAW1, (h - 1) >> 8);
}
