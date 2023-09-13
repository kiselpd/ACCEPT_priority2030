#ifndef SESSION_MEDIATOR_H
#define SESSION_MEDIATOR_H

#include <memory>

#include "additional.hpp"

#include "notification.h"
#include "notification_observer.h"

class BaseColleague;


class SessionMediator
{
public:
    ~SessionMediator();

    void registerColleague(std::shared_ptr<BaseColleague> colleague);
    void unregisterColleague(const User& colleague_type);
    void send(const Notification& notification, const User& colleague_type);

private:
    std::weak_ptr<BaseColleague> esp_;
    std::weak_ptr<BaseColleague> client_;
};


class BaseColleague : public NotificationListener
{
public:
    Addressee getName() const final;

    void getNotification(const Notification& notification) = 0;

    virtual void send(const Notification& notification) = 0;
    virtual void recive(const Notification& notification) = 0;
    virtual User getColleagueName() const = 0;
};


class EspColleague : public BaseColleague
{
public:
    EspColleague(std::shared_ptr<SessionMediator>  mediator);

    void getNotification(const Notification& notification) final;

    void send(const Notification& notification) final;
    void recive(const Notification& notification) final;
    User getColleagueName() const final;

private:
    std::shared_ptr<SessionMediator>  mediator_;
};


class ClientColleague : public BaseColleague
{
public:
    ClientColleague(std::shared_ptr<SessionMediator>  mediator);

    void getNotification(const Notification& notification) final;

    void send(const Notification& notification) final;
    void recive(const Notification& notification) final;
    User getColleagueName() const final;

private:
    std::shared_ptr<SessionMediator>  mediator_;
};

#endif /*SESSION_MEDIATOR_H*/