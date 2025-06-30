#pragma once

#include <Arduino.h>
#include <Arduino_AVRSTL.h>
#include <array>

#include "DashSensorReader.hpp"

// Sensor IDs
enum SensorID
{
    temp = 0,
    charge,
    fuel,
    fuel_low,
    tacho,
    oil_press,
    oil_press_low,
    lights,
    brake_fluid,
    lights_ind,
    blinkers,
    high_beam,
    park_brake,
    diff_lock,
    _sz
};

using sensors_t = std::array<IDashSensorReader*, SensorID::_sz>;

// Singleton class which provides sensor reading objects to consumers.
// Sensor readers are identified by string literal names.
class DashSensorsProvider
{
public:
    static DashSensorsProvider &instance()
    {
        static DashSensorsProvider _instance;
        return _instance;
    }
    DashSensorsProvider(DashSensorsProvider const&) = delete;
    void operator=(DashSensorsProvider const&) = delete;    

    sensors_t &sensors();
private:
    DashSensorsProvider();

    sensors_t m_sensors = {nullptr};
};