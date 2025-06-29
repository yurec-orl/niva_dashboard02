#pragma once

#include <Arduino.h>

#include "DashPageMgr.hpp"

PageDefinition *diagPageCallback(IDashGfxWrapper &gfx,
                                 const std::vector<PushButton> &buttons,
                                 PageState state);