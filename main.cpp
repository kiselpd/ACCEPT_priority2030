#include <iostream>
#include <thread>

#include "db_backend.h"
#include "boost/asio.hpp"

#include "acceptor.h"
#include "session_collection.h"
#include "auth_system.h"

int main(){
    std::shared_ptr<boost::asio::io_service> io = std::make_shared<boost::asio::io_service>();
    std::shared_ptr<SessionCollection> collection = std::make_shared<SessionCollection>();
    std::shared_ptr<DBConnectionPool> pool = std::make_shared<DBConnectionPool>(io);
    pool->open("127.0.0.1", 4000);
    
    std::shared_ptr<EspAuthSystem> esp_a = std::make_shared<EspAuthSystem>(pool);
    std::shared_ptr<ClientAuthSystem> client_a = std::make_shared<ClientAuthSystem>(pool);

    std::shared_ptr<SessionManager> esp_man = std::make_shared<SessionManager>(collection, io);
    esp_man->setAuth(esp_a);
    std::shared_ptr<SessionManager> client_man = std::make_shared<SessionManager>(collection, io);
    client_man->setAuth(client_a);
    
    std::shared_ptr<Acceptor> esp_acc = std::make_shared<Acceptor>(io, DEFAULT_ESP_ACCEPTOR_PORT);
    esp_acc->start(esp_man);

    std::shared_ptr<Acceptor> client_acc = std::make_shared<Acceptor>(io, DEFAULT_CLIENT_ACCEPTOR_PORT);
    client_acc->start(client_man);

    std::thread th(&SessionManager::startCheckSession, client_man);
    io->run();


    return 0;
};