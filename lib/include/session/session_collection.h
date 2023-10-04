#ifndef SESSION_COLLECTION_H
#define SESSION_COLLECTION_H

#include <vector>
#include <mutex>
#include <memory>

#include "additional.hpp"

#include "session.h"


class SessionCollection
{
public:
    SessionCollection();
    void addSession(std::shared_ptr<BaseSession> session);
    void removeSession(std::vector<std::shared_ptr<BaseSession>> :: iterator session_it);

    std::vector<std::shared_ptr<BaseSession>>::iterator findSession(std::shared_ptr<BaseSession> session);
    std::vector<std::shared_ptr<BaseSession>>::iterator findSession(const std::string& id, const User& owner);

    void erasePassiveSession();

    std::vector<std::shared_ptr<BaseSession>>::iterator begin();
    std::vector<std::shared_ptr<BaseSession>>::iterator end();

private:    
    std::shared_ptr<std::vector<std::shared_ptr<BaseSession>>> collection_;
    std::mutex mut_;
};

#endif /*SESSION_COLLECTION_H*/