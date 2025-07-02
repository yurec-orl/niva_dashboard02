#include "DashSensorReader.hpp"

int IDashSensorReader::read()
{
    return m_sigProc->value(m_sensor->read());
}

void IDashSensorReader::reset()
{
    // Reset signal processor.
    m_sigProc->reset();
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
