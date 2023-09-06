#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <queue>
#include <memory>
#include <mutex>
#include "message.h"

enum Addressee
{
    Mediator,
    Monitoring_System,
    Database,
    Messenger
};


class Notification
{
public:
    Notification() {};   
    Notification(const Addressee& addressee, std::shared_ptr<BaseMessage> message);
    
    void set(const Addressee& addressee, std::shared_ptr<BaseMessage> message);
    Addressee getAddressee() const;
    std::shared_ptr<BaseMessage> getMessage() const;

private:
    Addressee addressee_;
    std::shared_ptr<BaseMessage> message_;
};

#endif /*NOTIFICATION_H*/