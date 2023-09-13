#ifndef ADDITIONAL_HPP
#define ADDITIONAL_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <cstring>

const size_t CONSUMERS_NUMBER = 3;

enum User
{
    Esp = 0,
    Client,
    DB
};

enum StructType
{
    DATAGRAM = -1,
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

struct Datagram // отправляется на есп первой, с типом и размером следующей структуры
{
    int type;
    int size; //если type == SUCCESS, ERROR или SENSORS_DATA(на есп) то size = 0
};

struct AuthSize //с esp после этой структуры отправляется логин и пароль просто буфером символов
{
    int loginSize;
    int passwordSize;
};

struct Auth
{
    std::string login;
    std::string password;
};

struct Relay
{
    double voltage; //ВAuthSize
    double current; //мА
    bool status;
};

struct Sensors // отправляется с есп на сервер
{
    Relay solar;
    Relay wind;
    Relay generator;
    uint16_t battery_voltage;
    Relay consumer[CONSUMERS_NUMBER];
};

struct FullRelay
{
    double voltage;
    double current;
    double power;
    int percentages;
    bool status;
};

struct FullBattery
{
    double voltage;
    double percentages;
    bool status;
};

struct FullSensors // отправляеpowerтся с сервера на клиент
{
    FullRelay solar;
    FullRelay wind;
    FullRelay generator;
    FullBattery battery;
    FullRelay consumer[CONSUMERS_NUMBER];
};

struct FullPower
{
    FullPower() :
        solar(0), wind(0), generator(0), consumer{0} {};

    double solar;
    double wind;
    double generator;
    double consumer[CONSUMERS_NUMBER];

    friend FullPower operator+=(FullPower& power, const FullSensors& sensors);
    friend FullPower operator/(const FullPower& power, const size_t& k);
};

struct Consumer
{
    std::string name;
    double consumption;
};

typedef std::vector<std::vector<Consumer>> Consumers;

struct SwitchRelay // отправляется с сервера на есп о переключении реле
{
    int relayNumber;
    int status;
};

struct Mode // отправляется на есп о режиме работы
{
    int mode;
    int k; // какой-то коэффициент лучше пусть будет
};

template<typename TStruct>
TStruct get_struct(std::shared_ptr<char[]>& buffer){
    TStruct* t_struct = reinterpret_cast<TStruct*>(buffer.get());
    return *t_struct;
};

template<typename TStruct>
std::shared_ptr<char[]> get_buffer(const TStruct& t_struct){
    std::shared_ptr<char[]> buffer{new char[sizeof(t_struct)]};
    std::memcpy(buffer.get(), &t_struct, sizeof(t_struct));
    return buffer;
};

template<typename TStruct>
std::shared_ptr<char[]> get_buffer(const StructType& type, const TStruct& t_struct){
    Datagram type_struct{type, sizeof(t_struct)};
    std::shared_ptr<char[]> buffer{new char[sizeof(Datagram) + sizeof(t_struct)]};
    std::memcpy(buffer.get(), &type_struct, sizeof(type_struct));
    std::memcpy(buffer.get() + sizeof(Datagram), &t_struct, sizeof(t_struct));
    return buffer;
};

#endif /*ADDITIONAL_HPP*/