#ifndef MESSENGER_H
#define MESSENGER_H

#include "boost/asio.hpp"

#include "notification_observer.h"

#include "message_buffer.h"

const size_t DEFAULT_ESP_WAITING_IN_SECOND = 15;
const size_t DEFAULT_CLIENT_WAITING_IN_SECOND = 10;

class BaseMessenger : public NotificationListener
{
public:
    void start(std::shared_ptr<std::atomic<bool>> is_stopped);
    void stop();

    void getNotification(const Notification& notification) override;
    Addressee getName() const override;

private:
    virtual void on_start_() = 0;
    virtual void on_send_(const BaseMessage& base_message) = 0;

    std::shared_ptr<std::atomic<bool>> is_stopped_;
};


class EspMessenger : public BaseMessenger
{
public:
    EspMessenger(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::deadline_timer> timer, const size_t& waiting_in_second = DEFAULT_ESP_WAITING_IN_SECOND);

private:
    void on_start_() override;
    void on_send_(const BaseMessage& base_message) override;

    void on_read_type_();
    void on_read_struct_();
    void on_wait_();
    void on_process_();

    void wait_handler_(const boost::system::error_code& error);
    void read_type_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);
    void read_struct_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);
    void send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::shared_ptr<boost::asio::deadline_timer> timer_;

    size_t waiting_in_second_;

    EspMessageBuffer buffer_;
};


class ClientMessenger : public BaseMessenger
{
public:
    ClientMessenger(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::deadline_timer> timer, const size_t& waiting_in_second = DEFAULT_CLIENT_WAITING_IN_SECOND);

private:
    void on_start_() override;
    void on_send_(const BaseMessage& message) override;

    void on_read_();
    void on_wait_();
    void on_process_();
    void on_ping_();

    void wait_handler_(const boost::system::error_code& error);
    void read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);
    void send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::shared_ptr<boost::asio::deadline_timer> timer_;

    size_t waiting_in_second_;
    std::atomic<bool> is_active_;

    ClientMessageBuffer buffer_;
};

#endif /*MESSENGER_H*/