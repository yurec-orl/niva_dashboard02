#include <Arduino.h>

#include "SignalProcessing.hpp"

///////////////////////////////////////////////////////////////////////////////
// Signal debouncer
///////////////////////////////////////////////////////////////////////////////

void SignalDebouncer::setDelay(int debounce_delay)
{
  m_debounce_delay = debounce_delay;
}

int SignalDebouncer::value(int input)
{
  if (input != m_last_input_state)
  {
    m_debounce_timer = millis();
  }

  if ((input != m_input_state) && ((millis() - m_debounce_timer) > m_debounce_delay))
  {
    m_input_state = input;
  }

  m_last_input_state = input;

  return m_input_state;
}

void SignalDebouncer::reset()
{
  m_input_state = HIGH;
  m_last_input_state = HIGH;
  m_debounce_timer = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Signal smoother
///////////////////////////////////////////////////////////////////////////////

int SignalSmoother::value(int input)
{
  // Running average implementation. On each invocation, will subtract last reading
  // from m_total and add newest reading to m_total.
  // After that, m_total / sample_count is returned as an average.
  m_total = m_total - m_samples[m_index];
  m_samples[m_index] = input;
  m_total = m_total + m_samples[m_index];
  m_index++;

  if (m_index >= m_sample_count)
  {
    m_index = 0;
  }

  return m_total / m_sample_count;
}

void SignalSmoother::reset()
{
  m_index = 0;
  m_total = 0;
  m_samples.resize(m_sample_count);
  std::fill(m_samples.begin(), m_samples.end(), 0);
}