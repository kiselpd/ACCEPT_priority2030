#include "auth_system.h"


// EspAuthSystem
EspAuthSystem::EspAuthSystem(std::shared_ptr<DBBackend> db) : db_(db){};

std::shared_ptr<BaseSession> EspAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    // boost::asio::streambuf stream_buffer;

    // boost::asio::read_until(
    //     (*socket),
    //     stream_buffer,
    //     '\n'
    // );

    // TypeMessage message(Type::SUCCESS);
    // socket->write_some(
    //     message.getJson()
    // );

    std::shared_ptr<BaseSession> session = std::make_shared<EspSession>(1);
    session->linkDatabase(db_);
    return session;
};


// ClientAuthSystem
ClientAuthSystem::ClientAuthSystem(std::shared_ptr<DBBackend> db) : db_(db){};

std::shared_ptr<BaseSession> ClientAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    boost::asio::streambuf stream_buffer;

    boost::asio::read_until(
        (*socket),
        stream_buffer,
        '\n'
    );

    TypeMessage message(Type::SUCCESS);
    socket->write_some(
        boost::asio::buffer(message.getJson())
    );

    std::cout << "send: " << message.getJson() << std::endl;
    std::shared_ptr<BaseSession> session = std::make_shared<ClientSession>(1);
    session->linkDatabase(db_);
    std::cout << "client auth" << std::endl;
    return session;
};