#include "db_backend.h"

#include "notification.h"
// DBMessageBuffer
DBMessageBuffer::DBMessageBuffer(const size_t &buffer_size)
{
    buffer_.resize(buffer_size);
};

std::vector<char> &DBMessageBuffer::get()
{
    std::fill(buffer_.begin(), buffer_.end(), 0);
    return buffer_;
};

std::string DBMessageBuffer::getString() const
{
    return std::string{buffer_.begin(), buffer_.end()};
};

size_t DBMessageBuffer::size() const
{
    return buffer_.size();
};

// DBBackend
DBBackend::DBBackend(std::shared_ptr<DBConnectionPool> pool) : pool_(pool){};

size_t DBBackend::doRequest(std::shared_ptr<DBBaseMessage> request, std::shared_ptr<DBBaseAnswer> &answer)
{
    auto error = on_send(request->createRequest());
    if (!error)
    {
        std::string str_answer;
        error = on_read(str_answer);
        std::cout << "BD answer: " << str_answer << std::endl;
        if (!error && !str_answer.empty())
        {
            answer = std::make_shared<DBBaseAnswer>(str_answer);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
};

size_t DBBackend::on_send(const std::string &str_request)
{
    booked_connection_ = pool_->getFreeConnection();
    socket_error error;

    size_t bytes_transfering_number = (booked_connection_->getSocket()).write_some(boost::asio::buffer(str_request), error);

    return (!error && bytes_transfering_number) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};

size_t DBBackend::on_read(std::string &answer)
{
    socket_error error;

    size_t bytes_transfering_number = (booked_connection_->getSocket()).read_some(boost::asio::buffer(buffer_.get()), error);

    answer = buffer_.getString();
    pool_->setFreeConnection(std::move(booked_connection_));

    return (!error && bytes_transfering_number) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};

// DBAsyncBackend
DBAsyncBackend::DBAsyncBackend(std::shared_ptr<DBConnectionPool> pool) : pool_(pool){};

void DBAsyncBackend::getNotification(const Notification& notification){
    auto base_message = notification.getMessage();
    if(base_message.index() == User::DB){
        std::shared_ptr<DBBaseMessage> base_db_message = std::get<std::shared_ptr<DBBaseMessage>>(base_message);
        this->doRequest(base_db_message);
    }
};

Addressee DBAsyncBackend::getName() const {return Addressee::Database;};

void DBAsyncBackend::doRequest(std::shared_ptr<DBBaseMessage> request){
    if(std::string str_request = request->createRequest(); !str_request.empty())
        this->on_send_(str_request);
};

void DBAsyncBackend::on_send_(const std::string& str_request){
    if(!booked_connection_)
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
    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_handler_(error, bytes_transferred);
    };
    
    (booked_connection_->getSocket()).async_read_some(
        boost::asio::buffer(buffer_.get()),
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
    if(!error && bytes_transferred){
        pool_->setFreeConnection(std::move(booked_connection_));
        std::shared_ptr<DBBaseMessage> answer = std::make_shared<DBBaseAnswer>(buffer_.getString());
        std::cout << buffer_.getString() << std::endl;
        this->notifyDispatcher(Notification{Addressee::Monitoring_System, BaseMessage(answer)});
    }
    else
        this->booked_connection_.reset();
};