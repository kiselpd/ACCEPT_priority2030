#include "acceptor.h"
#include "boost/bind.hpp"

// Acceptor
Acceptor::Acceptor(std::shared_ptr<boost::asio::io_service> io_service, const short& port) :
    io_service_(io_service),
    acceptor_(std::make_shared<boost::asio::ip::tcp::acceptor>(*io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))){};

Acceptor::~Acceptor(){
    this->stop();
    acceptor_->cancel();
};

void Acceptor::start(std::shared_ptr<SessionManager> session_manager){
    session_manager_ = session_manager;
    this->do_accept_();
};

void Acceptor::stop(){
    acceptor_->close();
};

void Acceptor::do_accept_(){
    auto new_socket = std::make_shared<boost::asio::ip::tcp::socket>(*io_service_);

    acceptor_->async_accept(
        *new_socket,
        boost::bind(&Acceptor::accept_handler_, this, new_socket, boost::asio::placeholders::error)
    );
};

void Acceptor::accept_handler_(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, const boost::system::error_code& error){
    if(!error){
        std::cout << "newSession" << std::endl;
        session_manager_->createSession(new_socket);
    }
    else
        this->stop();
    
    this->do_accept_();
};