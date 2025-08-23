#include <Arduino_AVRSTL.h>
#include <SPI.h>

#include "DashGfxWrapper.hpp"
#include "DashPageMgr.hpp"
#include "PushButton.hpp"
#include "DashDiagPage.hpp"
#include "DashMainPage.hpp"
#include "snake.hpp"
#include "oscilloscope.hpp"

#include "util.hpp"

// IO pins

namespace
{

  constexpr int pin_buttons[] = {28, 26, 22, 24, 29, 27, 23, 25};

}

////////////////////////////////////////////////////////////////////////////////
// MFD pages
// mainPage - indicators (automatically displayed after startupPage)
////////////////////////////////////////////////////////////////////////////////

PageDefinition mainPage;
PageDefinition diagPage;
PageDefinition oscillPage;
PageDefinition snakePage;

PageDefinition *pageDef[] = {&mainPage, &diagPage, &oscillPage, &snakePage, nullptr};

////////////////////////////////////////////////////////////////////////////////
// Page callbacks
////////////////////////////////////////////////////////////////////////////////

PageDefinition *snakeCallback(IDashGfxWrapper &gfx,
                              const std::vector<PushButton> &buttons,
                              PageState state)
{
  return snake(gfx, buttons, state);
}

////////////////////////////////////////////////////////////////////////////////
// Global objects
////////////////////////////////////////////////////////////////////////////////

DashRA8875GfxWrapper dashRA8875GfxWrapper;
DashPageMgr pageMgr(dashRA8875GfxWrapper, pin_buttons, pageDef);

////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  Serial.println("Startup");

  // Initialize display
  if (!dashRA8875GfxWrapper.setup())
  {
    // nothing to do if error
    while (1)
      delay(1000);
  }

  //startupPage = {&startupPageCallback, {}};
  mainPage = {mainPageCallback,
              {{"ÇàÑ+", nullptr},
               {"ÇàÑ-", nullptr},
               {"", nullptr},
               {".", &snakePage},
               {"üêä+", nullptr},
               {"üêä-", nullptr},
               {"", nullptr},
               {"ÑàÄÉ", &diagPage}}};
  diagPage = {&diagPageCallback,
              {{"ùÅì", nullptr},
               {"", nullptr},
               {"", nullptr},
               {"", nullptr},
               {"éëñ", &oscillPage},
               {"", nullptr},
               {"", nullptr},
               {"ÇéáÇ", &mainPage}}};
  oscillPage = {oscillPageCallback,
                {{"ëíéè", nullptr},
                 {"", nullptr},
                 {"ìÇã+", nullptr},
                 {"ìÇã-", nullptr},
                 {"<", nullptr},
                 {">", nullptr},
                 {"", nullptr},
                 {"ÇéáÇ", &diagPage}}};
  snakePage = {snakeCallback,
               {{"/\\", nullptr},
                {"\\/", nullptr},
                {"", nullptr},
                {"", nullptr},
                {"<", nullptr},
                {">", nullptr},
                {"", nullptr},
                {"ÇéáÇ", &mainPage}}};

  delay(100);
  pageMgr.setPage(0);
  Serial.print("Free memory: ");
  Serial.println(freeMemory());
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
