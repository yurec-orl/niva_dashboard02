#include <Arduino.h>

#include "SignalDebouncer.hpp"

void SignalDebouncer::setDelay(int debounce_delay)
{
  m_debounce_delay = debounce_delay;
}

int SignalDebouncer::debounce(int input)
{
  if (input != m_last_button_state)
  {
    m_debounce_timer = millis();
  }

  if ((input != m_button_state) && ((millis() - m_debounce_timer) > m_debounce_delay)) {
    m_button_state = input;
  }

  m_last_button_state = input;

  return m_button_state;
}
