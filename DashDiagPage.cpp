#include "DashDiagPage.hpp"

PageDefinition *diagPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons,
                                 PageState state)
{
    // Active window position
    static constexpr int pos_x = 128;
    static constexpr int pos_y = 64;
    static const int w = gfx.width() - pos_x * 2;
    static const int h = gfx.height() - pos_y * 2;
}