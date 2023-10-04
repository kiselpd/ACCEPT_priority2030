
#ifndef SERVER_H
#define SERVER_H

#include "boost/asio.hpp"

#include "acceptor.h"
#include "session_manager.h"
#include "db_pool.h"

class Server
{
public:
    Server();

    bool connectToDB(const std::string& ip, const short& db_port);
    void start(const short& esp_port, const short& client_port);
    void stop();

private:
    void startAcceptEsp_(const short& esp_port);
    void startAcceptClient_(const short& client_port);

    std::shared_ptr<boost::asio::io_service> io_service_;

    std::shared_ptr<Acceptor> esp_acceptor_;
    std::shared_ptr<Acceptor> client_acceptor_;

    std::shared_ptr<SessionCollection> collection_;

    std::shared_ptr<DBConnectionPool> pool_;
};

#endif /*SERVER_H*/