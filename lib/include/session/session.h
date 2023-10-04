#ifndef SESSION_H
#define SESSION_H

#include "session_mediator.h"

#include "messenger.h"

#include "notification_observer.h"

#include "db_backend.h"

class BaseSession : public std::enable_shared_from_this<BaseSession>
{
public:
    BaseSession(const std::string& id);

    virtual User getOwner() const = 0;
    virtual User getFriend() const = 0;

    virtual void linkMediator(std::shared_ptr<SessionMediator> mediator) = 0;

    void addListener(std::shared_ptr<NotificationListener> listener); 
    void removeListener(const Addressee& addressee);

    bool getStatus() const;
    std::shared_ptr<std::atomic<bool>> getSharedStatus();

    std::string getId() const;  

protected:
    std::shared_ptr<NotificationDispatcher> dispatcher_;
    std::shared_ptr<std::atomic<bool>> is_stopped_;

private:
    std::string id_;
};


class EspSession : public BaseSession
{
public:
    EspSession(const std::string& id);
    ~EspSession(){std::cout << "esp session destructor" << std::endl;};
   
    User getOwner() const override;
    User getFriend() const override;

    void linkMediator(std::shared_ptr<SessionMediator> mediator) final;
};


class ClientSession : public BaseSession
{
public:
    ClientSession(const std::string& id);
    ~ClientSession(){std::cout << "client session destructor" << std::endl;};

    User getOwner() const override;
    User getFriend() const override;

    void linkMediator(std::shared_ptr<SessionMediator> mediator) final;
};

#endif /*SESSION_H*/