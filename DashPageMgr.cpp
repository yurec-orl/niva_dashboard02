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
  if (m_page_num == -1)
  {
    // No active page - nothing to do.
    return;
  }

  readButtons();
  draw();

  // Check if any of the buttons that switch to another page have been pressed.
  // Find the next page to switch to.
  int next_page = -1;
  for (int i = 0; i < btn_count; ++i)
  {
    PageDefinition *next_page_ptr = m_page_def[m_page_num]->buttons[i].next_page;
    if (m_button_state[i] == LOW && next_page_ptr != nullptr)
    {
      next_page = nextPageNum(next_page_ptr);
    }
  }

  // Call current page callback
  PageState state = m_page_state;
  m_page_state = PageState::on_process;
  if (!m_page_def[m_page_num]->callback(m_gfx, m_button_state, state))
  {
    int prev_page = prevPageNum(m_page_num);
    if (prev_page != -1)
    {
      setPage(prev_page);
    }
  } else if (next_page != -1) 
  {
    setPage(next_page);
  }
}

void DashPageMgr::draw()
{
  // Margin and spacing are hardcoded - I don't expect the physical button layout to change.
  constexpr int top_margin = 20;
  constexpr int spacing = 132;

  char buf[64];
  snprintf(buf, sizeof(buf), "PG %02d", m_page_num);
  m_gfx.textWrite(0, 460, 0, 65504, 0, buf);

  // Button labels

  // x starts on the left, goes to the right once left column has been printed
  int text_x = 0;
  int text_y = top_margin;
  int text_align = 0;
  for (int j = 0; j < btn_count; ++j)
  {
    const char *btn_text = m_page_def[m_page_num]->buttons[j].text;

    int fg_color = 65504;
    int bg_color = 0;
    if (m_button_state[j] == LOW)
    {
      int tmp = fg_color;
      fg_color = bg_color;
      bg_color = tmp;
    }

    m_gfx.textWrite(text_x - (text_align * strlen(btn_text) * 16), text_y, 1, fg_color, bg_color, btn_text);

    text_y += spacing;
    if (text_y > m_gfx.height())
    {
      text_y = top_margin;    // Reset y coordinate when reached the end of the first column.
                              // The code expects the number of buttons to fit in exactly two columns.
      text_x = 800;           // Move x coordinate to the right column.
      text_align = 1;
    }
  }
}

int DashPageMgr::readButton(int num)
{
  return btn_debouncer[num].read(digitalRead(m_button_pins[num]));
}

int DashPageMgr::nextPageNum(PageDefinition *next_page_ptr)
{
  if (next_page_ptr == nullptr)
  {
    return -1;
  }
  
  for (int i = 0; m_page_def[i] != nullptr; ++i)
  {
    if (m_page_def[i] == next_page_ptr)
    {
      return i;
    }
  }
  return -1;
}

int DashPageMgr::prevPageNum(int page_num)
{
  if (m_page_num != -1)
  {
    for (int i = 0; m_page_def[i] != nullptr; ++i)
    {
      for (int j = 0; j < btn_count; ++j)
      {
        if (m_page_def[m_page_num] == m_page_def[i]->buttons[j].next_page)
        {
          return i;
        }
      }
    }
  }
  return -1;
}

void DashPageMgr::setPage(int page_num)
{
  // Ivoke current page callback once to notify that we are leaving the page.
  if (m_page_num != -1)
  {
    m_page_def[m_page_num]->callback(m_gfx, m_button_state, PageState::on_leave);
  }
  
  m_gfx.fillScreen(RA8875_BLACK);
  m_page_num = page_num;
  m_page_state = PageState::on_switch;
}

void DashPageMgr::readButtons()
{
  for (int i = 0; i < btn_count; ++i)
  {
    m_button_state[i] = readButton(i);
  }
}
