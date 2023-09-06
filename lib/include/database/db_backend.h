#ifndef BACKEND_H
#define BACKEND_H

#include <vector>

#include "notification_observer.h"

#include "db_pool.h"
#include "db_message.h"

class DBBackend : public NotificationListener
{
public:
    DBBackend(std::shared_ptr<DBConnectionPool> pool);

    void getNotification(const Notification& notification) override;
    Addressee getName() const override;
    
    void doRequest(std::shared_ptr<DBBaseRequest> request);
    
private:
    void on_send_(const std::string& str_request);
    void on_read_();

    void send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);
    void read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);

    std::vector<char> buffer_;
    const size_t buffer_size_ = 2048; 
    
    std::shared_ptr<DBConnection> booked_connection_;
    std::shared_ptr<DBConnectionPool> pool_;
};

#endif /*BACKEND_H*/