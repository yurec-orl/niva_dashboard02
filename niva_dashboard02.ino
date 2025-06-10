#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

#include "DashGfxWrapper.hpp"
#include "DashPageMgr.hpp"

// IO pins

namespace {

  constexpr int pin_buttons[] = {28, 26, 22, 24, 29, 27, 23, 25};

}

// MFD pages definitions

bool testPageCallback(IDashGfxWrapper &gfx, const int (&button_state)[DashPageMgr::buttonCountInCol()], PageState state = PageState::on_process) 
{
  if (state == PageState::on_switch)
  {
    static constexpr char *msg = "TEST PAGE";
    gfx.textWrite(256, 120, 1, 65504, 0, msg);
  }
  return true;
}

bool testPage2Callback(IDashGfxWrapper &gfx, const int (&button_state)[DashPageMgr::buttonCountInCol()], PageState state = PageState::on_process) 
{
  static unsigned long timer = 0;
  
  if (state == PageState::on_switch)
  {
    static constexpr char *msg = "TEST PAGE 2";
    gfx.textWrite(256, 120, 1, 65504, 0, msg);
    timer = millis();
  } else if (button_state[7] == LOW) {
    return false;
  }
  
  return (millis() - timer) < 3000;
}

PageDefinition testPage2 = {testPage2Callback, 
  {{"<1", nullptr}, {"", nullptr}, {"", nullptr}, {"<4", nullptr}, {"5>", nullptr}, {"", nullptr}, {"", nullptr}, {"BACK>", nullptr}}};

PageDefinition testPage = {testPageCallback, 
  {{"<PAGE 2", &testPage2}, {"<2", nullptr}, {"<3", nullptr}, {"<4", nullptr}, {"5>", nullptr}, {"6>", nullptr}, {"7>", nullptr}, {"8>", nullptr}}};

PageDefinition *pageDef[] = {&testPage, &testPage2, nullptr};

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

  pageMgr.setPage(0);
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
  dashRA8875GfxWrapper.textWrite(650, 460, 0, 65504, 0, buf);
}
