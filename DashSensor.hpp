#pragma once

#include <Arduino.h>
#include <Arduino_AVRSTL.h>

class IDashSensor
{
public:
	IDashSensor(const char *name, int inputPin);

	const char *getName() { return m_name; }

	virtual int read() = 0;
protected:
	int m_inputPin;
	const char * m_name;

	int m_value = 0;
};

class DashAnalogSensor : public IDashSensor
{
public:
	DashAnalogSensor(const char *name, int inputPin);

	int read() override;
};

class DashDigitalSensor : public IDashSensor
{
public:
	DashDigitalSensor(const char *name, int inputPin);

	int read() override;
};

class DashTestSensor: public IDashSensor
{
public:
	DashTestSensor(const char *name, int inputPin);

	int read() override;
private:
	int m_time = 0;
	bool m_valueUp = true;
	bool m_started = false;
};