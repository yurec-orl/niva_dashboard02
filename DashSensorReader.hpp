#pragma once

#include <Arduino.h>

#include "DashSensor.hpp"
#include "SignalProcessing.hpp"

class IDashSensorReader
{
public:    IDashSensorReader(const int pin): m_pin(pin), m_sensor(nullptr), m_sigProc(nullptr) {}
    virtual ~IDashSensorReader()
    {
        delete m_sensor;
        delete m_sigProc;
    }

    virtual int read();
    virtual void reset();

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
    virtual ~DashDigitalSensorReader() {}    const int maxVal() const override {return HIGH;}
    const int minVal() const override {return LOW;}

protected:
    void setup() override;
};

class DashAnalogTestReader: public IDashSensorReader
{
public:
    DashAnalogTestReader(int cycleDurationMs = 5000, int minValue = 0, int maxValue = 1023): 
        IDashSensorReader(0), // No real pin needed for test reader
        m_cycleDurationMs(cycleDurationMs),
        m_minValue(minValue),
        m_maxValue(maxValue),
        m_startTime(0),
        m_isActive(false)
    {
        setup();
    };
    virtual ~DashAnalogTestReader() {}

    const int maxVal() const override { return m_maxValue; }
    const int minVal() const override { return m_minValue; }
    
    int read() override;
    void reset() override;

protected:
    void setup() override;

private:
    const unsigned long m_cycleDurationMs;
    const int m_minValue;
    const int m_maxValue;
    unsigned long m_startTime;
    bool m_isActive;
};