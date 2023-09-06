#include "monitoring_system.h"
#include "message.h"
#include "parser.h"

#include <sstream>

// ModeCondition
Mode ModeCondition::getMode() const{return mode_;};

void ModeCondition::updateCondition(const Mode& mode, const FullSensorsData& full_sensors_data){
    mode_ = mode;
};

// SensorsDataCondition
FullSensorsData SensorsDataCondition::getFullSensorsData() const{return sensors_data_;};

void SensorsDataCondition::updateCondition(const SensorsData& sensors_data){
    this->sensors_data_ = this->calculateFullSensorsData_(sensors_data);
    // power_data_.solar_energy += sensors_data_.solar.power;

    // this->updateDatabaseInfo_();
};

void SensorsDataCondition::updateDatabaseInfo_(){

};

FullSensorsData SensorsDataCondition::calculateFullSensorsData_(const SensorsData& sensors_data){
    FullSensorsData full_sensors_data;

    full_sensors_data.solar = this->calculateFullSensorData_(sensors_data.solar);
    full_sensors_data.wind = this->calculateFullSensorData_(sensors_data.wind);
    full_sensors_data.generator = this->calculateFullSensorData_(sensors_data.generator);
    full_sensors_data.battery = this->calculateFullBatteryData_(sensors_data);

    for (size_t i = 0; i < CONSUMERS_NUMBER; i++)
        full_sensors_data.consumer[i] = this->calculateFullSensorData_(sensors_data.consumer[i]);
    
    return full_sensors_data;
};

FullBatteryData SensorsDataCondition::calculateFullBatteryData_(const SensorsData& sensors_data) const{
    FullBatteryData bat;
    bat.voltage = sensors_data.battery_voltage;
    bat.percentages = (bat.voltage - 7.2) / 5.4;
    bat.status = 1;

    return bat;
};

FullRelayData SensorsDataCondition::calculateFullSensorData_(const RelayData& sensor_data) const{
    FullRelayData full_sensor_data;

    full_sensor_data.current = sensor_data.current;
    full_sensor_data.voltage = sensor_data.voltage;
    full_sensor_data.status = sensor_data.status;
    full_sensor_data.power = sensor_data.current * sensor_data.voltage;
    full_sensor_data.percentages = 32.12; //пока так

    return full_sensor_data;
};

// MonitoringSystem
void MonitoringSystem::getNotification(const Notification& notification){
    auto message = notification.getMessage();

    std::cout << "I.m here" << std::endl;
    switch (message->getType())
    {
    case Type::MODE:
    {
        auto t_struct = std::get<Mode>(message->getStruct());
        this->getModeRequest_(t_struct);
        break;
    }
    case Type::SENSORS_DATA:
    {
        auto t_struct = std::get<SensorsData>(message->getStruct());
        this->getSensorsDataRequest_(t_struct);
        break;
    }
    default:
        break;
    }
};

Addressee MonitoringSystem::getName() const{return Addressee::Monitoring_System;};


void MonitoringSystem::getSensorsDataRequest_(const SensorsData& sensors_data){
    this->sensors_data_condition_.updateCondition(sensors_data);
    std::shared_ptr<BaseMessage> message = std::make_shared<FullSensorsDataMessage>(this->sensors_data_condition_.getFullSensorsData());
    this->notifyDispatcher(Notification(Addressee::Mediator, message));
};


void MonitoringSystem::getModeRequest_(const Mode& mode){
    this->mode_condition_.updateCondition(mode, this->sensors_data_condition_.getFullSensorsData());
    std::shared_ptr<BaseMessage> message = std::make_shared<SwitchModeMessage>(this->mode_condition_.getMode());
    this->notifyDispatcher(Notification(Addressee::Messenger, message));
};