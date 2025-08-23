#include "DashMainPage.hpp"
#include "DashSensorReader.hpp"
#include "DashIndicators.hpp"

namespace {
    constexpr uint8_t view_count = 5;
    constexpr int btn_view_next = 0;
    constexpr int btn_view_prev = 1;
    constexpr int btn_brightness_incr = 4;
    constexpr int btn_brightness_decr = 5;

    constexpr int ind_count = 1;
}

PageDefinition *mainPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons, PageState state)
{
    static uint8_t view_idx = 0;

    static IDashIndicator * indicators[ind_count] = {0};
    static IDashSensorReader *rdr;
    static bool init = false;

    if (!init)
    {
        rdr = new DashAnalogSensorReader(A0);
        indicators[0] = new DashVBarIndicator(gfx, *rdr, 128, 128, 50, 256, RA8875_WHITE, RA8875_BLACK, "test", 2, 4);
        init = true;
    }

    // When state is on_switch (full page redraw), pass true and draw full indicator.
    indicators[0]->draw(state == PageState::on_switch);
    
    if (buttons[btn_brightness_incr].state() == PushButtonState::down && gfx.brightness() < 255)
    {
        gfx.setBrightness(gfx.brightness()+1);
    } else if (buttons[btn_brightness_decr].state() == PushButtonState::down && gfx.brightness() > 0)
    {
        gfx.setBrightness(gfx.brightness()-1);
    } else if (buttons[btn_view_next].state() == PushButtonState::pressed && view_idx < view_count)
    {
        view_idx++;
    } else if (buttons[btn_view_prev].state() == PushButtonState::pressed && view_idx > 0)
    {
        view_idx--;
    }
    return nullptr;
}
