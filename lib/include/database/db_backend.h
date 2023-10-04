#ifndef BACKEND_H
#define BACKEND_H

#include <vector>

#include "db_pool.h"
#include "db_message.h"

#include "notification_observer.h"

const size_t DEFAULT_DB_BUFFER_SIZE = 2048;

class DBMessageBuffer
{
public:
    DBMessageBuffer(const size_t &buffer_size = DEFAULT_DB_BUFFER_SIZE);

    std::vector<char> &get();

    std::string getString() const;
    size_t size() const;

private:
    std::vector<char> buffer_;
};

class DBBackend
{
public:
    DBBackend(std::shared_ptr<DBConnectionPool> pool);

    size_t doRequest(std::shared_ptr<DBBaseMessage> request, std::shared_ptr<DBBaseAnswer> &answer);

private:
    size_t on_send(const std::string &str_request);
    size_t on_read(std::string &answer);

    DBMessageBuffer buffer_;

    std::shared_ptr<DBConnection> booked_connection_;
    std::shared_ptr<DBConnectionPool> pool_;
};

class DBAsyncBackend : public NotificationListener
{
public:
    DBAsyncBackend(std::shared_ptr<DBConnectionPool> pool);

    void getNotification(const Notification &notification) override;
    Addressee getName() const override;

    void doRequest(std::shared_ptr<DBBaseMessage> request);

private:
    void on_send_(const std::string &str_request);
    void on_read_();

    void send_handler_(const boost::system::error_code &error, std::size_t bytes_transferred);
    void read_handler_(const boost::system::error_code &error, std::size_t bytes_transferred);

    DBMessageBuffer buffer_;

    std::shared_ptr<DBConnection> booked_connection_;
    std::shared_ptr<DBConnectionPool> pool_;

    std::mutex mut_;
};

#endif /*BACKEND_H*/