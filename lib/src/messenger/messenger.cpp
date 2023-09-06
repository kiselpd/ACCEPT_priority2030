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

// class EspMessenger : public BaseMessenger
// {
// public:
//     EspMessenger(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::deadline_timer> timer, const size_t& waiting_in_second = DEFAULT_ESP_WAITING_IN_SECOND);

// private:
//     void on_start_() override;
//     void on_send_(std::shared_ptr<BaseMessage> message) override;

//     void on_read_();
//     void on_wait_();
//     void on_process_();

//     void wait_handler_(const boost::system::error_code& error);
//     void read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);
//     void send_handler_(const boost::system::error_code& error, std::size_t bytes_transferred);

//     std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
//     std::shared_ptr<boost::asio::deadline_timer> timer_;

//     size_t waiting_in_second_;

//     Type type;
//     const size_t buffer_size_ = 2048; 
//     char buffer_[2048]{0};
// };

// EspMessenger
EspMessenger::EspMessenger(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::deadline_timer> timer, const size_t& waiting_in_second) :
    socket_(socket),
    timer_(timer),
    waiting_in_second_(waiting_in_second){};

void EspMessenger::on_start_(){
    on_wait_();
    on_read_();
};

void EspMessenger::on_read_(){
    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_handler_(error, bytes_transferred);
    };
    
    boost::asio::async_read(
        *socket_,
        boost::asio::buffer(buffer_, 160),
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
    std::shared_ptr<char[]> type_buffer{new char[sizeof(StructType)]};
    std::memcpy(type_buffer.get(), &buffer_, sizeof(StructType));
    StructType* t_struct = reinterpret_cast<StructType*>(type_buffer.get());

    std::cout << t_struct->type << " " << t_struct->size << std::endl;
    char *r = &(buffer_[8]);
    if(t_struct->type == Type::SENSORS_DATA){
        std::cout << "Get sensors data" << std::endl;
        std::shared_ptr<char[]> struct_buffer{new char[sizeof(SensorsData)]};
        std::memcpy(struct_buffer.get(), r, sizeof(SensorsData));
        SensorsData* str = reinterpret_cast<SensorsData*>(struct_buffer.get());
        std::cout << str->battery_voltage << std::endl;
        this->notifyDispatcher(Notification{Addressee::Monitoring_System, std::make_shared<SensorsDataMessage>(*str)});

    }
    std::fill(std::begin(buffer_), std::end(buffer_), 0);
};

void EspMessenger::wait_handler_(const boost::system::error_code& error){
    if(!error){
        socket_->close();
        this->stop();
    }
};

void EspMessenger::read_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
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
    buffer_.clear();
    buffer_.resize(this->buffer_size_);
    auto read_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
        this->read_handler_(error, bytes_transferred);
    };
    
    socket_->async_read_some(
        boost::asio::buffer(buffer_),
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
    std::string json_str(buffer_.begin(), buffer_.end());
    std::shared_ptr<BaseMessage> message;
    Addressee addressee;

    switch (get_type_from_json(json_str))
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
        break;
    
    default:
        break;
    }

    message->setJson(get_data_from_json(json_str));
    this->notifyDispatcher(Notification{Addressee::Mediator, message});
    
};

void ClientMessenger::on_ping_(){
    std::shared_ptr<BaseMessage> message = std::make_shared<TypeMessage>(Type::PING);
    std::cout << "on ping" << std::endl;
    this->on_send_(message);
};

void ClientMessenger::wait_handler_(const boost::system::error_code& error){
    if(!error){
        if(this->is_active_.load(std::memory_order_relaxed)){
            this->is_active_.store(false, std::memory_order_relaxed);
            this->on_wait_();
            this->on_ping_();
            std::cout << "wait on ping" << std::endl;
        }
        else{
            socket_->close();
            std::cout << "wait on stop" << std::endl;
            this->stop();
        }
    }
    else
        std::cout << "not ping" << std::endl;
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
