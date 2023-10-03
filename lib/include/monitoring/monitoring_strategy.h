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


class StrategyMessageFromEsp : public BaseStrategy
{
public:
    StrategyMessageFromEsp(std::shared_ptr<SensorsCondition> sensors_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<SensorsCondition> sensors_condition_;
};


class StrategyMessageFromClient : public BaseStrategy
{
public:
    StrategyMessageFromClient(std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<ModeCondition> mode_condition_;
};

class StrategyMessageFromDB : public BaseStrategy
{
public:
    StrategyMessageFromDB(std::shared_ptr<ModeCondition> mode_condition);
    Notification process(const BaseMessage& base_message) final;

private:
    std::shared_ptr<ModeCondition> mode_condition_;
};

#endif /*MONITORING_STRATEGY_H*/