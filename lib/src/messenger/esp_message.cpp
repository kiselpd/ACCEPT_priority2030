#include "esp_message.h"

// EspDatagramMessage
EspDatagramMessage::EspDatagramMessage(){};

EspDatagramMessage::EspDatagramMessage(const Datagram &t_struct) : t_struct_(t_struct){};

std::shared_ptr<char[]> EspDatagramMessage::getBuffer() const
{
    return get_buffer(t_struct_);
};

esp_struct EspDatagramMessage::getStruct() const
{
    return esp_struct(t_struct_);
};

StructType EspDatagramMessage::getType() const
{
    return StructType::DATAGRAM;
};

size_t EspDatagramMessage::getSize() const
{
    return sizeof(Datagram);
};

void EspDatagramMessage::setBuffer(std::shared_ptr<char[]> buffer)
{
    t_struct_ = get_struct<Datagram>(buffer);
};

// EspAuthMessage
EspAuthMessage::EspAuthMessage(){};

EspAuthMessage::EspAuthMessage(const AuthSize &t_struct) : t_struct_(t_struct){};

std::shared_ptr<char[]> EspAuthMessage::getBuffer() const
{
    return get_buffer(this->getType(), t_struct_);
};

esp_struct EspAuthMessage::getStruct() const
{
    return esp_struct(t_struct_);
};

StructType EspAuthMessage::getType() const
{
    return StructType::AUTH;
};

size_t EspAuthMessage::getSize() const
{
    return sizeof(AuthSize);
};

void EspAuthMessage::setBuffer(std::shared_ptr<char[]> buffer)
{
    t_struct_ = get_struct<AuthSize>(buffer);
};

// EspSensorsMessage
EspSensorsMessage::EspSensorsMessage(){};

EspSensorsMessage::EspSensorsMessage(const Sensors &t_struct) : t_struct_(t_struct){};

std::shared_ptr<char[]> EspSensorsMessage::getBuffer() const
{
    return get_buffer(this->getType(), t_struct_);
};

esp_struct EspSensorsMessage::getStruct() const
{
    return esp_struct(t_struct_);
};

StructType EspSensorsMessage::getType() const
{
    return StructType::SENSORS_DATA;
};

size_t EspSensorsMessage::getSize() const
{
    return sizeof(Sensors);
};

void EspSensorsMessage::setBuffer(std::shared_ptr<char[]> buffer)
{
    t_struct_ = get_struct<Sensors>(buffer);
};

// EspModeMessage
EspModeMessage::EspModeMessage(){};

EspModeMessage::EspModeMessage(const Mode &t_struct) : t_struct_(t_struct){};

std::shared_ptr<char[]> EspModeMessage::getBuffer() const
{
    return get_buffer(this->getType(), t_struct_);
};

esp_struct EspModeMessage::getStruct() const
{
    return esp_struct(t_struct_);
};

StructType EspModeMessage::getType() const
{
    return StructType::MODE;
};

size_t EspModeMessage::getSize() const
{
    return sizeof(Mode);
};

void EspModeMessage::setBuffer(std::shared_ptr<char[]> buffer)
{
    t_struct_ = get_struct<Mode>(buffer);
};

// EspSwitchRelayMessage
EspSwitchRelayMessage::EspSwitchRelayMessage(){};

EspSwitchRelayMessage::EspSwitchRelayMessage(const SwitchRelay &t_struct) : t_struct_(t_struct){};

std::shared_ptr<char[]> EspSwitchRelayMessage::getBuffer() const
{
    return get_buffer(this->getType(), t_struct_);
};

esp_struct EspSwitchRelayMessage::getStruct() const
{
    return esp_struct(t_struct_);
};

StructType EspSwitchRelayMessage::getType() const
{
    return StructType::SWITCH_RELAY;
};

size_t EspSwitchRelayMessage::getSize() const
{
    return sizeof(SwitchRelay);
};

void EspSwitchRelayMessage::setBuffer(std::shared_ptr<char[]> buffer)
{
    t_struct_ = get_struct<SwitchRelay>(buffer);
};