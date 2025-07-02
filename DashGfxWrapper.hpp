#pragma once

// #include <RA8875.h>

#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>
#include <Arduino.h>

struct GfxColorScheme
{
	int textColor = RA8875_YELLOW;
	int textBackground = RA8875_BLACK;
	int statusTextColor = RA8875_CYAN;
	int statusTextBackground = RA8875_BLACK;
	int buttonLabelsColor = RA8875_YELLOW;
	int buttonLabelsBackground = RA8875_BLACK;
	int buttonPressedLabelsColor = RA8875_WHITE;
	int buttonPressedLabelsBackground = RA8875_BLUE;
};

class IDashGfxWrapper
{
public:
	virtual bool setup() = 0;

	virtual int width() = 0;
	virtual int height() = 0;

	virtual void fillScreen(unsigned int color) = 0;
	virtual void drawLine(int x0, int y0, int x1, int y1, unsigned int color) = 0;
	virtual void drawRect(int x, int y, int w, int h, unsigned int color) = 0;
	virtual void fillRect(int x, int y, int w, int h, unsigned int color) = 0;
	virtual void drawCircle(int x, int y, int r, unsigned int color) = 0;
	virtual void fillCircle(int x, int y, int r, unsigned int color) = 0;
	virtual void drawRoundRect(int x, int y, int w, int h, int r,
							   unsigned int color) = 0;
	virtual void fillRoundRect(int x, int y, int w, int h, int r,
							   unsigned int color) = 0;

	virtual void drawPixels(uint16_t *p, uint32_t count, int16_t x, int16_t y) = 0;

	// Uses hardware ASCII font.
	virtual void textWrite(int x, int y, int scale, int color, int bkgnd,
						   const char *buf) = 0;

	// Uses custom CGRAM font.
	virtual void userTextWrite(int x, int y, int scale, int color, int bkgnd, const char *buf) = 0;

	// Uses software font. Very slow on RA8775.
	virtual void print(int x, int y, int color, int bkgnd,
					   const char *buf) = 0;

	virtual int chWidth(int) = 0;
	virtual int chHeight(int) = 0;

	virtual int textWidth(char *, int) = 0;

	static const GfxColorScheme &colorScheme() { return m_colorScheme; }

	virtual uint8_t brightness() = 0;
	virtual void setBrightness(uint8_t brightness) = 0;

	virtual void setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h,
								 uint8_t mode) = 0;
	virtual void scrollX(int16_t dist) = 0;
	virtual void scrollY(int16_t dist) = 0;

	virtual void setActiveWindow(int16_t x = 0, int16_t y = 0, int16_t w = 0, int16_t h = 0) = 0;

private:
	static const GfxColorScheme m_colorScheme;
};

// Library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
#define RA8875_INT 48
#define RA8875_CS 53
#define RA8875_RESET 49

class DashRA8875GfxWrapper : public IDashGfxWrapper
{
public:
	DashRA8875GfxWrapper() : m_tft(RA8875_CS, RA8875_RESET) {};

	bool setup();

	int width();
	int height();

	void fillScreen(unsigned int color);
	void drawLine(int x0, int y0, int x1, int y1, unsigned int color);
	void drawRect(int x, int y, int w, int h, unsigned int color);
	void fillRect(int x, int y, int w, int h, unsigned int color);
	void drawCircle(int x, int y, int r, unsigned int color);
	void fillCircle(int x, int y, int r, unsigned int color);
	void drawRoundRect(int x, int y, int w, int h, int r, unsigned int color);
	void fillRoundRect(int x, int y, int w, int h, int r, unsigned int color);

	void drawPixels(uint16_t *p, uint32_t count, int16_t x, int16_t y);

	void textWrite(int x, int y, int scale, int color, int bkgnd,
				   const char *buf);
	void userTextWrite(int x, int y, int scale, int color, int bkgnd, const char *buf);
	void print(int x, int y, int color, int bkgnd,
			   const char *buf);

	int chWidth(int scale) { return 8 * (scale + 1); }
	int chHeight(int scale) { return 16 * (scale + 1); }

	int textWidth(char *buf, int scale) { return strlen(buf) * chWidth(scale); }

	uint8_t brightness();
	void setBrightness(uint8_t brightness);

	void setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h,
						 uint8_t mode);
	void scrollX(int16_t dist);
	void scrollY(int16_t dist);

	// Works for RA8875_800x480 mode only.
	virtual void setActiveWindow(int16_t x = 0, int16_t y = 0, int16_t w = 0, int16_t h = 0);

private:
	Adafruit_RA8875 m_tft;
	uint8_t m_brightness = 16;

	void loadFont(const unsigned char *font, int width, int height, int first_char, int char_count);

	void loadUserChar(const uint8_t symbol[], uint8_t address);
	void drawUserChar(uint8_t symbolAddrs);
};
