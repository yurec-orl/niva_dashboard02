#pragma once

#include "SignalDebouncer.hpp"

enum PushButtonState 
{
  up = 0,
  down = 1,
  pressed = 2   // Repoted once after button has been pressed and released.
};

class PushButton: protected SignalDebouncer
{
  public:
    PushButton(int pin);

    PushButtonState read();           // Read pin value and convert it to PushButtonState.
    PushButtonState state() const;    // Return last PushButtonState value.

  private:
    int m_pin;
    PushButtonState m_state;
};
