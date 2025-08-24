#include "DashMainPage.hpp"
#include "DashSensorReader.hpp"
#include "DashIndicators.hpp"
#include <algorithm>

namespace {
    constexpr uint8_t view_count = 3;
    constexpr uint8_t ind_count = 8;
    constexpr int btn_view_next = 0;
    constexpr int btn_view_prev = 1;
    constexpr int btn_brightness_incr = 4;
    constexpr int btn_brightness_decr = 5;    constexpr int left_margin = 128;
    constexpr int right_margin = 128;
    constexpr int bottom_margin = 32;
    constexpr int top_margin = 64;

    // Global static variables for layouts and indicators
    static std::array<std::array<IDashIndicator *, ind_count>, view_count> view_arr;
    static std::array<std::array<DashIndicatorLayout *, ind_count>, view_count> layout_arr;
    
    // Static layout objects
    static DashIndicatorLayout* view1_speedLayout = nullptr;
    static DashIndicatorLayout* view1_tachoLayout = nullptr;
    static DashIndicatorLayout* view2_fuelLayout = nullptr;
    static DashIndicatorLayout* view2_oilLayout = nullptr;
    static DashIndicatorLayout* view2_tempLayout = nullptr;
    static DashIndicatorLayout* view3_speedLayout = nullptr;
    static DashIndicatorLayout* view3_tachoLayout = nullptr;
    static DashIndicatorLayout* view3_fuelLayout = nullptr;
    static DashIndicatorLayout* view3_oilLayout = nullptr;
    static DashIndicatorLayout* view3_tempLayout = nullptr;
    
    // Static indicator objects
    static DashSegmentIndicator* s_spdInd = nullptr;
    static DashHBarIndicator* s_tachoInd = nullptr;
    static DashVBarIndicator* s_fuelInd = nullptr;
    static DashVBarIndicator* s_oilInd = nullptr;
    static DashVBarIndicator* s_tempInd = nullptr;
    
