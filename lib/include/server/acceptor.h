
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "boost/asio.hpp"
#include "session_manager.h"

const short DEFAULT_ESP_ACCEPTOR_PORT = 2000;
const short DEFAULT_CLIENT_ACCEPTOR_PORT = 3000;

class Acceptor
{
public:
    Acceptor(std::shared_ptr<boost::asio::io_service> io_service, const short& port);
    ~Acceptor();

    void start(std::shared_ptr<SessionManager> session_manager);
    void stop();

private:
    void do_accept_();
    void accept_handler_(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, const boost::system::error_code& error);

    std::shared_ptr<boost::asio::io_service> io_service_;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::shared_ptr<SessionManager> session_manager_;
};

#endif /*ACCEPTOR_H*/