#include "session.h"
#include "monitoring_system.h"

#include "boost/format.hpp"
#include "boost/date_time.hpp"

// BaseSession
BaseSession::BaseSession(const std::string &id) : id_(id),
                                                  is_stopped_(std::make_shared<std::atomic<bool>>(false)),
                                                  dispatcher_(std::make_shared<NotificationDispatcher>()){};

void BaseSession::addListener(std::shared_ptr<NotificationListener> listener)
{
    dispatcher_->attachListener(listener);
    listener->attachDispatcher(dispatcher_);
};

void BaseSession::removeListener(const Addressee &addressee)
{
    dispatcher_->detachListener(addressee);
};

bool BaseSession::getStatus() const
{
    return !(is_stopped_->load(std::memory_order_acquire));
};

std::shared_ptr<std::atomic<bool>> BaseSession::getSharedStatus()
{
    return is_stopped_;
};

std::string BaseSession::getId() const { return id_; };

// EspSession
EspSession::EspSession(const std::string &id) : BaseSession(id){};

User EspSession::getOwner() const { return User::Esp; };

User EspSession::getFriend() const { return User::Client; };

void EspSession::linkMediator(std::shared_ptr<SessionMediator> mediator)
{
    auto colleague = std::make_shared<EspColleague>(mediator);
    mediator->registerColleague(colleague);

    dispatcher_->detachListener(Addressee::Mediator);
    dispatcher_->attachListener(colleague);
};

// ClientSession
ClientSession::ClientSession(const std::string &id) : BaseSession(id){};

User ClientSession::getOwner() const { return User::Client; };

User ClientSession::getFriend() const { return User::Esp; };

void ClientSession::linkMediator(std::shared_ptr<SessionMediator> mediator)
{
    auto colleague = std::make_shared<ClientColleague>(mediator);
    mediator->registerColleague(colleague);

    dispatcher_->detachListener(Addressee::Mediator);
    dispatcher_->attachListener(colleague);
};