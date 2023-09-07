#include "messenger.h"
#include "parser.h"

// BaseMessenger
void BaseMessenger::start(std::shared_ptr<std::atomic<bool>> is_stopped){
    is_stopped_ = is_stopped;
    on_start_(); 
};

void BaseMessenger::stop(){
    std::cout<<"messenger stop" << std::endl;
    is_stopped_->store(true, std::memory_order_release); 
};

void BaseMessenger::getNotification(const Notification& notification){
    if(auto message = notification.getMessage(); message){
        std::cout << "on send" << std::endl;
        on_send_(message);
    }
};

Addressee BaseMessenger::getName() const {return Addressee::Messenger;};

// EspMessenger
EspMessenger::EspMessenger(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::deadline_timer> timer, const size_t& waiting_in_second) :
    socket_(socket),
    timer_(timer),
    waiting_in_second_(waiting_in_second){};

void EspMessenger::on_start_(){
    on_wait_();
    on_read_type_();
};

void EspMessenger::on_read_type_(){
    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_type_handler_(error, bytes_transferred);
    };
    
    boost::asio::async_read(
        *socket_,
        boost::asio::buffer(buffer_.getTypeBuffer(), 8),
        read_handler
    );
};

void EspMessenger::on_read_struct_(){
    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_struct_handler_(error, bytes_transferred);
    };
    
    boost::asio::async_read(
        *socket_,
        boost::asio::buffer(buffer_.getStructBuffer(), buffer_.getSize()),
        read_handler
    );
};

void EspMessenger::on_wait_(){
    auto wait_handler = [this](const boost::system::error_code& error){
        this->wait_handler_(error);
    };
    timer_->expires_from_now(boost::posix_time::seconds(waiting_in_second_));
    timer_->async_wait(wait_handler);
};

void EspMessenger::on_send_(std::shared_ptr<BaseMessage> message){
    auto buffer = message->getBuffer();
    auto send_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->send_handler_(error, bytes_transferred);
    };

    std::cout << "send to esp" << std::endl;
    socket_->async_write_some(
        boost::asio::buffer(buffer.get(), message->getBufferSize()),
        send_handler
    );
};

void EspMessenger::on_process_(){
    switch (buffer_.getType())
    {
    case Type::SENSORS_DATA:
    {
        std::shared_ptr<char[]> struct_buffer{new char[sizeof(SensorsData)]};
        std::memcpy(struct_buffer.get(), buffer_.getStructBuffer(), sizeof(SensorsData));
        SensorsData* t_struct = reinterpret_cast<SensorsData*>(struct_buffer.get());
        this->notifyDispatcher(Notification{Addressee::Monitoring_System, std::make_shared<SensorsDataMessage>(*t_struct)});
        break;
    }
    case Type::SUCCESS:
    {
        std::shared_ptr<char[]> struct_buffer{new char[sizeof(StructType)]};
        std::memcpy(struct_buffer.get(), buffer_.getStructBuffer(), sizeof(StructType));
        StructType* t_struct = reinterpret_cast<StructType*>(struct_buffer.get());
        this->notifyDispatcher(Notification{Addressee::Monitoring_System, std::make_shared<TypeMessage>((Type)t_struct->type)});
        break;
    }
    case Type::ERROR:
    {
        std::shared_ptr<char[]> struct_buffer{new char[sizeof(StructType)]};
        std::memcpy(struct_buffer.get(), buffer_.getStructBuffer(), sizeof(StructType));
        StructType* t_struct = reinterpret_cast<StructType*>(struct_buffer.get());
        this->notifyDispatcher(Notification{Addressee::Monitoring_System, std::make_shared<TypeMessage>((Type)t_struct->type)});
        break;
    }
    default:
        break;
    }
};

void EspMessenger::wait_handler_(const boost::system::error_code& error){
    if(!error){
        socket_->close();
        this->stop();
    }
};

void EspMessenger::read_type_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    timer_->cancel();
    std::cout << "bytes trans" << bytes_transferred << std::endl;
    if(!error && bytes_transferred){
        if(buffer_.convertTypeBuffer()){
            this->on_wait_();
            this->on_read_struct_();
        }
        else{
            this->on_process_();
            this->on_start_();
        }
    }
    else{
        socket_->close();
        this->stop();
    }
};

void EspMessenger::read_struct_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    timer_->cancel();
    std::cout << "bytes trans" << bytes_transferred << std::endl;
    if(!error && bytes_transferred){
        this->on_process_();
        this->on_start_();
    }
    else{
        socket_->close();
        this->stop();
    }
};

void EspMessenger::send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(error || !bytes_transferred){
        socket_->close();
        timer_->cancel();
        this->stop();
    }
};

// ClientMessenger
ClientMessenger::ClientMessenger(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::deadline_timer> timer, const size_t& waiting_in_second) :
    socket_(socket),
    timer_(timer),
    waiting_in_second_(waiting_in_second){};

void ClientMessenger::on_start_(){
    on_wait_();
    on_read_();
};

void ClientMessenger::on_send_(std::shared_ptr<BaseMessage> message){
    auto send_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->send_handler_(error, bytes_transferred);
    };

    std::cout << message->getJson() << std::endl;
    socket_->async_write_some(
        boost::asio::buffer(message->getJson()),
        send_handler
    );
};

void ClientMessenger::on_read_(){
    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_handler_(error, bytes_transferred);
    };
    
    socket_->async_read_some(
        boost::asio::buffer(buffer_.get()),
        read_handler
    );
};

void ClientMessenger::on_wait_(){
    auto wait_handler = [this](const boost::system::error_code& error){
        this->wait_handler_(error);
    };
    timer_->expires_from_now(boost::posix_time::seconds(waiting_in_second_));
    timer_->async_wait(wait_handler);
};

void ClientMessenger::on_process_(){
    std::shared_ptr<BaseMessage> message;
    Addressee addressee;
    std::cout << buffer_.getString() << std::endl;
    switch (get_type_from_json(buffer_.getString()))
    {
    case Type::SWITCH_RELAY:
        message = std::make_shared<SwitchRelayMessage>();
        break;

    case Type::MODE:
        message = std::make_shared<SwitchModeMessage>();
        break;

    case Type::SHUTDOWN:
        timer_->cancel();
        socket_->cancel();
        this->stop();
        return;

    case Type::PING:
        this->on_ping_();
        return;
    default:
        break;
    }

    message->setJson(get_data_from_json(buffer_.getString()));
    this->notifyDispatcher(Notification{Addressee::Mediator, message});
    
};

void ClientMessenger::on_ping_(){
    std::shared_ptr<BaseMessage> message = std::make_shared<TypeMessage>(Type::PING);
    this->on_send_(message);
};

void ClientMessenger::wait_handler_(const boost::system::error_code& error){
    if(!error){
        socket_->close();
        this->stop();
    }
};

void ClientMessenger::read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    this->is_active_.store(true, std::memory_order_relaxed);
    timer_->cancel();
    if(!error && bytes_transferred){
        this->on_process_();
        this->on_start_();
    }
    else{
        socket_->close();
        this->stop();
    }
};

void ClientMessenger::send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    if(error || !bytes_transferred){
        socket_->close();
        timer_->cancel();
        this->stop();
    }
};
