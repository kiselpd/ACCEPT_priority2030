#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H

#include "additional.hpp"
#include <variant>

typedef std::variant<Datagram, Auth, FullSensors, Mode, SwitchRelay, Consumers> client_struct;

class ClientBaseMessage
{
public:
    virtual ~ClientBaseMessage() = default;

    virtual std::string getJson() const = 0;
    virtual client_struct getStruct() const = 0;
    virtual StructType getType() const = 0;

    virtual size_t setJson(const std::string& json) = 0;
};


class ClientDatagramMessage : public ClientBaseMessage
{
public:
    ClientDatagramMessage();
    ClientDatagramMessage(const Datagram& t_struct);

    std::string getJson() const override;
    client_struct getStruct() const override;
    StructType getType() const override;

    size_t setJson(const std::string& json) override;

private:
    Datagram t_struct_;
};


class ClientAuthMessage : public ClientBaseMessage
{
public:
    ClientAuthMessage();
    ClientAuthMessage(const Auth& t_struct);

    std::string getJson() const override;
    client_struct getStruct() const override;
    StructType getType() const override;

    size_t setJson(const std::string& json) override;

private:
    Auth t_struct_;
};


class ClientSensorsMessage : public ClientBaseMessage
{
public:
    ClientSensorsMessage();
    ClientSensorsMessage(const FullSensors& t_struct);

    std::string getJson() const override;
    client_struct getStruct() const override;
    StructType getType() const override;

    size_t setJson(const std::string& json) override;

private:
    FullSensors t_struct_;
};


class ClientModeMessage : public ClientBaseMessage
{
public:
    ClientModeMessage();
    ClientModeMessage(const Mode& t_struct);

    std::string getJson() const override;
    client_struct getStruct() const override;
    StructType getType() const override;

    size_t setJson(const std::string& json) override;

private:
    Mode t_struct_;
};


class ClientSwitchRelayMessage : public ClientBaseMessage
{
public:
    ClientSwitchRelayMessage();
    ClientSwitchRelayMessage(const SwitchRelay& t_struct);

    std::string getJson() const override;
    client_struct getStruct() const override;
    StructType getType() const override;

    size_t setJson(const std::string& json) override;

private:
    SwitchRelay t_struct_;
};


class ClientConsumersMessage : public ClientBaseMessage
{
public:
    ClientConsumersMessage();
    ClientConsumersMessage(const Consumers& t_struct);

    std::string getJson() const override;
    client_struct getStruct() const override;
    StructType getType() const override;

    size_t setJson(const std::string& json) override;

private:
    Consumers t_struct_;
};

#endif /*CLIENT_MESSAGE_H*/