#ifndef SESSION_COLLECTION_H
#define SESSION_COLLECTION_H

#include "session.h"
#include <vector>
#include <mutex>
#include <memory>

class SessionCollection
{
public:
    SessionCollection();
    void addSession(std::shared_ptr<BaseSession> session);
    void removeSession(std::vector<std::shared_ptr<BaseSession>> :: iterator session_it);

    std::vector<std::shared_ptr<BaseSession>>::iterator findSession(std::shared_ptr<BaseSession> session);
    std::vector<std::shared_ptr<BaseSession>>::iterator findSession(const int& id, const SessionOwner& owner);

    void erasePassiveSession();

    std::vector<std::shared_ptr<BaseSession>>::iterator begin();
    std::vector<std::shared_ptr<BaseSession>>::iterator end();

private:    
    std::shared_ptr<std::vector<std::shared_ptr<BaseSession>>> collection_;
    std::mutex mut_;
};

#endif /*SESSION_COLLECTION_H*/