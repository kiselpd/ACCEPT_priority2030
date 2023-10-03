#ifndef AUTH_SYSTEM_H
#define AUTH_SYSTEM_H

#include "session.h"
#include "db_backend.h"

#include "messenger.h"

class BaseAuthSystem
{
public:
    virtual std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;
    virtual void start(std::shared_ptr<BaseSession> session, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::io_service> io) = 0;
};

class EspAuthSystem : public BaseAuthSystem
{
public:
    EspAuthSystem(std::shared_ptr<DBConnectionPool> pool);
    std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) final;
    void start(std::shared_ptr<BaseSession> session, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::io_service> io) override;

private:
    Datagram on_read_type_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, size_t &error);
    AuthSize on_read_auth_size_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, size_t &error);
    std::string on_read_login(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const size_t &login_size);
    std::string on_read_password(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const size_t &password_size);
    std::string on_process_(const std::string &login, const std::string &password);

    size_t on_send_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const StructType &type);

    std::shared_ptr<DBBackend> db_;
    std::shared_ptr<DBConnectionPool> pool_;
};

class ClientAuthSystem : public BaseAuthSystem
{
public:
    ClientAuthSystem(std::shared_ptr<DBConnectionPool> pool);
    std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) final;
    void start(std::shared_ptr<BaseSession> session, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::io_service> io) override;

private:
    size_t reciveRequest_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<ClientBaseMessage> &message);
    std::string processRequest(const std::string &login, const std::string &password);
    size_t writeAnswer_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const StructType &type);

    std::shared_ptr<DBBackend> db_;
    std::shared_ptr<DBConnectionPool> pool_;
};

#endif /*AUTH_SYSTEM_H*/