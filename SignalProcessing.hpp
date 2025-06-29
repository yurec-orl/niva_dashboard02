#pragma once

#include <Arduino_AVRSTL.h>
#include <array>

namespace
{
  constexpr unsigned long _debounce_delay_default = 50;
  constexpr size_t _sample_count = 10;
}

///////////////////////////////////////////////////////////////////////////////

class ISignalProcessor
{
public:
  // Return processed value based on current input
  // and signal processing algorithm.
  virtual int value(int input) = 0;
  
  // Reset processor to initial state.
  virtual void reset() = 0;
};

///////////////////////////////////////////////////////////////////////////////

class SignalDebouncer: public ISignalProcessor
{
public:
  SignalDebouncer(int debounce_delay = _debounce_delay_default) : m_debounce_delay(debounce_delay) 
  {
    reset();
  }

  // Set debouncing delay.
  void setDelay(int debounce_delay);
  
  // Return value after applying debouncing algorithm.
  int value(int input);

  // Reset internal state to initial.
  void reset();

private:
  unsigned int m_debounce_delay;
  int m_input_state;
  int m_last_input_state;
  unsigned long m_debounce_timer;
};

///////////////////////////////////////////////////////////////////////////////

class SignalSmoother: public ISignalProcessor
{
public:
  SignalSmoother(int sample_count = _sample_count): m_sample_count(sample_count)
  {
    reset();
  }

  // Set sample buffer size.
  // Will reset internal state.
  void set_sample_count(size_t sample_count)
  {
    m_sample_count = sample_count;
    reset();
  }

  // Return smoothed value based on previous <sample_count> readings.
  // Uses buffer-based running average approach.
  int value(int input);

  // Reset internal state to initial.
  void reset();

private:
  int m_index;
  int m_total;
  size_t m_sample_count;
  std::vector<int> m_samples;
};