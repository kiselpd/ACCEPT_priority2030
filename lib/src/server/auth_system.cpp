#include "auth_system.h"
#include "additional.hpp"
#include "boost/format.hpp"
#include "parser.h"
#include "monitoring_system.h"

// EspAuthSystem
EspAuthSystem::EspAuthSystem(std::shared_ptr<DBConnectionPool> pool) : db_(std::make_shared<DBBackend>(pool)),
                                                                       pool_(pool){};

void EspAuthSystem::start(std::shared_ptr<BaseSession> session, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::io_service> io)
{
    auto monitoring = std::make_shared<EspMonitoringSystem>(session->getId());
    session->addListener(monitoring);
    auto db = std::make_shared<DBAsyncBackend>(pool_);
    session->addListener(db);
    auto messenger = std::make_shared<EspMessenger>(socket, std::make_shared<boost::asio::deadline_timer>(*io));
    session->addListener(messenger);

    messenger->start(session->getSharedStatus());
    monitoring->getPreparatoryData();
};

Datagram EspAuthSystem::on_read_type_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, size_t &error)
{
    Datagram type_struct_;
    error = 1;

    try
    {
        socket_error sock_error;

        std::shared_ptr<char[]> buffer(new char[sizeof(type_struct_)]);

        size_t bytes_transfering_number = boost::asio::read(
            *socket,
            boost::asio::buffer(buffer.get(), sizeof(Datagram)),
            sock_error);

        if (!sock_error && bytes_transfering_number)
        {
            Datagram *take_struct = reinterpret_cast<Datagram *>(buffer.get());
            type_struct_ = *take_struct;
            error = 0;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return type_struct_;
};

AuthSize EspAuthSystem::on_read_auth_size_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, size_t &error)
{
    AuthSize t_struct_;
    error = 1;

    try
    {
        socket_error sock_error;

        std::shared_ptr<char[]> buffer(new char[sizeof(t_struct_)]);

        size_t bytes_transfering_number = boost::asio::read(
            *socket,
            boost::asio::buffer(buffer.get(), sizeof(AuthSize)),
            sock_error);

        std::cout << bytes_transfering_number << std::endl;
        if (!sock_error && bytes_transfering_number)
        {
            AuthSize *take_struct1 = reinterpret_cast<AuthSize *>(buffer.get());
            t_struct_ = *take_struct1;
            error = 0;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return t_struct_;
};

std::string EspAuthSystem::on_read_login(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const size_t &login_size)
{
    std::shared_ptr<char[]> buffer_login(new char[login_size]);
    socket_error sock_error;
    std::string login;

    size_t bytes_transfering_number = boost::asio::read(
        *socket,
        boost::asio::buffer(buffer_login.get(), login_size),
        sock_error);

    if (!sock_error && bytes_transfering_number)
        login = buffer_login.get();

    return login;
};

std::string EspAuthSystem::on_read_password(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const size_t &password_size)
{
    std::shared_ptr<char[]> buffer_password(new char[password_size]);
    socket_error sock_error;
    std::string password;

    size_t bytes_transfering_number = boost::asio::read(
        *socket,
        boost::asio::buffer(buffer_password.get(), password_size),
        sock_error);

    if (!sock_error && bytes_transfering_number)
        password = buffer_password.get();
        password.pop_back();

    return password;
};

std::string EspAuthSystem::on_process_(const std::string &login, const std::string &password)
{
    std::shared_ptr<DBAuthRequest> request = std::make_shared<DBAuthRequest>();
    request->_login = login;
    request->_password = password;

    std::shared_ptr<DBBaseAnswer> answer;
    auto error = db_->doRequest(request, answer);

    if (!error && answer)
        if (answer->getStatus() == 200)
        {
            auto [val_count, val] = answer->getAnswer();
            return val[0][0];
        }

    return std::string();
};

size_t EspAuthSystem::on_send_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const StructType &type)
{
    std::shared_ptr<EspDatagramMessage> mes = std::make_shared<EspDatagramMessage>(Datagram{type, 0});
    socket_error sock_error;

    size_t bytes_transfering_number = socket->write_some(
        boost::asio::buffer((mes->getBuffer()).get(), mes->getSize()),
        sock_error);

    return (sock_error) ? (EXIT_FAILURE) : (EXIT_SUCCESS);
};

std::shared_ptr<BaseSession> EspAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::shared_ptr<BaseSession> session;
    size_t error = 0;

    auto datagram = this->on_read_type_(socket, error);

    if (!error)
    {
        auto auth_size = this->on_read_auth_size_(socket, error);

        if (!error)
        {
            auto login = this->on_read_login(socket, auth_size.loginSize);
            auto password = this->on_read_password(socket, auth_size.passwordSize);

            std::cout << "login size: " << auth_size.loginSize << "password size: " << auth_size.passwordSize << std::endl;

            if (!(login.empty() || password.empty()))
            {
                auto token = this->on_process_(login, password);

                if (!token.empty())
                {
                    error = this->on_send_(socket, StructType::SUCCESS);
                    if (!error)
                        session = std::make_shared<EspSession>(token);
                }
                else
                    error = this->on_send_(socket, StructType::ERROR);
            }
        }
    }

    return session;
};

// ClientAuthSystem
ClientAuthSystem::ClientAuthSystem(std::shared_ptr<DBConnectionPool> pool) : db_(std::make_shared<DBBackend>(pool)),
                                                                             pool_(pool){};

void ClientAuthSystem::start(std::shared_ptr<BaseSession> session, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<boost::asio::io_service> io)
{
    auto monitoring = std::make_shared<ClientMonitiringSystem>(session->getId());
    session->addListener(monitoring);
    auto db = std::make_shared<DBAsyncBackend>(pool_);
    session->addListener(db);
    auto messenger = std::make_shared<ClientMessenger>(socket, std::make_shared<boost::asio::deadline_timer>(*io));
    session->addListener(messenger);

    messenger->start(session->getSharedStatus());
    monitoring->getPreparatoryData();
};

std::shared_ptr<BaseSession> ClientAuthSystem::authorizeNewSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::shared_ptr<BaseSession> session;
    size_t error = 0;
    std::shared_ptr<ClientBaseMessage> client_request;

    error = this->reciveRequest_(socket, client_request);

    if (!error)
    {
        auto auth = std::get<Auth>(client_request->getStruct());
        auto token = this->processRequest(auth.login, auth.password);

        if (!token.empty())
        {
            error = this->writeAnswer_(socket, StructType::SUCCESS);

            if (!error)
                session = std::make_shared<ClientSession>(token);
        }
        else
            error = this->writeAnswer_(socket, StructType::ERROR);
    }
    return session;
};

