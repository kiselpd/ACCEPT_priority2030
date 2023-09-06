#ifndef FIELD_NAME
#define FIELD_NAME

#include <iostream>

namespace field_name //Поля для заполнения json
{
    const std::string TYPE = "type"; 
    const std::string DATA = "data";
    const std::string DAY = "day";
    const std::string DAYS = "days";

    namespace auth //Авторизации
    {
        const std::string LOGIN = "login";
        const std::string PASSWORD = "password";
        const std::string STATUS = "status";
    }

    namespace energy_source //Показатели датчиков
    {
        const std::string SOLAR = "solar";
        const std::string WIND = "wind";
        const std::string GEN = "gen";
        const std::string BAT = "bat";
        const std::string CONSUMPTION = "consumption";
    }

    namespace consumer //Данные о потребителях
    {
        const std::string ID = "id";
        const std::string NAME = "name";
        const std::string STATUS = "status";
    }

    namespace switch_relay //Переключение реле
    {
        const std::string RN = "relay";
        const std::string STATUS = "status";
    }

    namespace mode //Режимы работы
    {
        const std::string MODE = "mode";
        const std::string K = "k";
    }
}

#endif /*FIELD_NAME*/