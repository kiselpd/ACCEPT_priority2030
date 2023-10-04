#ifndef SYSTEM_CONDITION_H
#define SYSTEM_CONDITION_H

#include "additional.hpp"

class SensorsCondition
{
public:
    void updateCondition(const Sensors& sensors_data);

    FullSensors getTmpCondition() const;
    FullPower getPowerCondition() const;

    void resetMeasurements();

private:
    FullSensors calculateFullSensors_(const Sensors& sensors_data);
    FullBattery calculateFullBattery_(const Sensors& sensors_data) const;
    FullRelay calculateFullSensor_(const Relay& sensor_data) const;

    FullSensors tmp_sensors_condition_;
    FullPower accumulated_power_;
    size_t measurements_count_;
};

class ModeCondition
{
public:
    void updateModeCondition(const int& mode_condition);
    void updatePredictedDataCondition(const double& predicted_data_condition);
    Mode getCondition() const;
    
private:
    Mode tmp_mode_condition_;
};

#endif /*SYSTEM_CONDITION_H*/