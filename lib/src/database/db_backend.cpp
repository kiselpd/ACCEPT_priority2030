#include "db_backend.h"

#include "notification.h"
// DBMessageBuffer
DBMessageBuffer::DBMessageBuffer(const size_t& buffer_size){
    buffer_.resize(buffer_size);
};

std::vector<char>& DBMessageBuffer::get(){
    std::fill(buffer_.begin(), buffer_.end(), 0);
    return buffer_;
};

std::string DBMessageBuffer::getString() const{
    return std::string{buffer_.begin(), buffer_.end()};
};

size_t DBMessageBuffer::size() const{
    return buffer_.size();
};

// DBBackend
DBBackend::DBBackend(std::shared_ptr<DBConnectionPool> pool) :
    pool_(pool){};

size_t DBBackend::doRequest(std::shared_ptr<DBBaseMessage> request, std::shared_ptr<DBBaseAnswer>& answer){
    auto error = on_send(request->createRequest());
    if(!error){
        std::string str_answer;
        error = on_read(str_answer);
        std::cout << "BD answer: " << str_answer << std::endl;
        if(!error && !str_answer.empty()){
            answer = std::make_shared<DBBaseAnswer>(str_answer);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
};

size_t DBBackend::on_send(const std::string& str_request){
    booked_connection_ = pool_->getFreeConnection();
    socket_error error;

    size_t bytes_transfering_number = (booked_connection_->getSocket()).write_some(
        boost::asio::buffer(str_request),
        error
    );

    return (!error && bytes_transfering_number) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};

size_t DBBackend::on_read(std::string& answer){
    socket_error error;

    size_t bytes_transfering_number = (booked_connection_->getSocket()).read_some(
        boost::asio::buffer(buffer_.get()),
        error
    );

    answer = buffer_.getString();
    pool_->setFreeConnection(std::move(booked_connection_));

    return (!error && bytes_transfering_number) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};