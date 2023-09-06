#ifndef NOTIFICATION_OBSERVER_H
#define NOTIFICATION_OBSERVER_H

#include <map>
#include "notification.h"

class NotificationListener;

class NotificationDispatcher
{
public:
    void attachListener(std::shared_ptr<NotificationListener> listener);

    void detachListener(std::shared_ptr<NotificationListener> listener);
    void detachListener(const Addressee& listener);

    void notifyListener(const Notification& notification);
    
private:
    std::map<Addressee, std::shared_ptr<NotificationListener>> listeners_;
};


class NotificationListener
{
public:
    virtual ~NotificationListener(){};

    virtual void getNotification(const Notification& notification) = 0;
    virtual Addressee getName() const = 0;

    void attachDispatcher(std::shared_ptr<NotificationDispatcher> dispatcher);
    void detachDispatcher();

    void notifyDispatcher(const Notification& notification);

private:
    std::weak_ptr<NotificationDispatcher> dispatcher_;
};

#endif /*NOTIFICATION_OBSERVER_H*/