std::string ClientAuthSystem::processRequest(const std::string &login, const std::string &password)
{
    std::shared_ptr<DBAuthRequest> request = std::make_shared<DBAuthRequest>();
    request->_login = login;
    request->_password = password;

    std::shared_ptr<DBBaseAnswer> answer;
    auto error = db_->doRequest(request, answer);

    if (!error && answer)
        if (answer->getStatus() == 200)
        {
            auto [val_count, val] = answer->getAnswer();
            return val[0][0];
        }

    return std::string();
};

size_t ClientAuthSystem::writeAnswer_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const StructType &type)
{
    std::unique_ptr<ClientDatagramMessage> message = std::make_unique<ClientDatagramMessage>(Datagram{type, 0});
    socket_error error;

    size_t bytes_transfering_number = socket->write_some(
        boost::asio::buffer(message->getJson()),
        error);

    return (bytes_transfering_number && !error) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};

size_t ClientAuthSystem::reciveRequest_(std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::shared_ptr<ClientBaseMessage> &message)
{
    ClientMessageBuffer buffer_;
    socket_error error;
    size_t parser_error;

    size_t bytes_transfering_number = socket->read_some(
        boost::asio::buffer(buffer_.get()),
        error);

    if (!error && bytes_transfering_number)
    {
        std::pair<int, std::string> info;
        parser_error = get_from_json(info, buffer_.getString());
        std::cout << buffer_.getString() << std::endl;
        if (!parser_error)
        {
            message = std::make_shared<ClientAuthMessage>();
            parser_error = message->setJson(info.second);
            std::cout << "auth req: " <<  message->getJson() << std::endl;
        }
    }

    return (bytes_transfering_number && !error && !parser_error) ? (EXIT_SUCCESS) : (EXIT_FAILURE);
};