#include "monitoring_system.h"
#include <memory>

// EspStrategy
EspStrategy::EspStrategy(std::shared_ptr<SensorsCondition> sensors_condition, std::shared_ptr<ModeCondition> mode_condition) :
    sensors_condition_(sensors_condition),
    mode_condition_(mode_condition){};


Notification EspStrategy::process(const BaseMessage& base_message){
    std::shared_ptr<EspBaseMessage> message_from_esp = std::get<std::shared_ptr<EspBaseMessage>>(base_message);
    std::shared_ptr<ClientBaseMessage> message_to_client;
    
    switch(message_from_esp->getType())
    {
    case StructType::SENSORS_DATA:
        sensors_condition_->updateCondition(std::get<Sensors>(message_from_esp->getStruct()));
        message_to_client = std::make_shared<ClientSensorsMessage>(sensors_condition_->getTmpCondition());
        break;
    case StructType::MODE:
        mode_condition_->updateModeCondition((std::get<Mode>(message_from_esp->getStruct())).mode);
        message_to_client = std::make_shared<ClientModeMessage>(mode_condition_->getCondition());
        break;
    default:
        break;
    }
    
    return Notification(Addressee::Mediator, BaseMessage(message_to_client));
};

// ClientStrategy
ClientStrategy::ClientStrategy(std::shared_ptr<ModeCondition> mode_condition) :
    mode_condition_(mode_condition){};


Notification ClientStrategy::process(const BaseMessage& base_message){
    std::shared_ptr<ClientBaseMessage> message_from_client = std::get<std::shared_ptr<ClientBaseMessage>>(base_message);
    std::shared_ptr<EspBaseMessage> message_to_esp;

    switch(message_from_client->getType())
    {
    case StructType::SWITCH_RELAY:
        message_to_esp = std::make_shared<EspSwitchRelayMessage>(std::get<SwitchRelay>(message_from_client->getStruct()));
        break;
    case StructType::MODE:
        mode_condition_->updateModeCondition((std::get<Mode>(message_from_client->getStruct())).mode);
        message_to_esp = std::make_shared<EspModeMessage>(mode_condition_->getCondition());
        break;
    default:
        break;
    }
    
    return Notification(Addressee::Mediator, BaseMessage(message_to_esp));
};

// DBStrategy
DBStrategy::DBStrategy(std::shared_ptr<ModeCondition> mode_condition) :
    mode_condition_(mode_condition){};

Notification DBStrategy::process(const BaseMessage& base_message){
    std::shared_ptr<DBBaseMessage> message_from_db = std::get<std::shared_ptr<DBBaseMessage>>(base_message);
    if(message_from_db->getStatus() == 200){
        
    }
    return Notification();
};

// MonitoringSystem
void MonitoringSystem::getNotification(const Notification& notification){
    std::unique_ptr<BaseStrategy> strategy;
    auto base_message = notification.getMessage();
    switch (base_message.index())
    {
    case User::Esp:
        strategy = std::make_unique<EspStrategy>(sensors_condition_, mode_condition_);
        break;
    case User::Client:
        strategy = std::make_unique<ClientStrategy>(mode_condition_);
        break;
    case User::DB:
        strategy = std::make_unique<DBStrategy>(mode_condition_);
        break;
    default:
        return;
    }
    auto new_notification = strategy->process(base_message);
    // this->notifyDispatcher(new_notification); do async
};

Addressee MonitoringSystem::getName() const{
    return Addressee::Monitoring_System;
};