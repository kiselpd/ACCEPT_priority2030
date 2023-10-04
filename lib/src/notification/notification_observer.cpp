#include "notification_observer.h"
#include <thread>

// NotificationDispatcher
void NotificationDispatcher::attachListener(std::shared_ptr<NotificationListener> listener)
{
    if (!(this->listeners_.count(listener->getName())))
    {
        this->listeners_.insert(std::make_pair(listener->getName(), listener));
    }
};

void NotificationDispatcher::detachListener(std::shared_ptr<NotificationListener> listener)
{
    this->listeners_.erase(listener->getName());
};

void NotificationDispatcher::detachListener(const Addressee &listener)
{
    this->listeners_.erase(listener);
};

void NotificationDispatcher::notifyListener(const Notification &notification)
{
    if (this->listeners_.count(notification.getAddressee()))
    {
        auto listener = this->listeners_.at(notification.getAddressee());
        listener->getNotification(notification);
    }
};

// NotificationListener
void NotificationListener::attachDispatcher(std::shared_ptr<NotificationDispatcher> dispatcher)
{
    this->dispatcher_ = dispatcher;
};

void NotificationListener::detachDispatcher()
{
    auto lock_dispatcher = dispatcher_.lock();
    if (lock_dispatcher)
        dispatcher_.reset();
};

void NotificationListener::notifyDispatcher(const Notification &notification)
{
    auto lock_dispatcher = dispatcher_.lock();
    if (lock_dispatcher)
        lock_dispatcher->notifyListener(notification);
};