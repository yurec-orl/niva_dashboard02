#include "DashMainPage.hpp"

namespace {
    constexpr uint8_t view_count = 5;
    constexpr int btn_view_next = 0;
    constexpr int btn_view_prev = 1;
    constexpr int btn_brightness_incr = 4;
    constexpr int btn_brightness_decr = 5;
}

PageDefinition *mainPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons, PageState state)
{
    static uint8_t view_idx = 0;

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
