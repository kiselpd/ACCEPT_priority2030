#include "auth_system.h"
#include "additional.hpp"
#include "boost/format.hpp"
#include "parser.h"


// EspAuthSystem
EspAuthSystem::EspAuthSystem(std::shared_ptr<DBConnectionPool> pool) : 
    pool_(pool),
    db_(std::make_shared<DBSyncBackend>(pool)){};

std::shared_ptr<BaseSession> EspAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    // Datagram type_struct_;
    // socket_error error;

    // std::shared_ptr<char[]> buffer(new char[sizeof(type_struct_)]);
    
    // size_t bytes_transfering_number = boost::asio::read(
    //     *socket,
    //     boost::asio::buffer(buffer.get(), sizeof(StructType)),
    //     error
    // );

    // if(!error && bytes_transfering_number){
    //     Datagram* take_struct = reinterpret_cast<Datagram*>(buffer.get());
    //     type_struct_ = *take_struct;
    // }

    // std::cout << "type: " << type_struct_.type << " " << "size: " << type_struct_.size << std::endl;

    // if(bytes_transfering_number && !error && (type_struct_.type == StructType::AUTH)){
    //     AuthSize t_struct_;
    //     std::shared_ptr<char[]> buffer1(new char[sizeof(t_struct_)]);
    //     bytes_transfering_number = boost::asio::read(
    //         *socket,
    //         boost::asio::buffer(buffer1.get(), sizeof(AuthSize)),
    //         error
    //     );

        
    //     std::cout << bytes_transfering_number << std::endl;
    //     if(!error && bytes_transfering_number){
    //         AuthSize* take_struct1 = reinterpret_cast<AuthSize*>(buffer1.get());
    //         t_struct_ = *take_struct1;
    //         std::cout << "I am here" <<  take_struct1->loginSize <<  std::endl;
    //     }

    //     std::cout << t_struct_.loginSize << " " << t_struct_.passwordSize << std::endl;
    //     std::string login;
    //     std::string password;

    //     std::shared_ptr<char[]> buffer_login(new char[t_struct_.loginSize]);

    //     bytes_transfering_number = boost::asio::read(
    //         *socket,
    //         boost::asio::buffer(buffer_login.get(), t_struct_.loginSize),
    //         error
    //     );

    //     login = std::string(buffer_login.get());

    //     std::cout << "login " << login << std::endl;

    //     std::shared_ptr<char[]> buffer_password(new char[t_struct_.passwordSize]);

    //     bytes_transfering_number = boost::asio::read(
    //         *socket,
    //         boost::asio::buffer(buffer_password.get(), t_struct_.passwordSize),
    //         error
    //     );

    //     password = std::string(buffer_password.get());

    //     std::cout << "password " << password << std::endl;

    //     std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    //     request->_source = "user_data";

    //     auto option = boost::format(
    //         "(login = '%1%' OR email = '%1%') AND password = '%2%'") % login % password;
    //     request->_option = option.str();

    //     std::shared_ptr<DBBaseAnswer> answer;

    //     size_t error_t = db_->doRequest(request, answer);
        
    //     if(!error_t && answer){
    //         auto variant = answer->getStruct();
    //         auto [count, value] = std::get<DBResult>(variant);
    //         if(count){
    //             std::cout << "count " << count;
    //             type_struct_.type = Type::SUCCESS;
    //             type_struct_.size = 0;
                
    //             std::shared_ptr<char[]> buffer_answer{new char[sizeof(type_struct_)]};
    //             std::memcpy(buffer_answer.get(), &type_struct_, sizeof(type_struct_));

    //             bytes_transfering_number = boost::asio::write(
    //                 *socket,
    //                 boost::asio::buffer(buffer_answer.get(), sizeof(type_struct_)),
    //                 error
    //             );
    //         }
    //         else{
    //             type_struct_.type = Type::ERROR;
    //             type_struct_.size = 0;

    //             bytes_transfering_number = socket->write_some(
    //                 boost::asio::buffer(&type_struct_, sizeof(type_struct_)),
    //                 error
    //             );
    //             return nullptr;
    //         }

    //     }
    // }
    // else{
    //     socket->cancel();
    //     return nullptr;
    // }

    // std::shared_ptr<BaseSession> session = std::make_shared<EspSession>(1);
    // session->linkDatabase(std::make_shared<DBAsyncBackend>(pool_));
    // return session;
};

// ClientAuthSystem
ClientAuthSystem::ClientAuthSystem(std::shared_ptr<DBConnectionPool> pool) : 
    pool_(pool),
    db_(std::make_shared<DBSyncBackend>(pool)){};

std::shared_ptr<BaseSession> ClientAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket){
    std::shared_ptr<BaseSession> session;
    ClientMessageBuffer buffer_;
    socket_error error;

    size_t bytes_transfering_number = socket->read_some(
        boost::asio::buffer(buffer_.get()),
        error
    );

    if(!error){
        std::pair<int, std::string> info;
        auto parser_error = get_from_json(info, buffer_.getString());
        if(!parser_error && info.first == StructType::AUTH){
            std::shared_ptr<ClientBaseMessage> message = std::make_shared<ClientAuthMessage>();
            parser_error = message->setJson(info.second);
            if(!parser_error){
                Auth auth_struct = std::get<Auth>(message->getStruct());
                std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
                request->_source = "user_data";
                auto option = boost::format(
                    "(login = '%1%' OR email = '%1%') AND password = '%2%'") % auth_struct.login % auth_struct.password;
                request->_option = option.str();

                std::shared_ptr<DBBaseAnswer> answer;
                size_t error_t = db_->doRequest(request, answer);
                auto answer_tuple = answer->getAnswer();
                if(std::get<0>(answer_tuple)){
                    message = std::make_shared<ClientDatagramMessage>(Datagram{StructType::SUCCESS, 0});
                    session = std::make_shared<ClientSession>(1);
                    session->linkDatabase(std::make_shared<DBAsyncBackend>(pool_));
                    std::cout << "client auth" << std::endl;
                }
                else{
                    message = std::make_shared<ClientDatagramMessage>(Datagram{StructType::ERROR, 0});
                }

                socket->write_some(
                    boost::asio::buffer(message->getJson())
                );
            }
        }
    }
    
    return session;
};