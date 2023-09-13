#include "monitoring_system.h"
#include "monitoring_strategy.h"
#include <memory>

// EspMonitoringSystem
EspMonitoringSystem::EspMonitoringSystem() :
    sensors_condition_(std::make_shared<SensorsCondition>()),
    mode_condition_(std::make_shared<ModeCondition>()){};

void EspMonitoringSystem::getNotification(const Notification& notification){
    std::unique_ptr<BaseStrategy> strategy;
    auto base_message = notification.getMessage();
    switch (base_message.index())
    {
    case User::Esp:
        strategy = std::make_unique<EspStrategyFromEsp>(sensors_condition_, mode_condition_);
        break;
    case User::Client:
        strategy = std::make_unique<ClientStrategyFromEsp>(mode_condition_);
        break;
    case User::DB:
        strategy = std::make_unique<DBStrategyFromEsp>(mode_condition_);
        break;
    default:
        return;
    }
    auto new_notification = strategy->process(base_message);
    this->notifyDispatcher(new_notification);
};

Addressee EspMonitoringSystem::getName() const{
    return Addressee::Monitoring_System;
};

// ClientMonitiringSystem
void ClientMonitiringSystem::getNotification(const Notification& notification){
    std::unique_ptr<BaseStrategy> strategy;
    auto base_message = notification.getMessage();
    switch (base_message.index())
    {
    case User::DB:
        strategy = std::make_unique<DBStrategyFromClient>();
        break;
    default:
        return;
    }
    auto new_notification = strategy->process(base_message);
    this->notifyDispatcher(new_notification);
};

Addressee ClientMonitiringSystem::getName() const{
    return Addressee::Monitoring_System;
};