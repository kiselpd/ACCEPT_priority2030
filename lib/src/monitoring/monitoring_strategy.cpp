#include "monitoring_strategy.h"

// StrategyMessageFromEsp
StrategyMessageFromEsp::StrategyMessageFromEsp(std::shared_ptr<SensorsCondition> sensors_condition) : sensors_condition_(sensors_condition){};

Notification StrategyMessageFromEsp::process(const BaseMessage &base_message)
{
    std::shared_ptr<EspBaseMessage> message_from_esp = std::get<std::shared_ptr<EspBaseMessage>>(base_message);
    std::shared_ptr<ClientBaseMessage> message_to_client;

    if (message_from_esp->getType() == StructType::SENSORS_DATA)
    {
        sensors_condition_->updateCondition(std::get<Sensors>(message_from_esp->getStruct()));
        message_to_client = std::make_shared<ClientSensorsMessage>(sensors_condition_->getTmpCondition());
    }

    return Notification(Addressee::Mediator, BaseMessage(message_to_client));
};

// StrategyMessageFromClient
StrategyMessageFromClient::StrategyMessageFromClient(std::shared_ptr<ModeCondition> mode_condition) : mode_condition_(mode_condition){};

Notification StrategyMessageFromClient::process(const BaseMessage &base_message)
{
    std::shared_ptr<ClientBaseMessage> message_from_client = std::get<std::shared_ptr<ClientBaseMessage>>(base_message);
    std::shared_ptr<EspBaseMessage> message_to_esp;

    switch (message_from_client->getType())
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

    return Notification(Addressee::Messenger, BaseMessage(message_to_esp));
};

// StrategyMessageFromDB
StrategyMessageFromDB::StrategyMessageFromDB(std::shared_ptr<ModeCondition> mode_condition) : mode_condition_(mode_condition){};

Notification StrategyMessageFromDB::process(const BaseMessage &base_message)
{
    std::shared_ptr<DBBaseMessage> message_from_db = std::get<std::shared_ptr<DBBaseMessage>>(base_message);
    std::shared_ptr<EspBaseMessage> message_to_esp;

    if (message_from_db->getStatus() == 200 && message_from_db->getAnswer().first)
    {
        mode_condition_->updatePredictedDataCondition(std::atof(message_from_db->getAnswer().second[0][3].c_str()) / std::atof(message_from_db->getAnswer().second[0][4].c_str()));
        message_to_esp = std::make_shared<EspModeMessage>(mode_condition_->getCondition());
    }

    return Notification(Addressee::Messenger, BaseMessage(message_to_esp));
};