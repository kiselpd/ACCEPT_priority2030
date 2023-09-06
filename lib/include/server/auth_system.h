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
    EspAuthSystem(std::shared_ptr<DBBackend> db);
    std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) override;

private:
    std::shared_ptr<DBBackend> db_;
};


class ClientAuthSystem : public BaseAuthSystem
{
public:
    ClientAuthSystem(std::shared_ptr<DBBackend> db);
    std::shared_ptr<BaseSession> authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket) override;

private:
    std::shared_ptr<DBBackend> db_;
};

#endif /*AUTH_SYSTEM_H*/