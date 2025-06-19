#pragma once

#include <Arduino.h>

#include "DashPageMgr.hpp"

PageDefinition *mainPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons, PageState state);