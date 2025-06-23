#include <Arduino_AVRSTL.h>
#include <SPI.h>

#include "DashGfxWrapper.hpp"
#include "DashPageMgr.hpp"
#include "PushButton.hpp"
#include "DashMainPage.hpp"
#include "snake.hpp"
#include "splash.hpp"
#include "oscilloscope.hpp"

#include "util.hpp"

// IO pins

namespace
{

  constexpr int pin_buttons[] = {28, 26, 22, 24, 29, 27, 23, 25};

}

////////////////////////////////////////////////////////////////////////////////
// MFD pages
// startupPage - initial page, switches to mainPage after delay
// mainPage - indicators (automatically displayed after startupPage)
////////////////////////////////////////////////////////////////////////////////

PageDefinition startupPage;
PageDefinition mainPage;
PageDefinition diagPage;
PageDefinition oscillPage;
PageDefinition snakePage;
PageDefinition gameOfLifePage;

PageDefinition *pageDef[] = {&startupPage, &mainPage, &diagPage, &oscillPage, &snakePage, &gameOfLifePage, nullptr};

////////////////////////////////////////////////////////////////////////////////
// Page callbacks
////////////////////////////////////////////////////////////////////////////////

PageDefinition *startupPageCallback(IDashGfxWrapper &gfx,
                                    const std::vector<PushButton> &buttons,
                                    PageState state)
{
  unsigned long timer;
  if (state == PageState::on_switch)
  {
    int pos_x = 0;
    int pos_y = 0;

    auto draw_func = [&pos_x, &pos_y, &gfx](const unsigned char *ptr, uint32_t size, int x, int y, int width, int height)
    {
      std::array<uint16_t, 32> pixels;
      int pixels_idx = 0;
      int pixel_buf_idx = 0;

      pos_x = x;
      pos_y = y;
      for (int i = 0; i < size; i += 4)
      {
        std::array<uint8_t, 4> bytes;
        *((uint32_t *)bytes.data()) = pgm_read_dword_near(ptr + i);

        for (int byte_idx = 0; byte_idx < bytes.size(); ++byte_idx)
        {
          for (int bit = 0; bit < 8; ++bit)
          {
            pixels[pixels_idx + 7 - bit] = (bytes[byte_idx] & (1 << bit)) ? RA8875_WHITE : RA8875_BLACK;
          }
          pixels_idx += 8;
        }
        if (pixels_idx >= pixels.size())
        {
          gfx.drawPixels(pixels.data(), pixels.size(), pos_x, pos_y);
          pixels_idx = 0;
          pos_x += pixels.size();
          if (pos_x >= x + width || pos_x >= gfx.width())
          {
            pos_x = x;
            pos_y++;
          }
        }
      }
    };

    draw_func(epd_bitmap_lada_logo, sizeof(epd_bitmap_lada_logo), 240, 176, 320, 129);
    timer = millis();
  }
  else if (millis() - timer > 2500)
  {
    return &mainPage;
  }
  return nullptr;
}

PageDefinition *snakeCallback(IDashGfxWrapper &gfx,
                              const std::vector<PushButton> &buttons,
                              PageState state)
{
  return snake(gfx, buttons, state);
}

PageDefinition *gameOfLifeCallback(IDashGfxWrapper &gfx,
                                   const std::vector<PushButton> &buttons,
                                   PageState state)
{
  static constexpr int cell_w = 8;
  static constexpr int cell_h = 8;

  static constexpr int w = 50;
  static constexpr int h = 50;

  static const int pos_x = (gfx.width() - w * cell_w) / 2;
  static const int pos_y = (gfx.height() - h * cell_h) / 2;

  static char curGen[w][h];
  static char newGen[w][h];

  if (state == PageState::on_switch)
  {
    Serial.println("Game of life");
    memset(curGen, 0, sizeof(curGen));

    randomSeed(millis());
    for (int i = 0; i < 1000; ++i)
    {
      int x = random(0, w);
      int y = random(0, h);
      curGen[x][y] = 1;
    }
    // curGen[25][25] = 1;
    // curGen[26][26] = 1;
    // curGen[27][24] = 1;
    // curGen[27][25] = 1;
    // curGen[27][26] = 1;
  }

  memset(newGen, 0, sizeof(newGen));
  for (int x = 1; x < w - 1; ++x)
  {
    for (int y = 1; y < h - 1; ++y)
    {
      int cell_color = curGen[x][y] ? RA8875_WHITE : RA8875_BLACK;
      gfx.fillRect(pos_x + x * cell_w, pos_y + y * cell_h, cell_w, cell_h, cell_color);

      int count = 0;
      for (int x1 = x - 1; x1 <= x + 1; ++x1)
      {
        for (int y1 = y - 1; y1 <= y + 1; ++y1)
        {
          if (x1 == x && y1 == y)
            continue;
          if (curGen[x1][y1])
          {
            count++;
          }
        }
      }

      if (curGen[x][y])
      {
        if (count >= 2 && count <= 3)
        {
          // Serial.print("x="); Serial.print(x); Serial.print("y="); Serial.println(y);
          newGen[x][y] = 1;
        }
      }
      else if (count == 3)
      {
        // Serial.print("x="); Serial.print(x); Serial.print("y="); Serial.println(y);
        newGen[x][y] = 1;
      }
    }
  }
  memcpy(curGen, newGen, sizeof(curGen));
  Serial.println("------");

  return nullptr;
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

  startupPage = {&startupPageCallback, {}};
  mainPage = {mainPageCallback,
              {{"ÇàÑ+", nullptr},
               {"ÇàÑ-", nullptr},
               {".", &gameOfLifePage},
               {".", &snakePage},
               {"üêä+", nullptr},
               {"üêä-", nullptr},
               {"", nullptr},
               {"ÑàÄÉ", &diagPage}}};
  diagPage = {nullptr,
              {{"ÑÄíó", nullptr},
               {"ùÅì", nullptr},
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
  gameOfLifePage = {gameOfLifeCallback,
                    {{"", nullptr},
                     {"", nullptr},
                     {"", nullptr},
                     {"", nullptr},
                     {"", nullptr},
                     {"", nullptr},
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
