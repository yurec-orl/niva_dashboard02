#pragma once

#include <Arduino.h>
#include <Arduino_AVRSTL.h>

class IDashSensor
{
public:
	IDashSensor(int input_pin);

	virtual int read() = 0;
protected:
	int m_input_pin;

	int m_value = 0;
};

class DashAnalogSensor : public IDashSensor
{
public:
	DashAnalogSensor(int input_pin);

	int read() override;
};

class DashDigitalSensor : public IDashSensor
{
public:
	DashDigitalSensor(int input_pin, int mode = INPUT);

	int read() override;
};

class DashTestSensor: public IDashSensor
{
public:
	DashTestSensor(int input_pin);

	int read() override;
private:
	int m_time = 0;
	bool m_valueUp = true;
	bool m_started = false;
};