#ifndef SESSION_MEDIATOR_H
#define SESSION_MEDIATOR_H

#include <memory>
#include "notification.h"
#include "notification_observer.h"

enum ColleagueName
{
    EsP,
    ClienT
};

class BaseColleague;


class SessionMediator
{
public:
    ~SessionMediator();

    void registerColleague(std::shared_ptr<BaseColleague> colleague);
    void unregisterColleague(ColleagueName colleague_type);
    void send(const Notification& notification, ColleagueName colleague_type);

private:
    std::weak_ptr<BaseColleague> esp_;
    std::weak_ptr<BaseColleague> client_;
};


class BaseColleague : public NotificationListener
{
public:
    void getNotification(const Notification& notification) override;
    Addressee getName() const override;

    virtual void send(const Notification& notification) = 0;
    virtual void recive(const Notification& notification) = 0;
    virtual ColleagueName getColleagueName() const = 0;
};


class EspColleague : public BaseColleague
{
public:
    EspColleague(std::shared_ptr<SessionMediator>  mediator);
    
    void send(const Notification& notification) override;
    void recive(const Notification& notification) override;
    ColleagueName getColleagueName() const override;

private:
    std::shared_ptr<SessionMediator>  mediator_;
};


class ClientColleague : public BaseColleague
{
public:
    ClientColleague(std::shared_ptr<SessionMediator>  mediator);

    void send(const Notification& notification) override;
    void recive(const Notification& notification) override;
    ColleagueName getColleagueName() const override;

private:
    std::shared_ptr<SessionMediator>  mediator_;
};

#endif /*SESSION_MEDIATOR_H*/