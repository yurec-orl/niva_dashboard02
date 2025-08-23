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
    constexpr int scale = 2;
    constexpr int col_offset = 350;
    const int val_offset = 10 * gfx.chWidth(scale);

    static char buf[64];

    static std::pair<const char *, int> fmt[] = {
        {"����", SensorID::temp},
        {"�������", SensorID::fuel},
        {"���� ���", SensorID::oil_press},
        {"���浪�", SensorID::charge},
        {"��� ⮯�", SensorID::fuel_low},
        {"������", SensorID::tacho},
        {"�������", SensorID::speed},
        {"���� ���", SensorID::oil_press_low},
        {"�ᢥ饭��", SensorID::lights_ind},
        {"��� �����", SensorID::blinkers},
        {"���� ᢥ�", SensorID::high_beam},
        {"���� ����", SensorID::diff_lock},
        {nullptr, 0}};

    if (state == PageState::on_switch)
    {
        gfx.userTextWrite(title_pos_x, title_pos_y, 2, RA8875_WHITE, RA8875_BLACK, "�������");
        for (int i = 0; fmt[i].first != nullptr; ++i)
        {
            DashSensorsProvider::instance().sensors()[fmt[i].second]->reset();
        }

        int x = pos_x;
        int y = pos_y;

        for (int i = 0; fmt[i].first != nullptr; ++i)
        {
            gfx.userTextWrite(x, y, scale, RA8875_WHITE, RA8875_BLACK, fmt[i].first);
            y += gfx.chHeight(scale);
            if (y > h)
            {
                y = pos_y;
                x = pos_x + col_offset;
            }
        }
    }
    else
    {
        int x = pos_x;
        int y = pos_y;

        for (int i = 0; fmt[i].first != nullptr; ++i)
        {
            snprintf(buf, sizeof(buf), "%4d", DashSensorsProvider::instance().sensors()[fmt[i].second]->read());
            gfx.userTextWrite(x + val_offset, y, scale, RA8875_WHITE, RA8875_BLACK, buf);
            y += gfx.chHeight(scale);
            if (y > h)
            {
                y = pos_y;
                x = pos_x + col_offset;
            }
        }
    }

    return nullptr;
}