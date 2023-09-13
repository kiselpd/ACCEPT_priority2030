#include "messenger.h"
#include "parser.h"

// BaseMessenger
void BaseMessenger::start(std::shared_ptr<std::atomic<bool>> is_stopped){
    is_stopped_ = is_stopped;
    on_start_(); 
};

void BaseMessenger::stop(){
    is_stopped_->store(true, std::memory_order_release); 
};

void BaseMessenger::getNotification(const Notification& notification){
    auto base_message = notification.getMessage();
    this->on_send_(base_message);
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

void EspMessenger::on_send_(BaseMessage base_message){
    if(base_message.index() == User::Esp){
        std::shared_ptr<EspBaseMessage> esp_base_message = std::get<std::shared_ptr<EspBaseMessage>>(base_message);
        auto buffer = esp_base_message->getBuffer();
        auto send_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            this->send_handler_(error, bytes_transferred);
        };

        socket_->async_write_some(
            boost::asio::buffer(buffer.get(), sizeof(Datagram) + esp_base_message->getSize()),
            send_handler
        );
    }
};

void EspMessenger::on_process_(){
    std::shared_ptr<EspBaseMessage> esp_base_message;
    switch (buffer_.getType())
    {
    case StructType::SENSORS_DATA:
        esp_base_message = std::make_shared<EspSensorsMessage>();
        break;
    case StructType::MODE:
        esp_base_message = std::make_shared<EspModeMessage>();
        break;
    default:
        return;
    }
    esp_base_message->setBuffer(buffer_.getStructSharedBuffer());
    this->notifyDispatcher(Notification{Addressee::Monitoring_System, BaseMessage(esp_base_message)});
};

void EspMessenger::wait_handler_(const boost::system::error_code& error){
    if(!error){
        socket_->close();
        this->stop();
    }
};

void EspMessenger::read_type_handler_(const boost::system::error_code& error, std::size_t bytes_transferred){
    timer_->cancel();
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

void ClientMessenger::on_send_(BaseMessage base_message){
    if(base_message.index() == User::Client){
        std::shared_ptr<ClientBaseMessage> client_base_message = std::get<std::shared_ptr<ClientBaseMessage>>(base_message);
        auto send_handler = [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            this->send_handler_(error, bytes_transferred);
        };

        std::cout << client_base_message->getJson() << std::endl;
        socket_->async_write_some(
            boost::asio::buffer(client_base_message->getJson()),
            send_handler
        );
    }
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
    std::pair<int, std::string> info;
    auto error = get_from_json(info, buffer_.getString());
    if(!error){
        std::shared_ptr<ClientBaseMessage> client_base_message;

        switch (info.first)
        {
        case StructType::SWITCH_RELAY:
            client_base_message = std::make_shared<ClientSwitchRelayMessage>();            
            break;
        case StructType::MODE:
            client_base_message = std::make_shared<ClientModeMessage>();  
            break;
        case StructType::SHUTDOWN:
            timer_->cancel();
            socket_->cancel();
            this->stop();
            return;

        case StructType::PING:
            this->on_ping_();
            return;
        default:
            break;
        }
        client_base_message->setJson(info.second);
        this->notifyDispatcher(Notification{Addressee::Mediator, BaseMessage(client_base_message)});
    }
};

void ClientMessenger::on_ping_(){
    std::shared_ptr<ClientBaseMessage> client_base_message = std::make_shared<ClientDatagramMessage>(Datagram{StructType::PING, 0});
    this->on_send_(BaseMessage(client_base_message));
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
