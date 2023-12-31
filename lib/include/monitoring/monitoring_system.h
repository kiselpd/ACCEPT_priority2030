#ifndef MONITORING_SYSTEM_H
#define MONITORING_SYSTEM_H

#include "notification_observer.h"
#include "system_condition.h"

class EspMonitoringSystem : public NotificationListener
{
public:
    EspMonitoringSystem(const std::string& token);
    void getNotification(const Notification& notification) final;
    Addressee getName() const final;

    void getPreparatoryData();

private:
    std::shared_ptr<DBSelectRequest> createRequestToDB_();

    std::shared_ptr<SensorsCondition> sensors_condition_;
    std::shared_ptr<ModeCondition> mode_condition_;

    std::string token_;
};


class ClientMonitiringSystem : public NotificationListener
{
public:
    ClientMonitiringSystem(const std::string& token);
    void getNotification(const Notification& notification) final;
    Addressee getName() const final;

    void getPreparatoryData();

private:
    std::shared_ptr<DBSelectRequest> createPredictedRequestToDB_();
    std::shared_ptr<DBSelectRequest> createActualRequestToDB_();
    std::shared_ptr<DBSelectRequest> createConsumersRequestToDB_();

    void setPredictedPowersInfo_(const db_value& value);
    void setActualPowersInfo_(const db_value& value);
    void setConsumersInfo_(const db_value& value);

    std::shared_ptr<PredictedPowers> predicted_powers_;
    std::shared_ptr<ActualPowers> actual_powers_;

    std::string token_;
};

#endif /*MONITORING_SYSTEM_H*/