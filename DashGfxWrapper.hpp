#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_RA8875.h>

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

  virtual void textWrite(int x, int y, int scale, int color, int bkgnd, const char * buf) = 0;
private:
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
	void drawRoundRect(int x, int y, int w, int h, int r,
		unsigned int color);
	void fillRoundRect(int x, int y, int w, int h, int r,
		unsigned int color);
  void textWrite(int x, int y, int scale, int color, int bkgnd, const char * buf);

private:
	Adafruit_RA8875 m_tft;
	//IDashLogger& m_dashLogger;
};
