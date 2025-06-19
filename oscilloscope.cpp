#include <Arduino.h>

#include "oscilloscope.hpp"

PageDefinition *oscillPageCallback(IDashGfxWrapper &gfx,
                                   const std::vector<PushButton> &buttons, PageState state)
{
    // Graph viewport coords.
    constexpr int wp_x = 128;
    constexpr int wp_y = 32;
    const int wp_w = gfx.width() - wp_x * 2;
    const int wp_h = gfx.height() - wp_y * 2;

    constexpr int btn_idx = DashPageMgr::buttonCount() - 1;

    // Set up hardware scroll window.
    gfx.setScrollWindow(wp_x, wp_y, wp_x + wp_w, wp_y + wp_h, RA8875_SCROLL_BOTH);

    // Draw axis lines.
    gfx.drawLine(wp_x-1, wp_y, wp_x-1, wp_y + wp_h, RA8875_WHITE);
    gfx.drawLine(wp_x, wp_y + wp_h + 1, wp_x + wp_w, wp_y + wp_h + 1, RA8875_WHITE);

    // Copy buttons objects to be able to update them here without involving page manager.
    std::vector<PushButton> buttons_tmp = buttons;

    // To reduce delays, oscilloscope mode goes into it's own loop
    // and does not return to page manager until exited.
    int x = wp_x + wp_w;
    int old_y = wp_y + wp_h;
    int scroll_x = 0;
    gfx.drawLine(wp_x + wp_w/2, wp_y + wp_h/2, wp_x + wp_w/2 + 32, wp_y + wp_h/2 + 32, RA8875_WHITE);
    for (;;)
    {
        //int val = analogRead(A3);
        //int y = wp_y + wp_h - (val * wp_h / 1024);

        //gfx.drawLine(x, old_y, x-1, y, RA8875_WHITE);
        //old_y = y;
        gfx.scrollX(scroll_x);
        scroll_x += 1;

        buttons_tmp[btn_idx].read();
        if (buttons_tmp[btn_idx].state() == PushButtonState::pressed)
        {
            return DashPageMgr::prevPage();
        }
    }
    return nullptr;
}