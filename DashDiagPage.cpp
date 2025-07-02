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
    static const char *fmt[] = {
        "����      %4d",
        "�������   %4d",
        "���� ��� %4d",
        "���浪�   %4d",
        "��� ⮯�  %4d",
        "������   %4d",
        "�������  %4d",
        "���� ��� %4d",
        "�ᢥ饭�� %4d",
        "��� ����� %4d",
        "���� ᢥ� %4d",
        "���� ���� %4d",
        nullptr};

    if (state == PageState::on_switch)
    {
        gfx.userTextWrite(title_pos_x, title_pos_y, 2, RA8875_WHITE, RA8875_BLACK, "�������");
    }
    else
    {
        int x = pos_x;
        int y = pos_y;

        for (int i = 0; fmt[i] != nullptr; ++i)
        {
            snprintf(buf, sizeof(buf), fmt[i], DashSensorsProvider::instance().sensors()[i]->read());
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