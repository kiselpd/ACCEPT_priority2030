#ifndef DB_POOL_H
#define DB_POOL_H

#include "boost/asio.hpp"

#include <memory>
#include <queue>
#include <mutex>

typedef boost::system::error_code socket_error;

class DBConnection
{
public:
    DBConnection(std::shared_ptr<boost::asio::io_service> io_service);
    ~DBConnection();

    bool open(const std::string& ip, const short& port);
    bool close();

    boost::asio::ip::tcp::socket& getSocket();

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};


const size_t DEFAULT_POOL_SIZE = 5;

class DBConnectionPool
{
public:
    DBConnectionPool(std::shared_ptr<boost::asio::io_service> io_service, const size_t& pool_size = DEFAULT_POOL_SIZE);

    size_t open(const std::string& ip, const short& port);
    void close();

    std::shared_ptr<DBConnection> getFreeConnection();
    void setFreeConnection(std::shared_ptr<DBConnection> free_connection);

private:
    size_t pool_size_;
    std::queue<std::shared_ptr<DBConnection>> pool_;
    std::condition_variable m_condition_;
    std::mutex m_mutex_;
};

#endif /*DB_POOL_H*/