    void initializeLayouts(IDashGfxWrapper &gfx, IDashSensorReader &rdr)
    {
        // Screen dimensions with margins
        int screen_width = gfx.width();
        int screen_height = gfx.height();
        int available_width = screen_width - left_margin - right_margin;
        int available_height = screen_height - bottom_margin - top_margin;
        
        // View 1: Speed on top, tacho on bottom
        // Speed indicator: equal width and height (square)
        //int view1_speed_size = std::min(available_width, available_height / 2);
        int view1_speed_w = available_width / 2;
        int view1_speed_h = available_height / 2;
        int view1_speed_x = left_margin + (available_width - view1_speed_w) / 2;
        int view1_speed_y = top_margin;
        
        // Tacho indicator: limited height of 96 pixels
        int view1_tacho_h = std::min(96, available_height / 2);
        int view1_tacho_x = left_margin;
        int view1_tacho_y = top_margin + available_height / 2 + (available_height / 2 - view1_tacho_h) / 2;
        int view1_tacho_w = available_width;
        
        // View 2: Three indicators in a row (fuel, oil, temp)
        // Vertical bar indicators: limited width of 64 pixels
        int view2_indicator_w = 64;
        int view2_indicator_h = available_height;
        int view2_total_width = 3 * view2_indicator_w;
        int view2_spacing = (available_width - view2_total_width) / 4; // 4 spaces: before, between, between, after
        int view2_fuel_x = left_margin + view2_spacing;
        int view2_oil_x = view2_fuel_x + view2_indicator_w + view2_spacing;
        int view2_temp_x = view2_oil_x + view2_indicator_w + view2_spacing;
        int view2_indicator_y = top_margin;

        // View 3: Speed on top, tacho in middle, three indicators at bottom
        // Speed indicator: equal width and height (square)
        int view3_speed_size = std::min(available_width, available_height / 3);
        int view3_speed_x = left_margin + (available_width - view3_speed_size) / 2;
        int view3_speed_y = top_margin;
        int view3_speed_w = view3_speed_size;
        int view3_speed_h = view3_speed_size;
        
        // Tacho indicator: limited height of 96 pixels
        int view3_tacho_h = std::min(96, available_height / 3);
        int view3_tacho_x = left_margin;
        int view3_tacho_y = top_margin + available_height / 3 + (available_height / 3 - view3_tacho_h) / 2;
        int view3_tacho_w = available_width;
          // Bottom indicators: limited width of 64 pixels
        int view3_bottom_indicator_w = 64;
        int view3_bottom_indicator_h = available_height / 3;
        int view3_total_width = 3 * view3_bottom_indicator_w;
        int view3_spacing = (available_width - view3_total_width) / 4;
        int view3_fuel_x = left_margin + view3_spacing;
        int view3_oil_x = view3_fuel_x + view3_bottom_indicator_w + view3_spacing;
        int view3_temp_x = view3_oil_x + view3_bottom_indicator_w + view3_spacing;
        int view3_bottom_indicator_y = top_margin + 2 * available_height / 3;
        
        // Create layout objects for View 1
        // Speed indicator colors: bright yellow, black background, dim yellow accent
        std::vector<int> speedColors = {RA8875_YELLOW, RA8875_BLACK, 0x72C0 /*0x8B60*/};
        view1_speedLayout = new DashIndicatorLayout(view1_speed_x, view1_speed_y, view1_speed_w, view1_speed_h, speedColors);
        view1_tachoLayout = new DashIndicatorLayout(view1_tacho_x, view1_tacho_y, view1_tacho_w, view1_tacho_h, speedColors);
        
        // Create layout objects for View 2
        view2_fuelLayout = new DashIndicatorLayout(view2_fuel_x, view2_indicator_y, view2_indicator_w, view2_indicator_h, speedColors);
        view2_oilLayout = new DashIndicatorLayout(view2_oil_x, view2_indicator_y, view2_indicator_w, view2_indicator_h, speedColors);
        view2_tempLayout = new DashIndicatorLayout(view2_temp_x, view2_indicator_y, view2_indicator_w, view2_indicator_h, speedColors);
        
        // Create layout objects for View 3
        view3_speedLayout = new DashIndicatorLayout(view3_speed_x, view3_speed_y, view3_speed_w, view3_speed_h, speedColors);
        view3_tachoLayout = new DashIndicatorLayout(view3_tacho_x, view3_tacho_y, view3_tacho_w, view3_tacho_h, speedColors);
        view3_fuelLayout = new DashIndicatorLayout(view3_fuel_x, view3_bottom_indicator_y, view3_bottom_indicator_w, view3_bottom_indicator_h, speedColors);
        view3_oilLayout = new DashIndicatorLayout(view3_oil_x, view3_bottom_indicator_y, view3_bottom_indicator_w, view3_bottom_indicator_h, speedColors);
        view3_tempLayout = new DashIndicatorLayout(view3_temp_x, view3_bottom_indicator_y, view3_bottom_indicator_w, view3_bottom_indicator_h, speedColors);

        // Create indicators without layout parameters
        s_spdInd = new DashSegmentIndicator(gfx, rdr);
        s_tachoInd = new DashHBarIndicator(gfx, rdr, nullptr, 0, 0);
        s_fuelInd = new DashVBarIndicator(gfx, rdr, "íéèã", 2, 4);
        s_oilInd = new DashVBarIndicator(gfx, rdr, "åÄëãé", 2, 4);
        s_tempInd = new DashVBarIndicator(gfx, rdr, "íÖåè", 2, 4);

        // Initialize view arrays
        view_arr = {
            // View 1: Speed on top, tacho on bottom
            {s_spdInd, s_tachoInd, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
            // View 2: Fuel, oil, temp in a row
            {s_fuelInd, s_oilInd, s_tempInd, nullptr, nullptr, nullptr, nullptr, nullptr},
            // View 3: Speed on top, tacho in middle, fuel/oil/temp at bottom
            {s_spdInd, s_tachoInd, s_fuelInd, s_oilInd, s_tempInd, nullptr, nullptr, nullptr}
        };

        // Initialize layout arrays
        layout_arr = {
            // View 1: Speed on top, tacho on bottom
            {view1_speedLayout, view1_tachoLayout, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
            // View 2: Fuel, oil, temp in a row
            {view2_fuelLayout, view2_oilLayout, view2_tempLayout, nullptr, nullptr, nullptr, nullptr, nullptr},
            // View 3: Speed on top, tacho in middle, fuel/oil/temp at bottom
            {view3_speedLayout, view3_tachoLayout, view3_fuelLayout, view3_oilLayout, view3_tempLayout, nullptr, nullptr, nullptr}
        };
    }
}

PageDefinition *mainPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons, PageState state)
{
    static uint8_t view_idx = 0;
    static uint8_t old_view_idx = 1;    // To ensure initial redraw

    static DashAnalogTestReader rdr(2000, 0, 1023);

    // Initialize layouts and indicators if not already done
    static bool layouts_initialized = false;
    if (!layouts_initialized) {
        initializeLayouts(gfx, rdr);
        layouts_initialized = true;
        rdr.reset();
    }
    
    // When state is on_switch (full page redraw), pass true and draw full indicator.
    for (int i = 0; i < ind_count; ++i)
    {
        if (view_arr[view_idx][i] != nullptr && layout_arr[view_idx][i] != nullptr)
            view_arr[view_idx][i]->draw(*layout_arr[view_idx][i],
                (state == PageState::on_switch)||(old_view_idx != view_idx));
    }

    old_view_idx = view_idx;
    if (buttons[btn_brightness_incr].state() == PushButtonState::down && gfx.brightness() < 255)
    {
        gfx.setBrightness(gfx.brightness()+1);
    } else if (buttons[btn_brightness_decr].state() == PushButtonState::down && gfx.brightness() > 0)
    {
        gfx.setBrightness(gfx.brightness()-1);    
    } else if (buttons[btn_view_next].state() == PushButtonState::pressed && view_idx < view_count - 1)
    {
        view_idx++;
    } else if (buttons[btn_view_prev].state() == PushButtonState::pressed && view_idx > 0)
    {
        view_idx--;
    }

    if (old_view_idx != view_idx)
    {
        gfx.fillRect(left_margin, top_margin,
            gfx.width()-(right_margin+left_margin), gfx.height()-(bottom_margin+top_margin), RA8875_BLACK);
        rdr.reset();
    }

    return nullptr;
}
