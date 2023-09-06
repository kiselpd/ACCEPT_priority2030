#include "db_backend.h"

// class DBBackend : public NotificationListener
// {
// public:
//     DBBackend(std::shared_ptr<DBConnectionPool> pool);

//     void getNotification(const Notification& notification) override;
//     Addressee getName() const override;
    
//     void doRequest(std::shared_ptr<DBBaseRequest> request);
    
// private:
//     void on_send_(const std::string& str_request);
//     void on_read_();

//     void send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);
//     void read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);

//     std::vector<char> buffer_;
//     const size_t buffer_size_ = 2048; 
    
//     std::shared_ptr<DBConnection> booked_connection_;
//     std::shared_ptr<DBConnectionPool> pool_;
// };

// DBBackend
DBBackend::DBBackend(std::shared_ptr<DBConnectionPool> pool) : pool_(pool){};

void DBBackend::getNotification(const Notification& notification){
    //Распарсить запрос
    // this->doRequest();
};

Addressee DBBackend::getName() const {return Addressee::Database;};

void DBBackend::doRequest(std::shared_ptr<DBBaseRequest> request){
    if(std::string str_request = request->createRequest(); !str_request.empty())
        this->on_send_(str_request);
};

void DBBackend::on_send_(const std::string& str_request){
    this->booked_connection_ = pool_->getFreeConnection();
    
    auto send_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->send_handler_(error, bytes_transferred);
    };

    (booked_connection_->getSocket()).async_send(
        boost::asio::buffer(str_request),
        send_handler
    );
};

void DBBackend::on_read_(){
    buffer_.clear();
    buffer_.resize(this->buffer_size_, 0);

    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_handler_(error, bytes_transferred);
    };
    
    (booked_connection_->getSocket()).async_read_some(
        boost::asio::buffer(buffer_),
        read_handler
    );
};

void DBBackend::send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(!error && bytes_transferred)
        this->on_read_();
    else
        this->booked_connection_.reset();
};

void DBBackend::read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(!error && bytes_transferred)
        pool_->setFreeConnection(std::move(booked_connection_));
        // this->notifyDispatcher();
    else
        this->booked_connection_.reset();
};