#ifndef MONITORING_STRATEGY_H
#define MONITORING_STRATEGY_H

#include "additional.hpp"

#include "notification_observer.h"

#include "system_condition.h"

class BaseStrategy
{
public:
    virtual ~BaseStrategy() = default;
    virtual Notification process(const BaseMessage& base_message) = 0;
};


class EspStrategyFromEsp : public BaseStrategy
{
public:
    EspStrategyFromEsp(std::shared_ptr<SensorsCondition> sensors_condition, std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;
};


class ClientStrategyFromEsp : public BaseStrategy
{
public:
    ClientStrategyFromEsp(std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;
};


class DBStrategyFromEsp : public BaseStrategy
{
public:
    DBStrategyFromEsp(std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<ModeCondition> mode_condition_;
};


class DBStrategyFromClient : public BaseStrategy
{
public:
    Notification process(const BaseMessage& base_message) final;
};

#endif /*MONITORING_STRATEGY_H*/