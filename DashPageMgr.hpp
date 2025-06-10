#pragma once

#include <Arduino.h>
#include <Arduino_AVRSTL.h>

#include "DashGfxWrapper.hpp"
#include "PushButton.hpp"

namespace {
  constexpr int btn_count = 8;                  // number of physical buttons
}

enum PageState
{
  on_process = 0,   // Page has already been activated.
  on_switch = 1,    // Page has just been activated and the callback is invoked for the first time.
  on_leave = 2      // Leaving the page after callback has returned.
};

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
  // Callback for page specific stuff. on_switched is true when called first time after switching from another page.
  // Return false to switch to previous page.
  bool (*callback)(IDashGfxWrapper &gfx, const std::vector<PushButton> &buttons, PageState state = PageState::on_process);
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

    PushButtonState buttonState(int num);  // num is a button number, not the pin number.

    static constexpr int buttonCountInCol() { return btn_count; }
    
  private:
    IDashGfxWrapper &m_gfx;
    
    int m_page_num = -1;
    int m_prev_page = -1;
    PageState m_page_state = PageState::on_process;   // True after page switch before page callback is invoked for the first time.

    PageDefinition **m_page_def;

    std::vector<PushButton> m_buttons;

    void readButtons();

    int nextPageNum(PageDefinition *next_page_ptr);
    int prevPageNum(int page_num);

    void draw();
};
