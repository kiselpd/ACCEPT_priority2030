#ifndef MONITORING_SYSTEM_H
#define MONITORING_SYSTEM_H

#include "notification_observer.h"
#include "system_condition.h"

class BaseStrategy
{
public:
    virtual ~BaseStrategy() = default;
    virtual Notification process(const BaseMessage& base_message) = 0;
};


class EspStrategy : public BaseStrategy
{
public:
    EspStrategy(std::shared_ptr<SensorsCondition> sensors_condition, std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;
};


class ClientStrategy : public BaseStrategy
{
public:
    ClientStrategy(std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;
};


class DBStrategy : public BaseStrategy
{
public:
    DBStrategy(std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<ModeCondition> mode_condition_;
};


class MonitoringSystem : public NotificationListener
{
public:
    void getNotification(const Notification& notification) final;
    Addressee getName() const final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;
};

#endif /*MONITORING_SYSTEM_H*/