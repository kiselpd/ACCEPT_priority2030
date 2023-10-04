#include "server.h"
#include <thread>

// Server
Server::Server() : io_service_(std::make_shared<boost::asio::io_service>()),
                   collection_(std::make_shared<SessionCollection>()),
                   pool_(std::make_shared<DBConnectionPool>(io_service_)){};

bool Server::connectToDB(const std::string &ip, const short &db_port)
{
    return pool_->open(ip, db_port);
};

void Server::start(const short &esp_port, const short &client_port)
{
    this->startAcceptClient_(client_port);
    this->startAcceptEsp_(esp_port);
    io_service_->run();
};

void Server::stop()
{
    io_service_->stop();
};

void Server::startAcceptEsp_(const short &esp_port)
{
    auto auth = std::make_shared<EspAuthSystem>(pool_);
    auto esp_manager = std::make_shared<SessionManager>(collection_, io_service_);
    esp_manager->setAuth(auth);
    esp_acceptor_ = std::make_shared<Acceptor>(io_service_, esp_port);

    std::thread th(&Acceptor::start, esp_acceptor_, esp_manager);
    th.detach();
};

void Server::startAcceptClient_(const short &client_port)
{
    auto auth = std::make_shared<ClientAuthSystem>(pool_);
    auto client_manager = std::make_shared<SessionManager>(collection_, io_service_);
    client_manager->setAuth(auth);
    client_acceptor_ = std::make_shared<Acceptor>(io_service_, client_port);

    std::thread th1(&Acceptor::start, client_acceptor_, client_manager);
    th1.detach();

    std::thread th2(&SessionManager::startCheckSession, client_manager);
    th2.detach();
};
