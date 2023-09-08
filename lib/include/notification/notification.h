#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <memory>
#include <mutex>
#include <variant>

#include "esp_message.h"
#include "client_message.h"
#include "db_message.h"

enum Addressee
{
    Mediator,
    Monitoring_System,
    Database,
    Messenger
};

typedef std::variant<std::shared_ptr<EspBaseMessage>, std::shared_ptr<ClientBaseMessage>, std::shared_ptr<DBBaseMessage>> BaseMessage;

enum BaseMessageIndex
{
    Esp = 0,
    Client,
    DB
};

class Notification
{
public:
    Notification();   
    Notification(const Addressee& addressee, BaseMessage message);
    
    void set(const Addressee& addressee, BaseMessage message);
    Addressee getAddressee() const;
    BaseMessage getMessage() const;

private:
    Addressee addressee_;
    BaseMessage message_;
};

#endif /*NOTIFICATION_H*/