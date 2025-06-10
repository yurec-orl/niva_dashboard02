#include <Arduino.h>

#include "PushButton.hpp"

namespace {
  constexpr int btn_delay = 25;
}

PushButton::PushButton(int pin): SignalDebouncer(btn_delay), m_pin(pin), m_state(PushButtonState::up)
{
  pinMode(m_pin, INPUT_PULLUP);
}

PushButtonState PushButton::read()
{
  int input = SignalDebouncer::read(digitalRead(m_pin));

  switch (input)
  {
    case HIGH:  // Button is not pressed.
      m_state = PushButtonState::up;
      break;
    case LOW:   // Button is pressed.
      PushButtonState prev_state = m_state;
      if (m_state == PushButtonState::up)
      {
        m_state = PushButtonState::pressed;
      } else {
        m_state = PushButtonState::down;
      }
      break;
  }

  return m_state;
}

PushButtonState PushButton::state() const
{
  return m_state;
}
