#include "db_pool.h"

// DBConnection
DBConnection::DBConnection(std::shared_ptr<boost::asio::io_service> io_service) : socket_(std::make_shared<boost::asio::ip::tcp::socket>(*io_service)){};

DBConnection::~DBConnection()
{
    this->close();
};

bool DBConnection::open(const std::string &ip, const short &port)
{
    socket_error error;
    boost::asio::ip::tcp::endpoint en(boost::asio::ip::address::from_string(ip), port);
    socket_->connect(en, error);
    return (error) ? (false) : (true);
};

bool DBConnection::close()
{
    socket_error error;
    socket_->shutdown(boost::asio::socket_base::shutdown_both, error);
    if (!error)
        socket_->close(error);

    return (error) ? (false) : (true);
};

boost::asio::ip::tcp::socket &DBConnection::getSocket() { return *socket_; };

// DBConnectionPool
DBConnectionPool::DBConnectionPool(std::shared_ptr<boost::asio::io_service> io_service, const size_t &pool_size)
{
    pool_size_ = pool_size;
    for (size_t i = 0; i < pool_size_; i++)
        pool_.emplace(std::make_shared<DBConnection>(io_service));
};

size_t DBConnectionPool::open(const std::string &ip, const short &port)
{
    size_t tmp_pool_size = 0;

    for (size_t i = 0; i < pool_size_; i++)
    {
        auto conn = this->getFreeConnection();
        if (conn->open(ip, port))
        {
            this->setFreeConnection(conn);
            tmp_pool_size++;
        }
    }

    pool_size_ = tmp_pool_size;
    return pool_size_;
};

void DBConnectionPool::close()
{
    for (size_t i = 0; i < pool_size_; i++)
    {
        auto conn = this->getFreeConnection();
        conn->close();
    }
};

std::shared_ptr<DBConnection> DBConnectionPool::getFreeConnection()
{
    std::unique_lock locker(this->m_mutex_);
    while (this->pool_.empty())
        this->m_condition_.wait(locker);

    auto free_connection = this->pool_.front();
    this->pool_.pop();

    return free_connection;
};

void DBConnectionPool::setFreeConnection(std::shared_ptr<DBConnection> free_connection)
{
    std::unique_lock locker(this->m_mutex_);
    this->pool_.push(free_connection);
    locker.unlock();
    m_condition_.notify_one();
};
