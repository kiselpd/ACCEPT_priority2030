#include "notification.h"

// Notification
Notification::Notification(){}; 

Notification::Notification(const Addressee& addressee, BaseMessage message) : 
    addressee_(addressee), message_(message) {};

void Notification::set(const Addressee& addressee, BaseMessage message){
    this->addressee_ = addressee;
    this->message_ = message;
};

Addressee Notification::getAddressee() const{
    return this->addressee_;
};

BaseMessage Notification::getMessage() const{
    return this->message_;
};