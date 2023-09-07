#include "session.h"
#include "monitoring_system.h"

// BaseSession
BaseSession::BaseSession(const int& id) :
    id_(id),
    is_stopped_(std::make_shared<std::atomic<bool>>(false)),
    dispatcher_(std::make_shared<NotificationDispatcher>()){};

void BaseSession::unlinkMediator(){
    dispatcher_->detachListener(Addressee::Mediator);
};

void BaseSession::linkDatabase(std::shared_ptr<DBAsyncBackend> db){
    dispatcher_->attachListener(db);
    db->attachDispatcher(dispatcher_);
};

void BaseSession::stop(){

};

bool BaseSession::getStatus() const{
    return !(is_stopped_->load(std::memory_order_acquire));
};

int BaseSession::getId() const{return id_;};

// EspSession
EspSession::EspSession(const int& id) : BaseSession(id){
    auto ms = std::make_shared<MonitoringSystem>();
    dispatcher_->attachListener(ms);
    ms->attachDispatcher(dispatcher_);
};

SessionOwner EspSession::getOwner() const {return SessionOwner::Esp;};

SessionOwner EspSession::getFriend() const {return SessionOwner::Client;};

void EspSession::linkMediator(std::shared_ptr<SessionMediator> mediator){
    std::shared_ptr<EspColleague> colleague = std::make_shared<EspColleague>(mediator);
    mediator->registerColleague(colleague);
    std::cout << "esp mediator: " << mediator.get() << std::endl;
    dispatcher_->attachListener(colleague);
    colleague->attachDispatcher(dispatcher_);
};

void EspSession::start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer){
    std::shared_ptr<EspMessenger> messenger = std::make_shared<EspMessenger>(new_socket, timer);
    messenger->start(is_stopped_);
    dispatcher_->attachListener(messenger);
    messenger->attachDispatcher(dispatcher_);
};

// ClientSession
ClientSession::ClientSession(const int& id) : BaseSession(id){
    // std::shared_ptr<MonitoringSystem> monitor = std::make_shared<MonitoringSystem>();
    // dispatcher_->attachListener(monitor);
};

SessionOwner ClientSession::getOwner() const {return SessionOwner::Client;};

SessionOwner ClientSession::getFriend() const {return SessionOwner::Esp;};

void ClientSession::linkMediator(std::shared_ptr<SessionMediator> mediator){
    std::shared_ptr<ClientColleague> colleague = std::make_shared<ClientColleague>(mediator);
    mediator->registerColleague(colleague);
    std::cout << "client mediator: " << mediator.get() << std::endl;
    dispatcher_->attachListener(colleague);
    colleague->attachDispatcher(dispatcher_);
};

void ClientSession::start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer){
    std::shared_ptr<ClientMessenger> messenger = std::make_shared<ClientMessenger>(new_socket, timer);
    messenger->start(is_stopped_);
    dispatcher_->attachListener(messenger);
    messenger->attachDispatcher(dispatcher_);
};