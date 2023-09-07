#include "message.h"
#include "parser.h"

// class BaseMessage
std::shared_ptr<char[]> BaseMessage::getBuffer() const{return nullptr;};

std::string BaseMessage::getJson() const{return "";};

size_t BaseMessage::setJson(const std::string& json){return EXIT_FAILURE;};

size_t BaseMessage::getBufferSize() const{return 0;};

// TypeMessage
std::shared_ptr<char[]> TypeMessage::getBuffer() const{
    StructType struct_type_{this->getType(), sizeof(StructType)};
    std::shared_ptr<char[]> buffer{new char[sizeof(StructType)]};
    std::memcpy(buffer.get(), &struct_type_, sizeof(struct_type_));
    return buffer;
};

std::string TypeMessage::getJson() const{
    return make_type(this->getType());
};

TypeMessage::TypeMessage(const Type& type) : type_(type){};

Type TypeMessage::getType() const {return type_;};

struct_variant TypeMessage::getStruct() const {return struct_variant(StructType{type_, 0});};

// SwitchRelayMessage
std::shared_ptr<char[]> SwitchRelayMessage::getBuffer() const{
    StructType struct_type_{this->getType(), sizeof(t_struct_)};
    std::shared_ptr<char[]> buffer{new char[sizeof(struct_type_) + sizeof(t_struct_)]};
    std::memcpy(buffer.get(), &struct_type_, sizeof(struct_type_));
    std::memcpy(buffer.get() + sizeof(struct_type_), &t_struct_, sizeof(t_struct_));
    return buffer;
};

size_t SwitchRelayMessage::setJson(const std::string& json){
    SwitchRelay t_struct;
    auto error = get_switch_relay_from_json(t_struct, json);
    if(!error)
        this->t_struct_ = t_struct;
    
    return error;
};

Type SwitchRelayMessage::getType() const {return Type::SWITCH_RELAY;};

struct_variant SwitchRelayMessage::getStruct() const{return struct_variant(t_struct_);};

size_t SwitchRelayMessage::getBufferSize() const{return sizeof(StructType) + sizeof(SwitchRelay);};

// SwitchModeMessage
SwitchModeMessage::SwitchModeMessage(const Mode& mode){t_struct_ = mode;};

std::shared_ptr<char[]> SwitchModeMessage::getBuffer() const{
    StructType struct_type_{this->getType(), sizeof(t_struct_)};
    std::shared_ptr<char[]> buffer{new char[sizeof(struct_type_) + sizeof(t_struct_)]};
    std::memcpy(buffer.get(), &struct_type_, sizeof(struct_type_));
    std::memcpy(buffer.get() + sizeof(struct_type_), &t_struct_, sizeof(t_struct_));
    return buffer;
};

size_t SwitchModeMessage::setJson(const std::string& json){
    Mode t_struct;
    auto error = get_mode_from_json(t_struct, json);
    if(!error)
        this->t_struct_ = t_struct;
    
    return error;
};

Type SwitchModeMessage::getType() const{return Type::MODE;};

struct_variant SwitchModeMessage::getStruct() const{return struct_variant(t_struct_);};

size_t SwitchModeMessage::getBufferSize() const{return sizeof(StructType) + sizeof(Mode);};

// SensorsDataMessage
SensorsDataMessage::SensorsDataMessage(const SensorsData& t_struct) : t_struct_(t_struct) {};

Type SensorsDataMessage::getType() const {return Type::SENSORS_DATA;};

struct_variant SensorsDataMessage::getStruct() const{return struct_variant(t_struct_);};

size_t SensorsDataMessage::getBufferSize() const{return sizeof(StructType) + sizeof(SensorsData);};

// FullSensorsDataMessage
FullSensorsDataMessage::FullSensorsDataMessage(const FullSensorsData& t_struct) : t_struct_(t_struct){};

std::string FullSensorsDataMessage::getJson() const {
    return full_sensors_data_to_json(t_struct_);
};

Type FullSensorsDataMessage::getType() const {return Type::SENSORS_DATA;};

struct_variant FullSensorsDataMessage::getStruct() const{return struct_variant(t_struct_);};

size_t FullSensorsDataMessage::getBufferSize() const{return sizeof(StructType) + sizeof(FullSensorsData);};

// class FromDBMessage : public BaseMessage
// {
// public:
//     FromDBMessage(const size_t& count, const std::vector<std::vector<std::string>>& result);
//     struct_variant getStruct() const override;

// private:
//     size_t count_;
//     std::vector<std::vector<std::string>> result_;
// };
// FromDBMessage
FromDBMessage::FromDBMessage(const size_t& count, const std::vector<std::vector<std::string>>& result):
    count_(count),
    result_(result){};

struct_variant FromDBMessage::getStruct() const{return struct_variant(DBResult(count_, result_));};

Type FromDBMessage::getType() const {return SUCCESS;};