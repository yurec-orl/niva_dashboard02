#pragma once

namespace {
  constexpr unsigned long debounce_delay_default = 50;
}

class SignalDebouncer
{
  public:
    SignalDebouncer(int debounce_delay = debounce_delay_default): m_debounce_delay(debounce_delay) {};

    void setDelay(int debounce_delay);
    int read(int input);

  private:
    int m_debounce_delay;
    int m_button_state = 0;
    int m_last_button_state = HIGH;
    unsigned long m_debounce_timer = 0;
};
