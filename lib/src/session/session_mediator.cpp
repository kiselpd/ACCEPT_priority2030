#include "session_mediator.h"

// SessionMediator
SessionMediator::~SessionMediator(){
    this->unregisterColleague(User::Esp);
    this->unregisterColleague(User::Client);
};

void SessionMediator::registerColleague(std::shared_ptr<BaseColleague> colleague){
    if(colleague->getColleagueName() == User::Esp)
        this->esp_ = colleague;
    if(colleague->getColleagueName() == User::Client)
        this->client_ = colleague;
};

void SessionMediator::unregisterColleague(const User& colleague_type){
    if(colleague_type == User::Esp)
        this->esp_.reset();
    if(colleague_type == User::Client)
        this->client_.reset();
};

void SessionMediator::send(const Notification& notification, const User& colleague_type){
    auto shared_esp = esp_.lock();
    auto shared_client = client_.lock();
    if((colleague_type == User::Esp) && shared_esp)
    {
        shared_esp->recive(notification);
    }
    if((colleague_type == User::Client) && shared_client)
    {
        shared_client->recive(notification);
    }
};

// BaseColleague
Addressee BaseColleague::getName() const{
    return Addressee::Mediator;
};

// EspColleague
EspColleague::EspColleague(std::shared_ptr<SessionMediator>  mediator) : mediator_(mediator) {};

void EspColleague::getNotification(const Notification& notification){
    this->send(Notification{Addressee::Messenger, notification.getMessage()});
};

void EspColleague::send(const Notification& notification){
    mediator_->send(notification, User::Client);
};

void EspColleague::recive(const Notification& notification){
    this->notifyDispatcher(notification);
};

User EspColleague::getColleagueName() const{
    return User::Esp;
};

// ClientColleague
ClientColleague::ClientColleague(std::shared_ptr<SessionMediator>  mediator) : mediator_(mediator) {};

void ClientColleague::getNotification(const Notification& notification){
    this->send(Notification{Addressee::Monitoring_System, notification.getMessage()});
};

void ClientColleague::send(const Notification& notification){
    mediator_->send(notification, User::Esp);
};

void ClientColleague::recive(const Notification& notification){
    this->notifyDispatcher(Notification(Addressee::Messenger, notification.getMessage()));
};

User ClientColleague::getColleagueName() const{
    return User::Client;
};