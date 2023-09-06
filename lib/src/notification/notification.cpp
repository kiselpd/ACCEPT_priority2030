#include "notification.h"

// Notification
Notification::Notification(const Addressee& addressee, std::shared_ptr<BaseMessage> message) : addressee_(addressee), message_(message) {};

void Notification::set(const Addressee& addressee, std::shared_ptr<BaseMessage> message){
    this->addressee_ = addressee;
    this->message_ = message;
};

Addressee Notification::getAddressee() const{
    return this->addressee_;
};

std::shared_ptr<BaseMessage> Notification::getMessage() const{
    return this->message_;
};

// // NotificationQueue
// void NotificationQueue::pushNotification(const Notification& notification)
// {
//     std::lock_guard<std::mutex> guard(this->mtx_);
// 	queue_.push(notification);
// }

// Notification NotificationQueue::popNotification()
// {
//     std::lock_guard<std::mutex> guard(this->mtx_);
//     Notification request;

//     if(!(this->queue_.empty()))
//     {
//         request = queue_.front();
// 	    queue_.pop();
//     }

//     return request;
// }

// bool NotificationQueue::isEmpty()
// {
// 	std::lock_guard<std::mutex> guard(this->mtx_);
// 	return queue_.empty();
// }