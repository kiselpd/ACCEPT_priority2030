#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "boost/asio.hpp"
#include "session_collection.h"
#include "session.h"
#include "auth_system.h"


class SessionManager : public std::enable_shared_from_this<SessionManager>
{
public:
    SessionManager(std::shared_ptr<SessionCollection> collection, std::shared_ptr<boost::asio::io_service> io_service);
    void setAuth(std::shared_ptr<BaseAuthSystem> auth);
    
    void createSession(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket);

    void startCheckSession();
    void stopCheckSession();

private:
    std::atomic<bool> is_checking_;
    std::shared_ptr<boost::asio::io_service> io_service_;

    std::shared_ptr<BaseAuthSystem> auth_;
    std::shared_ptr<SessionCollection> collection_; 
};

#endif /*SESSION_MANAGER_H*/