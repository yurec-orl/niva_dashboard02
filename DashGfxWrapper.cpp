#include "DashGfxWrapper.hpp"

#include <Arduino.h>

#include "font.h"

const GfxColorScheme IDashGfxWrapper::m_colorScheme;

bool DashRA8875GfxWrapper::setup()
{
    /* Initialize the display using 'RA8875_480x80', 'RA8875_480x128', 'RA8875_480x272' or 'RA8875_800x480' */
    if (!m_tft.begin(RA8875_800x480))
    {
        return false;
    }

    // Reverse scan direction (register 0x20, bit 2 and 3) for both X and Y axis to rotate image 180 degrees (related to physical composition of the device).
    m_tft.writeReg(0x20, 0x0C); // reverse scan direction for X

    m_tft.displayOn(true);
    m_tft.GPIOX(true); // Enable TFT - display enable tied to GPIOX
    m_tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    m_tft.PWM1out(16);

    m_tft.fillScreen(RA8875_BLACK);

    constexpr char *msg = "INIT";
    textWrite(0, 0, 1, colorScheme().textColor, colorScheme().textBackground, msg);

    // Load font into RA8775 controller memory
    loadFont(&epd_bitmap_font[0], 960, 32, ' ', 212);

    char *text = "asdfgäë¢ ¯à®«";
    userTextWrite(64, 64, 2, colorScheme().textColor, colorScheme().textBackground, text);

    for (;;)
        delay(100);

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
    m_tft.fillScreen(color);
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
    //m_tft.writeReg(0x21, 0x80);     // Reg 0x21 bit 7 - CGRAM select
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

    for (int i = 0; i < len; ++i)
    {
        drawUserChar(buf[i]);
    }

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
    temp &= ~(1 << 7); //Select graphics mode - clear bit 7
    m_tft.writeReg(0x40, temp);
	
    m_tft.writeReg(0x23,address);

    uint8_t reg0x21 = m_tft.readReg(0x21);
    temp = reg0x21;
    temp &= ~(1 << 7); //clear bit 7
    m_tft.writeReg(0x21,temp);

	temp = reg0x41;
    temp &= ~(1 << 3); //clear bit 3
    temp |= (1 << 2); //bit set 2

    m_tft.writeReg(0x41,temp);
    
    m_tft.writeCommand(0x02);
	for (uint8_t i=0; i<16; ++i){
		m_tft.writeData(symbol[i]);
	}

    m_tft.writeReg(0x21, reg0x21); //restore register
    m_tft.writeReg(0x40, reg0x40); //restore register
    m_tft.writeReg(0x41, reg0x41); //restore register
}

void DashRA8875GfxWrapper::drawUserChar(uint8_t symbolAddrs) 
{
    m_tft.textMode();

    uint8_t oldRegState = m_tft.readReg(0x21);
	uint8_t temp = oldRegState;
	bitSet(temp,7);//set to CGRAM
	m_tft.writeReg(0x21,temp);

    temp = m_tft.readReg(0x41);
    temp &= ~((1<<3) | (1<<2));// Clear bits 3 and 2
    temp &= ~(1 << 0); //clear bit 0
    m_tft.writeReg(0x41, temp);

	m_tft.writeCommand(RA8875_MRWC);
	m_tft.writeData(symbolAddrs);

	m_tft.writeReg(0x21,oldRegState);
}

void DashRA8875GfxWrapper::loadFont(const unsigned char *font, int width, int height, int first_char, int char_count)
{
    int chars_per_row = width / 8;

    for (int char_idx = 0; char_idx < char_count; ++char_idx)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "Loading font: char %d", first_char+char_idx);
        textWrite(0, 32, 1, colorScheme().textColor, colorScheme().textBackground, buf);

        unsigned char char_data[16];
        int char_row = char_idx / chars_per_row;
        int char_col = char_idx % chars_per_row;

        for (int row = 0; row < 16; ++row)
        {
            // Each row in the bitmap is width/8 bytes
            const unsigned char* row_ptr = font + (char_row * 16 + row) * (width / 8);
            char_data[row] = pgm_read_byte_near(row_ptr + char_col);
        }

        loadUserChar(char_data, first_char + char_idx);
    }
    m_tft.writeReg(0x21, 0x80);     // Reg 0x21 bit 7 - CGRAM select
    m_tft.writeReg(0x41, 0x00);     // Switch back to display memory
}
