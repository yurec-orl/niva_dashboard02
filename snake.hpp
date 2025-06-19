#include <Arduino.h>
#include <Arduino_AVRSTL.h>

#include "DashGfxWrapper.hpp"
#include "DashPageMgr.hpp"
#include "PushButton.hpp"

PageDefinition *snake(IDashGfxWrapper &gfx,
                      const std::vector<PushButton> &buttons,
                      PageState state);