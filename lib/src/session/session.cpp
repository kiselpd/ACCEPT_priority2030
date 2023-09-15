#include "session.h"
#include "monitoring_system.h"

#include "boost/format.hpp"
#include "boost/date_time.hpp"

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
    auto ms = std::make_shared<EspMonitoringSystem>();
    dispatcher_->attachListener(ms);
    ms->attachDispatcher(dispatcher_);
};

User EspSession::getOwner() const {return User::Esp;};

User EspSession::getFriend() const {return User::Client;};

void EspSession::linkMediator(std::shared_ptr<SessionMediator> mediator){
    std::shared_ptr<EspColleague> colleague = std::make_shared<EspColleague>(mediator);
    mediator->registerColleague(colleague);
    dispatcher_->attachListener(colleague);
    colleague->attachDispatcher(dispatcher_);
};

void EspSession::start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer){
    std::shared_ptr<EspMessenger> messenger = std::make_shared<EspMessenger>(new_socket, timer);
    messenger->start(is_stopped_);
    dispatcher_->attachListener(messenger);
    messenger->attachDispatcher(dispatcher_);
    on_preparation_();
};

void EspSession::on_preparation_(){
    on_energy_();
};

void EspSession::on_energy_(){
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_source = "predicted_power";

    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    
    auto option = boost::format(
        "user_id = '%1%' AND date_stamp ='%2%' AND date_period='%3%'") % this->getId() % today % 3;
    request->_option = option.str();

    dispatcher_->notifyListener(Notification(Addressee::Database, BaseMessage(request)));
};

// ClientSession
ClientSession::ClientSession(const int& id) : BaseSession(id){
    auto ms = std::make_shared<ClientMonitiringSystem>();
    dispatcher_->attachListener(ms);
    ms->attachDispatcher(dispatcher_);
};

User ClientSession::getOwner() const {return User::Client;};

User ClientSession::getFriend() const {return User::Esp;};

void ClientSession::linkMediator(std::shared_ptr<SessionMediator> mediator){
    std::shared_ptr<ClientColleague> colleague = std::make_shared<ClientColleague>(mediator);
    mediator->registerColleague(colleague);
    dispatcher_->attachListener(colleague);
    colleague->attachDispatcher(dispatcher_);
};

void ClientSession::start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer){
    std::shared_ptr<ClientMessenger> messenger = std::make_shared<ClientMessenger>(new_socket, timer);
    messenger->start(is_stopped_);
    dispatcher_->attachListener(messenger);
    messenger->attachDispatcher(dispatcher_);
    on_preparation_();
};

void ClientSession::on_preparation_(){
    on_consumers_();
    on_energy_();
};

void ClientSession::on_consumers_(){
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_source = "consumer";
    auto option = boost::format(
        "user_id = '%1%'") % this->getId();
    request->_option = option.str();

    dispatcher_->notifyListener(Notification(Addressee::Database, BaseMessage(request)));
};

void ClientSession::on_energy_(){

};