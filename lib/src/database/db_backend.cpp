#include "db_backend.h"

// DBAsyncBackend
DBAsyncBackend::DBAsyncBackend(std::shared_ptr<DBConnectionPool> pool) : pool_(pool){};

void DBAsyncBackend::getNotification(const Notification& notification){
    //Распарсить запрос
    // this->doRequest();
};

Addressee DBAsyncBackend::getName() const {return Addressee::Database;};

void DBAsyncBackend::doRequest(std::shared_ptr<DBBaseRequest> request){
    if(std::string str_request = request->createRequest(); !str_request.empty())
        this->on_send_(str_request);
};

void DBAsyncBackend::on_send_(const std::string& str_request){
    this->booked_connection_ = pool_->getFreeConnection();
    
    auto send_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->send_handler_(error, bytes_transferred);
    };

    (booked_connection_->getSocket()).async_send(
        boost::asio::buffer(str_request),
        send_handler
    );
};

void DBAsyncBackend::on_read_(){
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

void DBAsyncBackend::send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(!error && bytes_transferred)
        this->on_read_();
    else
        this->booked_connection_.reset();
};

void DBAsyncBackend::read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(!error && bytes_transferred)
        pool_->setFreeConnection(std::move(booked_connection_));
        // this->notifyDispatcher();
    else
        this->booked_connection_.reset();
};

// DBSyncBackend
DBSyncBackend::DBSyncBackend(std::shared_ptr<DBConnectionPool> pool) :
    pool_(pool){};

size_t DBSyncBackend::doRequest(std::shared_ptr<DBBaseRequest> request, std::shared_ptr<FromDBMessage>& result){
    auto error = on_send(request->createRequest());
    if(!error){
        std::string answer;
        error = on_read(answer);
        std::cout << "BD answer: " << answer << std::endl;
        if(!error && !answer.empty()){
            auto [status, db_answer] = parseAnswer(answer);
            result = std::make_shared<FromDBMessage>(db_answer._count, db_answer._value);
            return EXIT_SUCCESS;
        }
        else
            return EXIT_FAILURE;
    }
    else
        return EXIT_FAILURE;
};

size_t DBSyncBackend::on_send(const std::string& str_request){
    booked_connection_ = pool_->getFreeConnection();

    socket_error error;

    size_t bytes_transfering_number = (booked_connection_->getSocket()).write_some(
        boost::asio::buffer(str_request),
        error
    );

    return (!error && bytes_transfering_number) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};

size_t DBSyncBackend::on_read(std::string& answer){
    socket_error error;
    buffer_.clear();
    buffer_.resize(buffer_size_);

    size_t bytes_transfering_number = (booked_connection_->getSocket()).read_some(
        boost::asio::buffer(buffer_),
        error
    );

    answer = std::string(buffer_.begin(), buffer_.end());
    std::cout << "BD answer: " << bytes_transfering_number << " " << answer << std::endl;

    pool_->setFreeConnection(booked_connection_);
    booked_connection_.reset();

    return (!error && bytes_transfering_number) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};