#include "DashSensor.hpp"

#include <Arduino.h>

namespace
{
	const int testSensorCycleMillis = 500;
}

IDashSensor::IDashSensor(const char *name, int inputPin)
	: m_name(name), m_inputPin(inputPin)
{
}

DashAnalogSensor::DashAnalogSensor(const char *name, int inputPin)
	: IDashSensor(name, inputPin)
{
}

int DashAnalogSensor::read()
{
	return analogRead(m_inputPin);
}

DashDigitalSensor::DashDigitalSensor(const char *name, int inputPin)
	: IDashSensor(name, inputPin)
{
}

int DashDigitalSensor::read()
{
	return digitalRead(m_inputPin);
}

DashTestSensor::DashTestSensor(const char *name, int inputPin)
	: IDashSensor(name, inputPin)
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

	/*if (m_value > 0)
	{
		Serial.println("--");
		Serial.println(curTime);
		Serial.println(timeDelta);
		Serial.println(m_valueDelta);
		Serial.println(m_value);
	}*/

	return m_value;
}
