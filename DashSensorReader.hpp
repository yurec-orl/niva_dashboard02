#pragma once

#include <Arduino.h>

#include "DashSensor.hpp"
#include "SignalProcessing.hpp"

class IDashSensorReader
{
public:
    IDashSensorReader(const int pin): m_pin(pin) {}
    virtual ~IDashSensorReader()
    {
        delete m_sensor;
        delete m_sigProc;
    }

    int read();
    void reset();

    virtual const int maxVal() const = 0;
    virtual const int minVal() const = 0;

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
    virtual ~DashAnalogSensorReader() {}

    const int maxVal() const override {return 1024;}
    const int minVal() const override {return 0;}

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
    virtual ~DashDigitalSensorReader() {}

    const int maxVal() const override {return HIGH;}
    const int minVal() const override {return LOW;}

protected:
    void setup() override;
};