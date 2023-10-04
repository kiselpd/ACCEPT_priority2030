#include "monitoring_system.h"
#include "monitoring_strategy.h"
#include <memory>
#include "boost/date_time.hpp"
#include "boost/format.hpp"
#include <thread>

// EspMonitoringSystem
EspMonitoringSystem::EspMonitoringSystem(const std::string &token) : sensors_condition_(std::make_shared<SensorsCondition>()),
                                                                     mode_condition_(std::make_shared<ModeCondition>()),
                                                                     token_(token){};

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

void EspMonitoringSystem::getPreparatoryData()
{
    auto request = this->createRequestToDB_();
    this->notifyDispatcher(Notification(Addressee::Database, BaseMessage(request)));
};

std::shared_ptr<DBSelectRequest> EspMonitoringSystem::createRequestToDB_()
{
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_token = token_;
    request->_source = "predicted_power_info";

    boost::gregorian::date today = boost::gregorian::day_clock::local_day();

    auto option = boost::format("datestamp ='%1%'") % today;
    request->_option = option.str();

    return request;
};

// ClientMonitiringSystem
ClientMonitiringSystem::ClientMonitiringSystem(const std::string &token) : token_(token){};

void ClientMonitiringSystem::getNotification(const Notification &notification)
{
    std::shared_ptr<DBBaseMessage> message_from_db = std::get<std::shared_ptr<DBBaseMessage>>(notification.getMessage());
    auto [val_count, val] = message_from_db->getAnswer();
    if (message_from_db->getStatus() == 200 && val_count)
    {
        switch (val[0].size())
        {
        case 5:
            this->setPredictedPowersInfo_(val);
            break;
        case 9:
            this->setActualPowersInfo_(val);
            break;
        case 4:
            this->setConsumersInfo_(val);
            break;
        default:
            break;
        }
    }
};

Addressee ClientMonitiringSystem::getName() const
{
    return Addressee::Monitoring_System;
};

void ClientMonitiringSystem::getPreparatoryData()
{    
    auto consumer_request = this->createConsumersRequestToDB_();
    this->notifyDispatcher(Notification(Addressee::Database, BaseMessage(consumer_request)));
};

std::shared_ptr<DBSelectRequest> ClientMonitiringSystem::createPredictedRequestToDB_()
{
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_token = token_;
    request->_source = "predicted_power_info";
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date in_seven_day = today + boost::gregorian::days(7);

    auto option = boost::format("datestamp BETWEEN '%1%' AND '%2%'") % today % in_seven_day;
    request->_option = option.str();

    return request;
};

std::shared_ptr<DBSelectRequest> ClientMonitiringSystem::createActualRequestToDB_()
{
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_token = token_;
    request->_source = "reality_power_info";
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    boost::gregorian::date before_seven_day = today - boost::gregorian::days(7);

    auto option = boost::format("datestamp BETWEEN '%1%' AND '%2%'") % before_seven_day % today;
    request->_option = option.str();

    return request;
};

std::shared_ptr<DBSelectRequest> ClientMonitiringSystem::createConsumersRequestToDB_(){
    std::shared_ptr<DBSelectRequest> request = std::make_shared<DBSelectRequest>();
    request->_token = token_;
    request->_source = "consumer_info";

    return request;
};

void ClientMonitiringSystem::setPredictedPowersInfo_(const db_value &value)
{
    auto predicted = std::make_shared<PredictedPowers>();

    for (const auto &row : value)
    {
        PredictedPower val;
        val.date = row[2].c_str();
        val.generated_power = std::atof(row[3].c_str());
        val.consumption_power = std::atof(row[4].c_str());

        predicted->push_back(val);
    };

    predicted_powers_ = predicted;

    auto actual_request = this->createActualRequestToDB_();
    this->notifyDispatcher(Notification(Addressee::Database, BaseMessage(actual_request)));
};

void ClientMonitiringSystem::setActualPowersInfo_(const db_value &value)
{
    auto actual = std::make_shared<ActualPowers>();

    for (const auto &row : value)
    {
        ActualPower val;
        val.date = row[2].c_str();
        val.solar = std::atof(row[3].c_str());
        val.wind = std::atof(row[4].c_str());
        val.generator = std::atof(row[5].c_str());
        val.consumer[0] = std::atof(row[6].c_str());
        val.consumer[1] = std::atof(row[7].c_str());
        val.consumer[2] = std::atof(row[8].c_str());

        actual->push_back(val);
    };

    actual_powers_ = actual;

    if (predicted_powers_ && actual_powers_)
    {
        auto message_to_client = std::make_shared<ClientGraphsMessage>(*predicted_powers_, *actual_powers_);
        this->notifyDispatcher(Notification(Addressee::Messenger, BaseMessage(message_to_client)));
    }
};

void ClientMonitiringSystem::setConsumersInfo_(const db_value &value)
{
    Consumers consumers;
    consumers.resize(CONSUMERS_NUMBER);

    for (const auto &row : value)
    {
        std::string val;
        val = row[2].c_str();
        consumers[std::atoi(row[CONSUMERS_NUMBER].c_str()) - 1].push_back(val);
    };

    auto predicted_request = this->createPredictedRequestToDB_();
    this->notifyDispatcher(Notification(Addressee::Database, BaseMessage(predicted_request)));

    auto message_to_client = std::make_shared<ClientConsumersMessage>(consumers);
    this->notifyDispatcher(Notification(Addressee::Messenger, BaseMessage(message_to_client)));
};