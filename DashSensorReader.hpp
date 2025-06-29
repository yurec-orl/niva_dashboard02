#pragma once

#include <Arduino.h>

#include "DashSensor.hpp"
#include "SignalProcessing.hpp"

class IDashSensorReader
{
public:
    IDashSensorReader(const char * name, const int pin): m_name(name), m_pin(pin) 
    {
        setup();
    }
    ~IDashSensorReader()
    {
        delete m_sensor;
        delete m_sigProc;
    }

    int read();
    void reset();

protected:
    const char *m_name;
    const int m_pin;

    virtual void setup() = 0;

    IDashSensor *m_sensor;
    ISignalProcessor *m_sigProc;
};

class DashAnalogSensorReader: public IDashSensorReader
{
private:
    virtual void setup();
};

class DashDigitalSensorReader: public IDashSensorReader
{
private:
    virtual void setup();
};