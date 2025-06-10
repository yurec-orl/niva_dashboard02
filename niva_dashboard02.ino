#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

#include "DashGfxWrapper.hpp"
#include "DashPageMgr.hpp"

// IO pins

namespace {

  constexpr int pin_buttons[] = {25, 23, 27, 29, 24, 22, 26, 28};

}

// MFD pages definitions

bool testPageCallback(IDashGfxWrapper &gfx) {
  static constexpr char *msg = "TEST PAGE";
  gfx.textWrite(256, 120, 1, 65504, 0, msg);
  return true;
}

PageDefinition testPage = {testPageCallback, 
  {{"<1", nullptr}, {"<2", nullptr}, {"<3", nullptr}, {"<4", nullptr}, {"5>", nullptr}, {"6>", nullptr}, {"7>", nullptr}, {"8>", nullptr}}};

PageDefinition *pageDef[] = {&testPage, nullptr};

DashRA8875GfxWrapper dashRA8875GfxWrapper;
DashPageMgr pageMgr(dashRA8875GfxWrapper, pin_buttons, pageDef);

void setup() {
  // Initialize display
  if (!dashRA8875GfxWrapper.setup()) {
    // nothing to do if error
    while (1) delay(1000);
  }

  // With hardware accelleration this is instant
  dashRA8875GfxWrapper.fillScreen(RA8875_WHITE);
  delay(250);
  dashRA8875GfxWrapper.fillScreen(RA8875_BLACK);

  constexpr char *msg = "INIT";
  constexpr int msg_len = strlen(msg);
  dashRA8875GfxWrapper.textWrite(256, 0, 1, 65504, 0, msg);
}

void loop() {
  pageMgr.loop();

  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  char buf[64];
  sprintf(buf, "Uptime: %ld\d %ld:%02ld:%02ld", days, hours, minutes, seconds);
  dashRA8875GfxWrapper.textWrite(450, 432, 0, 65504, 0, buf);
}
