#include "DashSensorsProvider.hpp"

namespace 
{
    // Sensor pins. Hardcoded according to physical wiring.
    enum SensorPins 
    {
        temp_pin = A2,
        charge = 43,
        fuel = A1,
        fuel_low = 45,
        tacho = 41,
        oil_press = A0,
        oil_press_low = 47,
        lights = 46,
        brake_fluid = 44,
        lights_ind = 42,
        blinkers = 40,
        high_beam = 38,
        park_brake = 36,
        diff_lock = 34
    };

    // All sensor reader objects.
    DashAnalogSensorReader temp_sensor_reader(temp_pin);
}

DashSensorsProvider::DashSensorsProvider()
{
    m_sensors[SensorID::temp] = &temp_sensor_reader;
}

sensors_t &DashSensorsProvider::sensors()
{
    return m_sensors;
}
