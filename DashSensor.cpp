#include "DashSensor.hpp"

#include <Arduino.h>

namespace
{
	const int testSensorCycleMillis = 500;
}

IDashSensor::IDashSensor(int input_pin)
	: m_input_pin(input_pin)
{
}

DashAnalogSensor::DashAnalogSensor(int input_pin)
	: IDashSensor(input_pin)
{
	pinMode(input_pin, INPUT);
}

int DashAnalogSensor::read()
{
	return analogRead(m_input_pin);
}

DashDigitalSensor::DashDigitalSensor(int input_pin, int mode)
	: IDashSensor(input_pin)
{
	pinMode(input_pin, mode);
}

int DashDigitalSensor::read()
{
	return digitalRead(m_input_pin);
}

DashTestSensor::DashTestSensor(int input_pin)
	: IDashSensor(input_pin)
{
	// Serial.print("DashGlobals::setupDone = "); Serial.println(DashGlobals::setupDone);
}

int DashTestSensor::read()
{
	int curTime = millis();
	if (!m_started)
	{
		m_time = curTime;
		m_started = true;
	}

	unsigned long timeDelta = curTime - m_time;
	unsigned long m_valueDelta = timeDelta * 1023 / testSensorCycleMillis;

	m_time = curTime;
	if (m_valueUp)
	{
		m_value += m_valueDelta;
	}
	else
	{
		m_value -= m_valueDelta;
		if (m_value < 0)
			m_value = 0;
	}
	if (m_value >= 1023)
		m_valueUp = false;

	return m_value;
}
