#include "DashPageMgr.hpp"

DashPageMgr::DashPageMgr(IDashGfxWrapper &gfx, const int (&button_pins)[btn_count], PageDefinition **page_def): m_gfx(gfx), m_button_pins(button_pins), m_page_def(page_def)
{
  // Set up input pins for defined buttons
  for (int i = 0; i < btn_count; ++i)
  {
    pinMode(m_button_pins[i], INPUT_PULLUP);
  }
};

void DashPageMgr::loop()
{
  draw();

  // Call current page callback
  if (!m_page_def[m_page_num]->callback(m_gfx))
  {
    int prev_page = prevPage(m_page_num);
    if (prev_page != -1)
    {
      ;
    }
  }
}

void DashPageMgr::draw()
{
  // Margin and spacing are hardcoded - I don't expect the physical button layout to change.
  constexpr int top_margin = 20;
  constexpr int spacing = 132;

  constexpr char *msg = "PAGEMGR_DRAW";
  
  m_gfx.textWrite(256, 80, 1, 65504, 0, msg);

  // Button labels

  // x starts on the left, goes to the right once left column has been printed
  int text_x = 0;
  int text_y = top_margin;
  for (int j = 0; j < btn_count; ++j)
  {
    const char *btn_text = m_page_def[m_page_num]->buttons[j].text;

    int fg_color = 65504;
    int bg_color = 0;
    if (readButton(j) == LOW)
    {
      int tmp = fg_color;
      fg_color = bg_color;
      bg_color = tmp;
    }

    m_gfx.textWrite(text_x, text_y, 1, fg_color, bg_color, btn_text);

    text_y += spacing;
    if (text_y > m_gfx.height())
    {
      text_y = top_margin;    // Reset y coordinate when reached the end of the first column.
                              // The code expects the number of buttons to fit in exactly two columns.
      text_x = 764;           // Move x coordinate to the right column.
    }
  }
}

int DashPageMgr::readButton(int num)
{
  int input = digitalRead(m_button_pins[num]);

  if (input != m_last_button_state[num])
  {
    m_debounce_timer[num] = millis();
  }

  if ((millis() - m_debounce_timer[num]) > debounce_delay) {
    if (input != m_button_state[num])
    {
      m_button_state[num] = input;
    }
  }

  m_last_button_state[num] = input;

  return m_button_state[num];
}

int DashPageMgr::prevPage(int page_num)
{
  return -1;
}
