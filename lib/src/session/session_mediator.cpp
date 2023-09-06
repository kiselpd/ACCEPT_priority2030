#include "session_mediator.h"

// SessionMediator
SessionMediator::~SessionMediator(){
    this->unregisterColleague(ColleagueName::EsP);
    this->unregisterColleague(ColleagueName::ClienT);

    std::cout << "mediator destructor" << std::endl;
};

void SessionMediator::registerColleague(std::shared_ptr<BaseColleague> colleague){
    if(colleague->getColleagueName() == ColleagueName::EsP)
        this->esp_ = colleague;
    if(colleague->getColleagueName() == ColleagueName::ClienT)
        this->client_ = colleague;
};

void SessionMediator::unregisterColleague(ColleagueName colleague_type){
    if(colleague_type == ColleagueName::EsP)
        this->esp_.reset();
    if(colleague_type == ColleagueName::ClienT)
        this->client_.reset();
};

void SessionMediator::send(const Notification& notification, ColleagueName colleague_type){
    auto shared_esp = esp_.lock();
    auto shared_client = client_.lock();
    if((colleague_type == ColleagueName::EsP) && shared_esp)
            shared_esp->recive(notification);
    if((colleague_type == ColleagueName::ClienT) && shared_client)
        shared_client->recive(notification);
};

// BaseColleague
void BaseColleague::getNotification(const Notification& notification){
    std::cout << "i am here 2" << std::endl;
    if(this->getColleagueName() == ColleagueName::EsP){
        std::cout << "i am here 3" << std::endl;
        this->send(Notification{Addressee::Messenger, notification.getMessage()});
    }
    else{
        std::cout << "i am here 4" << std::endl;
        auto message = notification.getMessage();
        if(message->getType() == Type::SWITCH_RELAY){
            std::cout << "i am here 5" << std::endl;
            this->send(Notification{Addressee::Messenger, notification.getMessage()});
        }
        if(message->getType() == Type::MODE)
            this->send(Notification{Addressee::Monitoring_System, notification.getMessage()});
    }
};

Addressee BaseColleague::getName() const{
    return Addressee::Mediator;
};

// EspColleague
EspColleague::EspColleague(std::shared_ptr<SessionMediator>  mediator) : mediator_(mediator) {};

void EspColleague::send(const Notification& notification){
    mediator_->send(notification, ColleagueName::ClienT);
};

void EspColleague::recive(const Notification& notification){
    std::cout << "I am here 7" << std::endl;
    this->notifyDispatcher(notification);
};

ColleagueName EspColleague::getColleagueName() const{
    return ColleagueName::EsP;
};

// ClientColleague
ClientColleague::ClientColleague(std::shared_ptr<SessionMediator>  mediator) : mediator_(mediator) {};

void ClientColleague::send(const Notification& notification){
    std::cout << "i am here 6" << std::endl;
    mediator_->send(notification, ColleagueName::EsP);
};

void ClientColleague::recive(const Notification& notification){
    this->notifyDispatcher(notification);
};

ColleagueName ClientColleague::getColleagueName() const{
    return ColleagueName::ClienT;
};