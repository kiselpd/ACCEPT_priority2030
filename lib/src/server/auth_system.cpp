#include "auth_system.h"
#include "additional.hpp"
#include "boost/format.hpp"
#include "parser.h"


// EspAuthSystem
EspAuthSystem::EspAuthSystem(std::shared_ptr<DBConnectionPool> pool) : 
    pool_(pool),
    db_(std::make_shared<DBSyncBackend>(pool)){};

std::shared_ptr<BaseSession> EspAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    Datagram type_struct_;
    socket_error error;
    std::shared_ptr<BaseSession> session;

    std::shared_ptr<char[]> buffer(new char[sizeof(type_struct_)]);
    
    size_t bytes_transfering_number = boost::asio::read(
        *socket,
        boost::asio::buffer(buffer.get(), sizeof(Datagram)),
        error
    );

    if(!error && bytes_transfering_number){
        Datagram* take_struct = reinterpret_cast<Datagram*>(buffer.get());
        type_struct_ = *take_struct;
    }

    if(bytes_transfering_number && !error && (type_struct_.type == StructType::AUTH)){
        AuthSize t_struct_;
        std::shared_ptr<char[]> buffer1(new char[sizeof(t_struct_)]);
        bytes_transfering_number = boost::asio::read(
            *socket,
            boost::asio::buffer(buffer1.get(), sizeof(AuthSize)),
            error
        );

        
        std::cout << bytes_transfering_number << std::endl;
        if(!error && bytes_transfering_number){
            AuthSize* take_struct1 = reinterpret_cast<AuthSize*>(buffer1.get());
            t_struct_ = *take_struct1;
        }

        std::cout << t_struct_.loginSize << " " << t_struct_.passwordSize << std::endl;
        std::string login;
        std::string password;

        std::shared_ptr<char[]> buffer_login(new char[t_struct_.loginSize]);

        bytes_transfering_number = boost::asio::read(
            *socket,
            boost::asio::buffer(buffer_login.get(), t_struct_.loginSize),
            error
        );

        login = std::string(buffer_login.get());

        std::shared_ptr<char[]> buffer_password(new char[t_struct_.passwordSize]);

        bytes_transfering_number = boost::asio::read(
            *socket,
            boost::asio::buffer(buffer_password.get(), t_struct_.passwordSize),
            error
        );

        password = std::string(buffer_password.get());

        std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
        request->_source = "user_data";

        auto option = boost::format(
            "(login = '%1%' OR email = '%1%') AND password = '%2%'") % login % password;
        request->_option = option.str();

        std::shared_ptr<DBBaseAnswer> answer;
        db_->doRequest(request, answer);
        StructType type_answer;

        if(std::get<0>(answer->getAnswer())){
            type_answer = StructType::SUCCESS;
            session = std::make_shared<EspSession>(1);
            session->linkDatabase(std::make_shared<DBAsyncBackend>(pool_));
        }
        else
            type_answer = StructType::ERROR;


        std::shared_ptr<EspDatagramMessage> mes = std::make_shared<EspDatagramMessage>(Datagram{type_answer, 0});

        bytes_transfering_number = socket->write_some(
            boost::asio::buffer((mes->getBuffer()).get(), mes->getSize()),
                    error
            );
        
    }
    else{
        socket->cancel();
        return nullptr;
    }

    return session;
};

// size_t EspAuthSystem::reciveRequest_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<EspBaseMessage>& message){
//     EspMessageBuffer buffer_;
//     socket_error error;
//     size_t parser_error;

//     size_t bytes_transfering_number = socket->read_some(
//         boost::asio::buffer(buffer_.getTypeBuffer(), 8),
//         error
//     );

//     if(buffer_.convertTypeBuffer() && !error && bytes_transfering_number){
//         bytes_transfering_number = socket->read_some(
//             boost::asio::buffer(buffer_.getStructBuffer(), buffer_.getSize()),
//         error
//         );

//         if(!error && bytes_transfering_number){
//             message = std::make_shared<EspAuthMessage>()
//         }
//     }

//     if(!error && bytes_transfering_number){
//         std::pair<int, std::string> info;
//         parser_error = get_from_json(info, buffer_.getString());
//         if(!parser_error){
//             message = std::make_shared<ClientAuthMessage>();
//             parser_error = message->setJson(info.second);
//         }
//     }

//     return (bytes_transfering_number && !error && !parser_error) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
// };

// ClientAuthSystem
ClientAuthSystem::ClientAuthSystem(std::shared_ptr<DBConnectionPool> pool) : 
    pool_(pool),
    db_(std::make_shared<DBSyncBackend>(pool)){};

std::shared_ptr<BaseSession> ClientAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    std::shared_ptr<BaseSession> session;
    size_t error = 0;
    std::shared_ptr<ClientBaseMessage> client_request;

    error = this->reciveRequest_(socket, client_request);

    if(!error){
        std::shared_ptr<DBSelectRequest> db_request = this->createRequestToDB_(std::get<Auth>(client_request->getStruct()));
        std::shared_ptr<DBBaseAnswer> db_answer;
        error = db_->doRequest(db_request, db_answer);
        StructType type_answer;
        if(std::get<0>(db_answer->getAnswer())){
            type_answer = StructType::SUCCESS;
            session = std::make_shared<ClientSession>(1);
            session->linkDatabase(std::make_shared<DBAsyncBackend>(pool_));
        }
        else
            type_answer = StructType::ERROR;
                
        this->writeAnswer_(socket, type_answer);
    }
    return session;
};

std::shared_ptr<DBSelectRequest> ClientAuthSystem::createRequestToDB_(const Auth& auth_struct){
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();

    request->_source = "user_data";
    auto option = boost::format(
        "(login = '%1%' OR email = '%1%') AND password = '%2%'") % auth_struct.login % auth_struct.password;
    request->_option = option.str();

    return request;
};

size_t ClientAuthSystem::writeAnswer_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const StructType& type){
    std::unique_ptr<ClientDatagramMessage> message = std::make_unique<ClientDatagramMessage>(Datagram{type, 0});
    socket_error error;

    size_t bytes_transfering_number = socket->write_some(
        boost::asio::buffer(message->getJson()),
        error
    );

    return (bytes_transfering_number && !error) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};

size_t ClientAuthSystem::reciveRequest_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<ClientBaseMessage>& message){
    ClientMessageBuffer buffer_;
    socket_error error;
    size_t parser_error;

    size_t bytes_transfering_number = socket->read_some(
        boost::asio::buffer(buffer_.get()),
        error
    );

    if(!error && bytes_transfering_number){
        std::pair<int, std::string> info;
        parser_error = get_from_json(info, buffer_.getString());
        if(!parser_error){
            message = std::make_shared<ClientAuthMessage>();
            parser_error = message->setJson(info.second);
        }
    }

    return (bytes_transfering_number && !error && !parser_error) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};