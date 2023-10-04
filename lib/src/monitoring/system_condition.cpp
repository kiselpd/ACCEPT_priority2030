#include "system_condition.h"

// SensorsCondition
void SensorsCondition::updateCondition(const Sensors& sensors_data){
    this->tmp_sensors_condition_ = this->calculateFullSensors_(sensors_data);
    this->accumulated_power_ += this->tmp_sensors_condition_;
    this->measurements_count_++;
};

FullSensors SensorsCondition::getTmpCondition() const{
    return this->tmp_sensors_condition_;
};

FullPower SensorsCondition::getPowerCondition() const{
    return (this->accumulated_power_ / this->measurements_count_);
};

void SensorsCondition::resetMeasurements(){
    this->accumulated_power_ = FullPower();
    this->measurements_count_ = 0;
};

FullSensors SensorsCondition::calculateFullSensors_(const Sensors& sensors_data){
    FullSensors full_sensors_data;

    full_sensors_data.solar = this->calculateFullSensor_(sensors_data.solar);
    full_sensors_data.solar.percentages = full_sensors_data.solar.current / 3 * 100;
    full_sensors_data.wind = this->calculateFullSensor_(sensors_data.wind);
    full_sensors_data.wind.percentages = full_sensors_data.wind.current / 3 * 100;
    full_sensors_data.generator = this->calculateFullSensor_(sensors_data.generator);
    full_sensors_data.generator.percentages = full_sensors_data.generator.current * 100;
    full_sensors_data.battery = this->calculateFullBattery_(sensors_data);

    for (size_t i = 0; i < CONSUMERS_NUMBER; i++){
        full_sensors_data.consumer[i] = this->calculateFullSensor_(sensors_data.consumer[i]);
        full_sensors_data.consumer[i].percentages = full_sensors_data.consumer[i].current / 1.5 * 100;
    }
    
    return full_sensors_data;
};

FullBattery SensorsCondition::calculateFullBattery_(const Sensors& sensors_data) const{
    FullBattery battery;
    double tmp_voltage = sensors_data.battery_voltage * 3.3 * 5.3 / 4096;
    battery.voltage = tmp_voltage + (0.00028*(1+0.0001*(tmp_voltage - 12.5))*(sensors_data.consumer[0].current / 1000 + sensors_data.consumer[1].current / 1000 + sensors_data.consumer[2].current / 1000 - sensors_data.solar.current / 2000 - sensors_data.wind.current / 2000 - sensors_data.generator.current * 4000));
    if(sensors_data.generator.voltage > 100 && sensors_data.generator.current < 0.05)
        battery.voltage -= 2;
    battery.percentages = (battery.voltage - 7.2) / (14 - 7.2) * 100;
    battery.status = ((sensors_data.solar.current * sensors_data.solar.voltage + sensors_data.wind.current * sensors_data.wind.voltage + sensors_data.generator.current * sensors_data.generator.voltage) - (sensors_data.consumer[0].current * sensors_data.consumer[0].voltage + sensors_data.consumer[1].current * sensors_data.consumer[1].voltage + sensors_data.consumer[2].current * sensors_data.consumer[2].current) > 0) ? (1) : (0);

    return battery;
};

FullRelay SensorsCondition::calculateFullSensor_(const Relay& sensor_data) const{
    FullRelay full_sensor_data;

    full_sensor_data.current = sensor_data.current;
    full_sensor_data.voltage = sensor_data.voltage;
    full_sensor_data.status = sensor_data.status;
    full_sensor_data.power = sensor_data.current * sensor_data.voltage;

    return full_sensor_data;
};

// ModeCondition
void ModeCondition::updateModeCondition(const int& mode_condition){
    tmp_mode_condition_.mode = mode_condition;
};

void ModeCondition::updatePredictedDataCondition(const double& predicted_data_condition){
    tmp_mode_condition_.k = predicted_data_condition;
    std::cout << "K= " << tmp_mode_condition_.k << std::endl;
};

Mode ModeCondition::getCondition() const{
    return tmp_mode_condition_;
};