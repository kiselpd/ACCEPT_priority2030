#include "monitoring_system.h"
#include "monitoring_strategy.h"
#include <memory>
#include "boost/date_time.hpp"
#include "boost/format.hpp"

// EspMonitoringSystem
EspMonitoringSystem::EspMonitoringSystem() : sensors_condition_(std::make_shared<SensorsCondition>()),
                                             mode_condition_(std::make_shared<ModeCondition>()){};

void EspMonitoringSystem::getNotification(const Notification &notification)
{
    std::unique_ptr<BaseStrategy> strategy;
    auto base_message = notification.getMessage();

    switch (base_message.index())
    {
    case User::Esp:
        strategy = std::make_unique<StrategyMessageFromEsp>(sensors_condition_);
        break;
    case User::Client:
        strategy = std::make_unique<StrategyMessageFromClient>(mode_condition_);
        break;
    case User::DB:
        strategy = std::make_unique<StrategyMessageFromDB>(mode_condition_);
        break;
    default:
        return;
    }

    auto new_notification = strategy->process(base_message);
    this->notifyDispatcher(new_notification);
};

Addressee EspMonitoringSystem::getName() const
{
    return Addressee::Monitoring_System;
};

void EspMonitoringSystem::getPreparatoryData(){
    auto request = this->createRequestToDB();
    this->notifyDispatcher(Notification(Addressee::Database, BaseMessage(request)));
};

std::shared_ptr<DBSelectRequest> EspMonitoringSystem::createRequestToDB(){
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_source = "predicted_power_info";

    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    
    auto option = boost::format("datestamp ='%1%'") % today;
    request->_option = option.str();

    return request;
};