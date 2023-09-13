#ifndef AUTH_SYSTEM_H
#define AUTH_SYSTEM_H

#include "session.h"
#include "db_backend.h"

#include "messenger.h"

class BaseAuthSystem
{
public:
    virtual std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;
};


class EspAuthSystem : public BaseAuthSystem
{
public:
    EspAuthSystem(std::shared_ptr<DBConnectionPool> pool);
    std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) override;

private:
    std::shared_ptr<DBConnectionPool> pool_;
    std::shared_ptr<DBSyncBackend> db_;
};


class ClientAuthSystem : public BaseAuthSystem
{
public:
    ClientAuthSystem(std::shared_ptr<DBConnectionPool> pool);
    std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) override;

private:
    size_t reciveRequest_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<ClientBaseMessage>& message);
    std::shared_ptr<DBSelectRequest> createRequestToDB_(const Auth& auth_struct);

    size_t writeAnswer_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const StructType& type);

    std::shared_ptr<DBConnectionPool> pool_;
    std::shared_ptr<DBSyncBackend> db_;
};

#endif /*AUTH_SYSTEM_H*/