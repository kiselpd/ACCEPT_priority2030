#include "client_message.h"
#include "parser.h"

// ClientDatagramMessage
ClientDatagramMessage::ClientDatagramMessage(){};

ClientDatagramMessage::ClientDatagramMessage(const Datagram& t_struct) :
    t_struct_(t_struct){};

std::string ClientDatagramMessage::getJson() const{
    return datagram_to_json(t_struct_);
};

client_struct ClientDatagramMessage::getStruct() const{
    return client_struct(t_struct_);
};

ClientStructType ClientDatagramMessage::getType() const{
    return ClientStructType::DATAGRAM;
};

size_t ClientDatagramMessage::setJson(const std::string& json){
    return get_datagram_from_json(t_struct_, json);
};

// ClientAuthMessage
ClientAuthMessage::ClientAuthMessage(){};

ClientAuthMessage::ClientAuthMessage(const Auth& t_struct) :
    t_struct_(t_struct){};

std::string ClientAuthMessage::getJson() const{
    return auth_to_json(t_struct_);
};

client_struct ClientAuthMessage::getStruct() const{
    return client_struct(t_struct_);
};

ClientStructType ClientAuthMessage::getType() const{
    return ClientStructType::AUTH;
};

size_t ClientAuthMessage::setJson(const std::string& json){
    return get_auth_from_json(t_struct_, json);
};

// ClientSensorsMessage
ClientSensorsMessage::ClientSensorsMessage(){};

ClientSensorsMessage::ClientSensorsMessage(const FullSensors& t_struct) :
    t_struct_(t_struct){};

std::string ClientSensorsMessage::getJson() const{
    return full_sensors_to_json(t_struct_);
};

client_struct ClientSensorsMessage::getStruct() const{
    return client_struct(t_struct_);
};

ClientStructType ClientSensorsMessage::getType() const{
    return ClientStructType::SENSORS_DATA;
};

size_t ClientSensorsMessage::setJson(const std::string& json){
    return EXIT_FAILURE;
};

// ClientModeMessage
ClientModeMessage::ClientModeMessage(){};

ClientModeMessage::ClientModeMessage(const Mode& t_struct) :
    t_struct_(t_struct){};

std::string ClientModeMessage::getJson() const{
    return mode_to_json(t_struct_);
};

client_struct ClientModeMessage::getStruct() const{
    return client_struct(t_struct_);
};

ClientStructType ClientModeMessage::getType() const{
    return ClientStructType::MODE;
};

size_t ClientModeMessage::setJson(const std::string& json){
    return get_mode_from_json(t_struct_, json);
};

// ClientSwitchRelayMessage
ClientSwitchRelayMessage::ClientSwitchRelayMessage(){};

ClientSwitchRelayMessage::ClientSwitchRelayMessage(const SwitchRelay& t_struct) :
    t_struct_(t_struct){};

std::string ClientSwitchRelayMessage::getJson() const{
    return std::string();
};

client_struct ClientSwitchRelayMessage::getStruct() const{
    return client_struct(t_struct_);
};

ClientStructType ClientSwitchRelayMessage::getType() const{
    return ClientStructType::SWITCH_RELAY;
};

size_t ClientSwitchRelayMessage::setJson(const std::string& json){
    return get_switch_relay_from_json(t_struct_, json);
};

// ClientConsumersMessage
ClientConsumersMessage::ClientConsumersMessage(){};

ClientConsumersMessage::ClientConsumersMessage(const Consumers& t_struct) :
    t_struct_(t_struct){};

std::string ClientConsumersMessage::getJson() const{
    return consumers_to_json(t_struct_);
};

client_struct ClientConsumersMessage::getStruct() const{
    return client_struct(t_struct_);
};

ClientStructType ClientConsumersMessage::getType() const{
    return ClientStructType::CONSUMERS_DATA;
};

size_t ClientConsumersMessage::setJson(const std::string& json){
    return EXIT_FAILURE;
};