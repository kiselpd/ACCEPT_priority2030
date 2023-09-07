#ifndef ADDITIONAL_H
#define ADDITIONAL_H

#include <iostream>
#include <memory>
#include <vector>

#define MAX 4

const size_t CONSUMERS_NUMBER = 3;

enum EspStructType
{
    SUCCESS = 0,
    ERROR,
    AUTH,
    SENSORS_DATA,
    SWITCH_RELAY = 5,
    SWITCH_MODE
};

enum Type
{
    SUCCESS = 0, //Ответ от сервера на клиента и есп, если авторизация успешна
    ERROR, //Ответ от сервера на клиента и есп, если авторизация не успешна
    AUTH, //Запрос на авторизацию от клиента на сервер и есп на сервер
    SENSORS_DATA, //Показатели силы тока и напряжения на esp (сервер отправляет запрос на есп, получает ответ и отправляет клиенту)
    CONSUMERS_DATA, //Сервер отправляет информацию с потребителями клиенту
    SWITCH_RELAY, //Клиент отправляет запрос на переключение реле на сервер, а сервер на есп.
    MODE, //Клиент отправляет запрос на сервер по режиму работы есп, а сервер отправляет на есп
    DAY_POWER_DATA, //Сервер отправляет клиенту информацию о графиках
    ESP_STATUS,
    PING,
    SHUTDOWN//Желательно чтобы это отправлял клиент и есп, когда они отключаются
};

struct StructType // отправляется на есп первой, с типом и размером следующей структуры
{
    int type;
    int size; //если type == SUCCESS, ERROR или SENSORS_DATA(на есп) то size = 0
};

struct AuthSize //с esp после этой структуры отправляется логин и пароль просто буфером символов
{
    int loginSize;
    int passwordSize;
};

struct AuthData
{
    std::string login;
    std::string password;
};

struct FullEnergyData
{
    double solar_energy;
    double wind_energy;
    double gen_energy;
    double consumer[CONSUMERS_NUMBER];
};

struct RelayData
{
    double voltage; //В
    double current; //мА
    bool status;
};

struct SensorsData // отправляется с есп на сервер
{
    RelayData solar;
    RelayData wind;
    RelayData generator;
    uint16_t battery_voltage;
    RelayData consumer[CONSUMERS_NUMBER];
};

struct FullRelayData
{
    double voltage;
    double current;
    double power;
    int percentages;
    bool status;
};

struct FullBatteryData
{
    double voltage;
    double percentages;
    bool status;
};

struct FullSensorsData // отправляеpowerтся с сервера на клиент
{
    FullRelayData solar;
    FullRelayData wind;
    FullRelayData generator;
    FullBatteryData battery;
    FullRelayData consumer[CONSUMERS_NUMBER];
};

struct ConsumerData
{
    size_t id;
    std::string consumer_name;
    bool status;
};

struct SwitchRelay // отправляется с сервера на есп о переключении реле
{
    int relayNumber;
    int status;
};

struct Mode // отправляется на есп о режиме работы
{
    int mode;
    double k; // какой-то коэффициент лучше пусть будет
};

typedef std::pair<size_t, std::vector<std::vector<std::string>>> DBResult;


#endif /*ADDITIONAL_H*/