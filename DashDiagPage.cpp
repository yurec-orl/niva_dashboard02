#include "DashDiagPage.hpp"
#include "DashSensorsProvider.hpp"

PageDefinition *diagPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons,
                                 PageState state)
{
    // Active window position
    constexpr int text_scale = 2;
    static const int h = gfx.height() - 2 * gfx.chHeight(text_scale);
    constexpr int title_pos_x = 100;
    constexpr int title_pos_y = 20;
    constexpr int pos_x = 100;
    constexpr int pos_y = 128;

    static char buf[64];
    static std::pair<const char *, int> fmt[] = {
        {"����      %4d", SensorID::temp},
        {"�������   %4d", SensorID::fuel},
        {"���� ��� %4d", SensorID::oil_press},
        {"���浪�   %4d", SensorID::charge},
        {"��� ⮯�  %4d", SensorID::fuel_low},
        {"������   %4d", SensorID::tacho},
        {"�������  %4d", SensorID::speed},
        {"���� ��� %4d", SensorID::oil_press_low},
        {"�ᢥ饭�� %4d", SensorID::lights_ind},
        {"��� ����� %4d", SensorID::blinkers},
        {"���� ᢥ� %4d", SensorID::high_beam},
        {"���� ���� %4d", SensorID::diff_lock},
        {nullptr, 0}};

    if (state == PageState::on_switch)
    {
        gfx.userTextWrite(title_pos_x, title_pos_y, 2, RA8875_WHITE, RA8875_BLACK, "�������");
    }
    else
    {
        int x = pos_x;
        int y = pos_y;

        for (int i = 0; fmt[i].first != nullptr; ++i)
        {
            snprintf(buf, sizeof(buf), fmt[i].first, DashSensorsProvider::instance().sensors()[fmt[i].second]->read());
            //snprintf(buf, sizeof(buf), fmt[i], 0);
            gfx.userTextWrite(x, y, 2, RA8875_WHITE, RA8875_BLACK, buf);
            y += gfx.chHeight(2);
            if (y > h)
            {
                y = pos_y;
                x = pos_x + 350;
            }
        }
    }

    return nullptr;
}