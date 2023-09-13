#ifndef MONITORING_SYSTEM_H
#define MONITORING_SYSTEM_H

#include "notification_observer.h"
#include "system_condition.h"

class EspMonitoringSystem : public NotificationListener
{
public:
    EspMonitoringSystem();
    void getNotification(const Notification& notification) final;
    Addressee getName() const final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;
};


class ClientMonitiringSystem : public NotificationListener
{
public:
    void getNotification(const Notification& notification) final;
    Addressee getName() const final;
};

#endif /*MONITORING_SYSTEM_H*/