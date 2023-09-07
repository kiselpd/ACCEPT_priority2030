#ifndef MESSAGE_H
#define MESSAGE_H

#include "additional.h"

// Esp
class EspBaseMessage
{
public:
    virtual ~EspBaseMessage() = default;

    virtual char* getBuffer() const = 0;
    virtual EspStructType getType() const = 0;
    virtual size_t getBufferSize() const = 0;

    virtual char* setBuffer() = 0;
};


class EspTypeMessage : public EspBaseMessage
{
public:
    EspTypeMessage();
    EspTypeMessage(const EspStructType& type);

    char* getBuffer() const override;
    EspStructType getType() const override;

private:
    EspStructType type_;
};


class EspAuthMessage : public EspBaseMessage
{
public:
    EspAuthMessage();
    EspAuthMessage(const EspStructType& type);

    char* getBuffer() const override;
    EspStructType getType() const override;

private:
    AuthSize t_struct_;
};


class EspSensorsMessage : public EspBaseMessage
{

};


class EspModeMessage : public EspBaseMessage
{

};



#endif /*MESSAGE_H*/