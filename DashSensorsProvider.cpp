#include "DashSensorsProvider.hpp"

namespace 
{
    // Sensor pins. Hardcoded according to physical wiring.
    enum SensorPins 
    {
        // Analog pins
        temp_pin = A2,
        fuel_pin = A1,
        oil_press_pin = A0,
        // Digital pins
        charge_pin = 43,
        fuel_low_pin = 45,
        tacho_pin = 41,
        oil_press_low_pin = 47,
        lights_pin = 46,
        brake_fluid_pin = 44,
        lights_ind_pin = 42,
        blinkers_pin = 40,
        high_beam_pin = 38,
        park_brake_pin = 36,
        diff_lock_pin = 34,
        speed_pin = 32
    };
}

DashSensorsProvider::DashSensorsProvider()
{
    // All sensor reader objects.
    static DashAnalogSensorReader temperature_sensor_reader(SensorPins::temp_pin);
    static DashAnalogSensorReader fuel_sensor_reader(SensorPins::fuel_pin);
    static DashAnalogSensorReader oil_pressure_sensor_reader(SensorPins::oil_press_pin);

    static DashDigitalSensorReader charge_sensor_reader(SensorPins::charge_pin, INPUT_PULLUP);
    static DashDigitalSensorReader fuel_low_sensor_reader(SensorPins::fuel_low_pin, INPUT_PULLUP);
    static DashDigitalSensorReader tacho_sensor_reader(SensorPins::tacho_pin);
    static DashDigitalSensorReader speed_sensor_reader(SensorPins::speed_pin);
    static DashDigitalSensorReader oil_press_low_sensor_reader(SensorPins::oil_press_low_pin, INPUT_PULLUP);
    static DashDigitalSensorReader lights_sensor_reader(SensorPins::lights_pin);
    static DashDigitalSensorReader brake_fluid_sensor_reader(SensorPins::brake_fluid_pin);
    static DashDigitalSensorReader lights_indicator_sensor_reader(SensorPins::lights_ind_pin);
    static DashDigitalSensorReader blinker_sensor_reader(SensorPins::blinkers_pin);
    static DashDigitalSensorReader heigh_beam_sensor_reader(SensorPins::high_beam_pin);
    static DashDigitalSensorReader park_brake_sensor_reader(SensorPins::park_brake_pin, INPUT_PULLUP);
    static DashDigitalSensorReader diff_lock_sensor_reader(SensorPins::diff_lock_pin, INPUT_PULLUP);

    Serial.println("DashSensorsProvider::DashSensorsProvider");
    m_sensors[SensorID::temp] = &temperature_sensor_reader;
    m_sensors[SensorID::fuel] = &fuel_sensor_reader;
    m_sensors[SensorID::oil_press] = &oil_pressure_sensor_reader;

    m_sensors[SensorID::charge] = &charge_sensor_reader;
    m_sensors[SensorID::fuel_low] = &fuel_low_sensor_reader;
    m_sensors[SensorID::tacho] = &tacho_sensor_reader;
    m_sensors[SensorID::speed] = &speed_sensor_reader;
    m_sensors[SensorID::oil_press_low] = &oil_press_low_sensor_reader;
    m_sensors[SensorID::lights] = &lights_sensor_reader;
    m_sensors[SensorID::brake_fluid] = &brake_fluid_sensor_reader;
    m_sensors[SensorID::lights_ind] = &lights_indicator_sensor_reader;
    m_sensors[SensorID::blinkers] = &blinker_sensor_reader;
    m_sensors[SensorID::high_beam] = &heigh_beam_sensor_reader;
    m_sensors[SensorID::park_brake] = &park_brake_sensor_reader;
    m_sensors[SensorID::diff_lock] = &diff_lock_sensor_reader;
}

sensors_t &DashSensorsProvider::sensors()
{
    return m_sensors;
}
