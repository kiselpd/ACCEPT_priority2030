#include "additional.hpp"

FullPower operator+=(FullPower &power, const FullSensors &sensors)
{
    power.solar += sensors.solar.power;
    power.wind += sensors.wind.power;
    power.generator += sensors.generator.power;

    for (size_t i = 0; i < CONSUMERS_NUMBER; i++)
        power.consumer[i] += sensors.consumer[i].power;

    return power;
};

FullPower operator/(const FullPower &power, const size_t &k)
{
    FullPower ans;
    ans.solar = power.solar / k;
    ans.wind = power.wind / k;
    ans.generator = power.generator / k;

    for (size_t i = 0; i < CONSUMERS_NUMBER; i++)
        ans.consumer[i] = power.consumer[i] / k;

    return ans;
};