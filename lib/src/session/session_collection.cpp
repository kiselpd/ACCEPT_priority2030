#include <session_collection.h>
#include <algorithm>

// SessionCollection
SessionCollection::SessionCollection() :
    collection_(std::make_shared<std::vector<std::shared_ptr<BaseSession>>>()) {};

void SessionCollection::addSession(std::shared_ptr<BaseSession> session){
    std::lock_guard<std::mutex> locker(mut_);
    collection_->push_back(session);
};

void SessionCollection::removeSession(std::vector<std::shared_ptr<BaseSession>> :: iterator session_it){
    std::lock_guard<std::mutex> locker(mut_);
    collection_->erase(session_it);
};

std::vector<std::shared_ptr<BaseSession>>::iterator SessionCollection::findSession(std::shared_ptr<BaseSession> session){
    std::lock_guard<std::mutex> locker(mut_);
    auto it = collection_->begin();
    return std::find(collection_->begin(), collection_->end(), session);
};

std::vector<std::shared_ptr<BaseSession>>::iterator SessionCollection::findSession(const int& id, const User& owner){
    auto lambda = [id, owner](const std::shared_ptr<BaseSession>& session){
        return (session->getId() == id) && (session->getOwner() == owner);};

    std::lock_guard<std::mutex> locker(mut_);
    return std::find_if(collection_->begin(), collection_->end(), lambda);
};

void SessionCollection::erasePassiveSession(){
    auto lambda = [](std::shared_ptr<BaseSession> session){
        return !(session->getStatus());
    };
    
    std::lock_guard<std::mutex> locker(mut_);
    collection_->erase(std::remove_if(collection_->begin(), collection_->end(), lambda), collection_->end());
};

std::vector<std::shared_ptr<BaseSession>>::iterator SessionCollection::begin(){
    std::lock_guard<std::mutex> locker(mut_);
    return collection_->begin();
};

std::vector<std::shared_ptr<BaseSession>>::iterator SessionCollection::end(){
    std::lock_guard<std::mutex> locker(mut_);
    return collection_->end();
};