#ifndef MONITORING_SYSTEM_H
#define MONITORING_SYSTEM_H

#include "additional.h"
#include "notification_observer.h"

class SensorsDataCondition
{
public:   
    FullSensorsData getFullSensorsData() const;
    void updateCondition(const SensorsData& sensors_data);

private:
    void updateDatabaseInfo_();

    FullSensorsData calculateFullSensorsData_(const SensorsData& sensors_data);
    FullBatteryData calculateFullBatteryData_(const SensorsData& sensors_data) const;
    FullRelayData calculateFullSensorData_(const RelayData& sensor_data) const;

    FullSensorsData sensors_data_;
    FullEnergyData power_data_;
    size_t sensors_data_count_ = 0;
};


class ModeCondition
{
public:
    Mode getMode() const;

    void updateCondition(const Mode& mode, const FullSensorsData& full_sensors_data);

private:
    Mode mode_;
};


class MonitoringSystem : public NotificationListener
{
public:
    void getNotification(const Notification& notification) override;
    Addressee getName() const override;

private:
    void getSensorsDataRequest_(const SensorsData& sensors_data);
    void getModeRequest_(const Mode& mode);

    SensorsDataCondition sensors_data_condition_;
    ModeCondition mode_condition_;
};

#endif /*MONITORING_SYSTEM_H*/