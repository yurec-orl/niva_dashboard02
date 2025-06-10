#include "DashGfxWrapper.hpp"

#include <Arduino.h>

bool DashRA8875GfxWrapper::setup()
{
    /* Initialize the display using 'RA8875_480x80', 'RA8875_480x128', 'RA8875_480x272' or 'RA8875_800x480' */
    if (!m_tft.begin(RA8875_800x480)) {
        return false;
    }

    // Reverse scan direction (register 0x20, bit 2 and 3) for both X and Y axis to rotate image 180 degrees (related to physical composition of the device).
    m_tft.writeReg(0x20, 0x0C); //reverse scan direction for X

    m_tft.displayOn(true);
    m_tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    m_tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    m_tft.PWM1out(16);

    m_tft.fillScreen(RA8875_BLACK);

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

void DashRA8875GfxWrapper::textWrite(int x, int y, int scale, int color, int bkgnd, const char * buf)
{
  const int len = strlen(buf);
  
  m_tft.textMode();
  m_tft.textSetCursor(x, y);
  m_tft.textEnlarge(scale);
  m_tft.textColor(color, bkgnd);
  m_tft.textWrite(buf, len);
  m_tft.graphicsMode();
}
