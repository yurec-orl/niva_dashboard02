#pragma once

#include <Arduino.h>

#include "DashGfxWrapper.hpp"

namespace {
  constexpr int btn_count = 8;                  // number of physical buttons
  constexpr unsigned long debounce_delay = 100;
}

struct PageDefinition;

struct ButtonDefinition
{
  char *text;
  PageDefinition *next_page;
};

// Single page definition. Contains callback for page-specific processing,
// and buttons definition for this specific page. Button definition order
// corresponds to the physical order of the buttons, top to bottom and left to right.
struct PageDefinition
{
  bool (*callback)(IDashGfxWrapper &gfx);     // Switch to previous page if callback returns false.
  ButtonDefinition buttons[btn_count];
};

// Page manager handles switching MFD pages

class DashPageMgr
{
  public:
    DashPageMgr(IDashGfxWrapper &gfx, const int (&button_pins)[btn_count], PageDefinition **page_def);

    void setPage(int page_num);

    // Call from main loop() function to handle input and output
    void loop();

    int readButton(int num);  // num is a button number, not the pin number.

    static constexpr int buttonCountInCol() { return btn_count; }
    
  private:
    IDashGfxWrapper &m_gfx;
    
    int m_page_num = 0;
    int m_prev_page = -1;
    PageDefinition **m_page_def;

    const int (&m_button_pins)[btn_count];

    int m_button_state[btn_count] = {0};
    int m_last_button_state[btn_count] = {0};
    unsigned long m_debounce_timer[btn_count] = {LOW};

    int prevPage(int page_num);

    void draw();
};
