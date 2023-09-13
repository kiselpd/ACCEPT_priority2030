#ifndef SESSION_H
#define SESSION_H

#include "session_mediator.h"

#include "messenger.h"

#include "notification_observer.h"

#include "db_backend.h"

class BaseSession : public std::enable_shared_from_this<BaseSession>
{
public:
    BaseSession(const int& id);

    virtual User getOwner() const = 0;
    virtual User getFriend() const = 0;

    virtual void linkMediator(std::shared_ptr<SessionMediator> mediator) = 0;
    void unlinkMediator();
    void linkDatabase(std::shared_ptr<DBAsyncBackend> db);
    

    virtual void start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer) = 0;
    void stop();
    bool getStatus() const;

    int getId() const;  

protected:
    std::shared_ptr<NotificationDispatcher> dispatcher_;
    std::shared_ptr<std::atomic<bool>> is_stopped_;

private:
    int id_;
};


class EspSession : public BaseSession
{
public:
    EspSession(const int& id);
    ~EspSession(){std::cout << "esp session destructor" << std::endl;};
   
    void linkMediator(std::shared_ptr<SessionMediator> mediator) override;

    void start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer) override;

    User getOwner() const override;
    User getFriend() const override;

private:
    void on_preparation_();
    void on_energy_();
};


class ClientSession : public BaseSession
{
public:
    ClientSession(const int& id);
    ~ClientSession(){std::cout << "client session destructor" << std::endl;};

    void linkMediator(std::shared_ptr<SessionMediator> mediator) override;

    void start(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket, std::shared_ptr<boost::asio::deadline_timer> timer) override;

    User getOwner() const override;
    User getFriend() const override;

private:
    void on_preparation_();

    void on_consumers_();
    void on_energy_();
};

#endif /*SESSION_H*/