#include <Arduino_AVRSTL.h>
#include <SPI.h>

#include "DashGfxWrapper.hpp"
#include "DashPageMgr.hpp"
#include "PushButton.hpp"
#include "snake.hpp"

// IO pins

namespace
{

  constexpr int pin_buttons[] = {28, 26, 22, 24, 29, 27, 23, 25};

}

////////////////////////////////////////////////////////////////////////////////
// Page callbacks
////////////////////////////////////////////////////////////////////////////////

bool testPageCallback(IDashGfxWrapper &gfx,
                      const std::vector<PushButton> &buttons, PageState state)
{
  if (state == PageState::on_switch)
  {
    constexpr int scale = 2;
    static constexpr char *msg = "ëíêÄçàñÄ 1";
    gfx.userTextWrite(256, 120, scale, gfx.colorScheme().textColor, gfx.colorScheme().textBackground, msg);
    gfx.drawRect(256, 120, gfx.textWidth(msg, scale), gfx.chHeight(scale), gfx.colorScheme().statusTextColor);
  }
  return true;
}

bool testPage2Callback(IDashGfxWrapper &gfx,
                       const std::vector<PushButton> &buttons,
                       PageState state)
{
  static unsigned long timer = 0;

  if (state == PageState::on_switch)
  {
    constexpr int scale = 2;
    static constexpr char *msg = "ëíêÄçàñÄ 2";
    gfx.userTextWrite(256, 120, scale, gfx.colorScheme().textColor, gfx.colorScheme().textBackground, msg);
    timer = millis();
  }

  return (millis() - timer) < 3000;
}

bool hiddenCallback(IDashGfxWrapper &gfx,
                    const std::vector<PushButton> &buttons,
                    PageState state)
{
  return snake(gfx, buttons, state);
}

////////////////////////////////////////////////////////////////////////////////
// MFD pages definitions
////////////////////////////////////////////////////////////////////////////////

PageDefinition testPage;
PageDefinition testPage2;
PageDefinition hidden;

PageDefinition *pageDef[] = {&testPage, &testPage2, &hidden, nullptr};

////////////////////////////////////////////////////////////////////////////////
// Global objects
////////////////////////////////////////////////////////////////////////////////

DashRA8875GfxWrapper dashRA8875GfxWrapper;
DashPageMgr pageMgr(dashRA8875GfxWrapper, pin_buttons, pageDef);

////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  Serial.println("Startup");

  // Initialize display
  if (!dashRA8875GfxWrapper.setup())
  {
    // nothing to do if error
    while (1)
      delay(1000);
  }

  testPage = {testPageCallback,
              {{"ëíê 2", &testPage2},
               {"2", nullptr},
               {"3", nullptr},
               {"4", nullptr},
               {"5", nullptr},
               {"6", nullptr},
               {">", &hidden},
               {"8", nullptr}}};
  testPage2 = {testPage2Callback,
               {{"1", nullptr},
                {"", nullptr},
                {"", nullptr},
                {"4", nullptr},
                {"5", nullptr},
                {"", nullptr},
                {"", nullptr},
                {"ÇéáÇ", &testPage}}};
  hidden = {hiddenCallback,
            {{"/\\", nullptr},
             {"\\/", nullptr},
             {"", nullptr},
             {"", nullptr},
             {"<", nullptr},
             {">", nullptr},
             {"", nullptr},
             {"ÇéáÇ", &testPage}}};

  pageMgr.setPage(0);
}

void drawUptime(DashRA8875GfxWrapper &gfx, int scale)
{
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
  sprintf(buf, "ê†°Æ‚†: %ld\d %ld:%02ld:%02ld", days, hours, minutes, seconds);
  gfx.userTextWrite(gfx.width() - gfx.textWidth(buf, scale), gfx.height() - gfx.chHeight(scale), scale, gfx.colorScheme().statusTextColor, gfx.colorScheme().statusTextBackground, buf);
}

void loop()
{
  pageMgr.loop();

  drawUptime(dashRA8875GfxWrapper, 0);
}
