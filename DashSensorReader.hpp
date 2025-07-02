#pragma once

#include <Arduino.h>

#include "DashSensor.hpp"
#include "SignalProcessing.hpp"

class IDashSensorReader
{
public:
    IDashSensorReader(const int pin): m_pin(pin) {}
    ~IDashSensorReader()
    {
        delete m_sensor;
        delete m_sigProc;
    }

    int read();
    void reset();

protected:
    const int m_pin;

    virtual void setup() {}

    IDashSensor *m_sensor;
    ISignalProcessor *m_sigProc;
};

class DashAnalogSensorReader: public IDashSensorReader
{
public:
    DashAnalogSensorReader(int pin): IDashSensorReader(pin)
    {
        setup();
    };
protected:
    void setup() override;
};

class DashDigitalSensorReader: public IDashSensorReader
{
public:
    DashDigitalSensorReader(int pin, int mode = INPUT): IDashSensorReader(pin)
    {
        setup();
    };
protected:
    void setup() override;
};