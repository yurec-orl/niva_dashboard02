#include "DashSensorReader.hpp"

int IDashSensorReader::read()
{
    if (m_sigProc != nullptr && m_sensor != nullptr) {
        return m_sigProc->value(m_sensor->read());
    }
    return 0; // Return safe default if objects are null
}

void IDashSensorReader::reset()
{
    // Reset signal processor.
    if (m_sigProc != nullptr) {
        m_sigProc->reset();
    }
}

void DashAnalogSensorReader::setup()
{
    m_sigProc = new SignalSmoother;
    m_sensor = new DashAnalogSensor(m_pin);
}

void DashDigitalSensorReader::setup()
{
    m_sigProc = new SignalDebouncer;
    m_sensor = new DashDigitalSensor(m_pin);
}

void DashAnalogTestReader::setup()
{
    // Create a pass-through signal processor for consistency
    m_sigProc = new SignalSmoother;
    m_sensor = nullptr; // No real sensor needed for test reader
}

int DashAnalogTestReader::read()
{
    if (!m_isActive) {
        return m_minValue; // Return minimum value when not active
    }
    
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - m_startTime;
    
    // Check if cycle is complete
    if (elapsed >= m_cycleDurationMs) {
        m_isActive = false;
        return m_minValue;
    }
    
    // Calculate position in cycle (0.0 to 1.0)
    float cyclePosition = (float) elapsed / (float) m_cycleDurationMs;
    
    // Create triangle wave: 0 -> 1 -> 0 over the cycle
    float triangleWave;
    if (cyclePosition <= 0.5f) {
        // Rising edge: 0 to 1
        triangleWave = cyclePosition * 2.0f;
    } else {
        // Falling edge: 1 to 0
        triangleWave = 2.0f * (1.0f - cyclePosition);
    }
    
    // Map to value range
    int value = m_minValue + (int)((m_maxValue - m_minValue) * triangleWave);
    
    // Apply signal processing for consistency with other readers (with null check)
    if (m_sigProc != nullptr) {
        return m_sigProc->value(value);
    }
    return value; // Return raw value if no signal processor
}

void DashAnalogTestReader::reset()
{
    m_startTime = millis();
    m_isActive = true;
    
    // Reset signal processor (with null check)
    if (m_sigProc != nullptr) {
        m_sigProc->reset();
    }
}
