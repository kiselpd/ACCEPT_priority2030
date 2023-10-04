#include "session_manager.h"
#include "session_mediator.h"

#include <thread>

// SessionManager
SessionManager::SessionManager(std::shared_ptr<SessionCollection> collection, std::shared_ptr<boost::asio::io_service> io_service) :
    collection_(collection),
    io_service_(io_service),
    is_checking_(false){};

void SessionManager::setAuth(std::shared_ptr<BaseAuthSystem> auth){auth_ = auth;};

void SessionManager::createSession(std::shared_ptr<boost::asio::ip::tcp::socket> new_socket){
    if(auth_){
        auto new_session = auth_->authorizeNewSession(new_socket);
        if(new_session){
            auto session_it = collection_->findSession(new_session->getId(), new_session->getOwner());
            if(session_it != collection_->end())
                collection_->removeSession(session_it);

            collection_->addSession(new_session);

            auto friend_session_it = collection_->findSession(new_session->getId(), new_session->getFriend());
            if(friend_session_it != collection_->end()){
                std::cout << "link mediator" << std::endl;
                std::shared_ptr<SessionMediator> mediator = std::make_shared<SessionMediator>();
                new_session->linkMediator(mediator);
                (*friend_session_it)->linkMediator(mediator);
            }
            auth_->start(new_session, new_socket, io_service_);
        }
    }
};

void SessionManager::startCheckSession(){
    is_checking_.store(true, std::memory_order_acquire);

    while (is_checking_.load(std::memory_order_relaxed)){
        collection_->erasePassiveSession();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }       
};

void SessionManager::stopCheckSession(){
    is_checking_.store(false, std::memory_order_acquire);
};