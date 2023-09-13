#include "monitoring_strategy.h"

// EspStrategyFromEsp
EspStrategyFromEsp::EspStrategyFromEsp(std::shared_ptr<SensorsCondition> sensors_condition, std::shared_ptr<ModeCondition> mode_condition) :
    sensors_condition_(sensors_condition),
    mode_condition_(mode_condition){};


Notification EspStrategyFromEsp::process(const BaseMessage& base_message){
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

// ClientStrategyFromEsp
ClientStrategyFromEsp::ClientStrategyFromEsp(std::shared_ptr<ModeCondition> mode_condition) :
    mode_condition_(mode_condition){};


Notification ClientStrategyFromEsp::process(const BaseMessage& base_message){
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
    

    return Notification(Addressee::Messenger, BaseMessage(message_to_esp));
};

// DBStrategyFromEsp
DBStrategyFromEsp::DBStrategyFromEsp(std::shared_ptr<ModeCondition> mode_condition) :
    mode_condition_(mode_condition){};

Notification DBStrategyFromEsp::process(const BaseMessage& base_message){
    std::shared_ptr<DBBaseMessage> message_from_db = std::get<std::shared_ptr<DBBaseMessage>>(base_message);
    if(message_from_db->getStatus() == 200 && message_from_db->getAnswer().first){
        mode_condition_->updatePredictedDataCondition(std::atof(message_from_db->getAnswer().second[0][4].c_str()) / std::atof(message_from_db->getAnswer().second[0][5].c_str()));
    }
    return Notification();
};

// DBStrategyFromClient
Notification DBStrategyFromClient::process(const BaseMessage& base_message){
    std::shared_ptr<DBBaseMessage> message_from_db = std::get<std::shared_ptr<DBBaseMessage>>(base_message);
    std::shared_ptr<ClientBaseMessage> message_to_client;

    if(message_from_db->getStatus() == 200 && message_from_db->getAnswer().first){
        Consumers consumers_t;
        consumers_t.push_back(std::vector<Consumer>());
        consumers_t.push_back(std::vector<Consumer>());
        consumers_t.push_back(std::vector<Consumer>());
        std::cout << "proccess" << std::endl;
        for(auto row: message_from_db->getAnswer().second)
            consumers_t[std::stoi(row.at(4)) - 1].push_back(Consumer{row.at(2), std::atof(row.at(3).c_str())});
    
        message_to_client = std::make_shared<ClientConsumersMessage>(consumers_t);
    }  

    return Notification(Addressee::Messenger, BaseMessage(message_to_client));
};