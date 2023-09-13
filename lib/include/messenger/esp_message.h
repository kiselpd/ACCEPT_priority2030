#ifndef ESP_MESSAGE_H
#define ESP_MESSAGE_H

#include "additional.hpp"
#include <variant>

typedef std::variant<Datagram, AuthSize, Sensors, Mode, SwitchRelay> esp_struct;

class EspBaseMessage
{
public:
    virtual ~EspBaseMessage() = default;

    virtual std::shared_ptr<char[]> getBuffer() const = 0;
    virtual esp_struct getStruct() const = 0;

    virtual StructType getType() const = 0;
    virtual size_t getSize() const = 0;

    virtual void setBuffer(std::shared_ptr<char[]> buffer) = 0;
};


class EspDatagramMessage : public EspBaseMessage
{
public:
    EspDatagramMessage();
    EspDatagramMessage(const Datagram& t_struct);

    std::shared_ptr<char[]> getBuffer() const override;
    esp_struct getStruct() const override;

    StructType getType() const override;
    size_t getSize() const override;

    void setBuffer(std::shared_ptr<char[]> buffer) override;

private:
    Datagram t_struct_;
};


class EspAuthMessage : public EspBaseMessage
{
public:
    EspAuthMessage();
    EspAuthMessage(const AuthSize& t_struct);

    std::shared_ptr<char[]> getBuffer() const override;
    esp_struct getStruct() const override;

    StructType getType() const override;
    size_t getSize() const override;

    void setBuffer(std::shared_ptr<char[]> buffer) override;

private:
    AuthSize t_struct_;
};


class EspSensorsMessage : public EspBaseMessage
{
public:
    EspSensorsMessage();
    EspSensorsMessage(const Sensors& t_struct);

    std::shared_ptr<char[]> getBuffer() const override;
    esp_struct getStruct() const override;

    StructType getType() const override;
    size_t getSize() const override;

    void setBuffer(std::shared_ptr<char[]> buffer) override;

private:
    Sensors t_struct_;
};


class EspModeMessage : public EspBaseMessage
{
public:
    EspModeMessage();
    EspModeMessage(const Mode& t_struct);

    std::shared_ptr<char[]> getBuffer() const override;
    esp_struct getStruct() const override;

    StructType getType() const override;
    size_t getSize() const override;

    void setBuffer(std::shared_ptr<char[]> buffer) override;

private:
    Mode t_struct_;
};


class EspSwitchRelayMessage : public EspBaseMessage
{
public:
    EspSwitchRelayMessage();
    EspSwitchRelayMessage(const SwitchRelay& t_struct);

    std::shared_ptr<char[]> getBuffer() const override;
    esp_struct getStruct() const override;

    StructType getType() const override;
    size_t getSize() const override;

    void setBuffer(std::shared_ptr<char[]> buffer) override;

private:
    SwitchRelay t_struct_;
};

#endif /*ESP_MESSAGE_H*/