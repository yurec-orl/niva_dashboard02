#include <Arduino.h>

#include "oscilloscope.hpp"

void floatToBytes(uint8_t bytes[2], float flt)
{
    bytes[0] = (uint8_t)flt;      // truncate whole numbers
    flt = (flt - bytes[0]) * 100; // remove whole part of flt and shift 2 places over
    bytes[1] = (uint8_t)flt;      // truncate the fractional part from the new "whole" part
}

PageDefinition *oscillPageCallback(IDashGfxWrapper &gfx,
                                   const std::vector<PushButton> &buttons, PageState state)
{
    // Graph viewport coords.
    constexpr int wp_x = 128;
    constexpr int wp_y = 64;
    const int wp_w = gfx.width() - wp_x * 2;
    const int wp_h = gfx.height() - wp_y * 2;

    constexpr float r1 = 47000.0;
    constexpr float r2 = 20000.0;
    constexpr float div_coeff = r2 / (r1 + r2);

    constexpr int btn_idx = DashPageMgr::buttonCount() - 1;

    // Set up hardware scroll window.
    // gfx.setScrollWindow(wp_x, wp_y, wp_x + wp_w, wp_y + wp_h, RA8875_SCROLL_BOTH);

    // Draw axis lines.
    gfx.drawLine(wp_x - 1, wp_y, wp_x - 1, wp_y + wp_h, RA8875_WHITE);
    gfx.drawLine(wp_x, wp_y + wp_h + 1, wp_x + wp_w, wp_y + wp_h + 1, RA8875_WHITE);

    // Copy buttons objects to be able to update them here without involving page manager.
    std::vector<PushButton> buttons_tmp = buttons;

    // To reduce delays, oscilloscope mode goes into it's own loop
    // and does not return to page manager until exited.
    int x = wp_x;
    int old_y = wp_y + wp_h;

    int data_buf[wp_w];
    char buf[64];
    uint8_t int_conv_buf[2];

    unsigned long timer;
    unsigned long avg_time;
    unsigned long avg = 0;
    int max = 0;
    int min = 1024;

    bool pause = false;

    for (;;)
    {
        if (!pause)
        {
            timer = millis();

            for (int i = 0; i < wp_w; ++i)
            {
                data_buf[i] = analogRead(A3);
            }
            avg_time = ((unsigned long)(millis() - timer)) * 1000 / wp_w;

            avg = 0;
            max = 0;
            min = 1024;
            for (int i = 0; i < wp_w; ++i)
            {
                avg += data_buf[i];
                if (data_buf[i] > max)
                {
                    max = data_buf[i];
                }
                if (data_buf[i] < min)
                {
                    min = data_buf[i];
                }
            }

            avg /= wp_w;

            float u = (avg * 5.0 / 1024.0) / div_coeff;
            floatToBytes(int_conv_buf, u);
            int u1 = int_conv_buf[0];
            int u2 = int_conv_buf[1];

            float d_u = ((max - min) * 5.0 / 1024.0) / div_coeff;
            floatToBytes(int_conv_buf, d_u);

            snprintf(buf, sizeof(buf), "%4ld us  U=%2d.%02d V  dV=%d.%02d V    ", avg_time, u1, u2, int_conv_buf[0], int_conv_buf[1]);
            gfx.userTextWrite(150, 32, 0, RA8875_YELLOW, RA8875_BLACK, buf);

            int cy = ((float)wp_h / 2.0 - 32) / ((max - avg) * wp_h / 1024);
            int cy1 = ((float)wp_h / 2.0 - 32) / ((avg - min) * wp_h / 1024);
            cy = cy1 < cy ? cy1 : cy;

            int dy = ((float)avg * wp_h / 1024 * cy) - wp_h / 2;

            for (int i = 0; i < wp_w; ++i)
            {
                int y = wp_y + wp_h - ((float)data_buf[i] * wp_h / 1024 * cy) + dy;

                if ((i % 10) == 0)
                {
                    gfx.fillRect(wp_x + i, wp_y, 10, wp_h, RA8875_BLACK);
                }

                gfx.drawLine(wp_x + i, old_y, wp_x + i + 1, y, RA8875_WHITE);
                old_y = y;

                for (auto &button : buttons_tmp)
                {
                    button.read();
                }
            }
        } else {
            for (auto &button : buttons_tmp)
            {
                button.read();
            }
        }

        if (buttons_tmp[btn_idx].state() == PushButtonState::pressed)
        {
            return DashPageMgr::prevPage();
        }
        else if (buttons_tmp[0].state() == PushButtonState::pressed)
        {
            pause = !pause;
        }
        // for (;;) delay(1000);
    }
    return nullptr;
}