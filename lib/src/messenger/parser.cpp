#include "parser.h"
#include "field_name.h"

size_t get_from_json(std::pair<int, std::string>& info, const std::string& str_json){
    try{
        nlohmann::json json = nlohmann::json::parse(str_json);
        info.first = (int)json[field_name::TYPE];
        info.second = json[field_name::DATA].dump();
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cout << "Info from json error!" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
};

std::string datagram_to_json(const Datagram& t_struct){
    std::string str_json;
    try{
        nlohmann::json json;
        json[field_name::TYPE] = t_struct.type;
        str_json = json.dump();
    }
    catch(const std::exception& e){
        std::cout << "Datagram to json error!" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return str_json;
};

size_t get_datagram_from_json(Datagram& t_struct, const std::string& str_json){
    try{
        nlohmann::json json = nlohmann::json::parse(str_json);
        t_struct.type = (int)json[field_name::TYPE];
        t_struct.size = 0;
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cout << "Datagram from json error!" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
};

std::string auth_to_json(const Auth& t_struct){
    std::string str_json;
    try{
        nlohmann::json json;
        json[field_name::TYPE] = (int)StructType::AUTH;

        nlohmann::json json_data;
        json_data[field_name::auth::LOGIN] = t_struct.login;
        json_data[field_name::auth::PASSWORD] = t_struct.password;

        json[field_name::DATA] = json_data;

        str_json = json.dump();
    }
    catch(const std::exception& e){
        std::cout << "Auth to json error!" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return str_json;
};

size_t get_auth_from_json(Auth& t_struct, const std::string& str_json){
    try{
        nlohmann::json json = nlohmann::json::parse(str_json);
        t_struct.login = (std::string)json[field_name::auth::LOGIN];
        t_struct.password = (std::string)json[field_name::auth::PASSWORD];
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cout << "Auth from json error!" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
};

std::string full_sensors_to_json(const FullSensors& t_struct){
    std::string str_json;
    try{
        nlohmann::json json;
        json[field_name::TYPE] = (int)StructType::SENSORS_DATA;

        nlohmann::json json_data;

        json_data[field_name::energy_source::SOLAR] = nlohmann::json::array(
            {(double)t_struct.solar.voltage,
            (double)t_struct.solar.current,
            (double)t_struct.solar.power,
            (double)t_struct.solar.percentages,
            (size_t)t_struct.solar.status});

        json_data[field_name::energy_source::WIND] = nlohmann::json::array(
            {(double)t_struct.wind.voltage,
            (double)t_struct.wind.current,
            (double)t_struct.wind.power,
            (double)t_struct.wind.percentages,
            (size_t)t_struct.wind.status});

        json_data[field_name::energy_source::GEN] = nlohmann::json::array(
            {(double)t_struct.generator.voltage,
            (double)t_struct.generator.current,
            (double)t_struct.generator.power,
            (double)t_struct.generator.percentages,
            (size_t)t_struct.generator.status});

        json_data[field_name::energy_source::BAT] = nlohmann::json::array(
            {(double)t_struct.battery.voltage,
            (int)t_struct.battery.percentages,
            (size_t)t_struct.battery.status});

        for(size_t consumer_number = 0; consumer_number < CONSUMERS_NUMBER; consumer_number++)
            json_data[std::to_string(consumer_number + 1)] = nlohmann::json::array(
                {(double)t_struct.consumer[consumer_number].voltage,
                (double)t_struct.consumer[consumer_number].current,
                (double)t_struct.consumer[consumer_number].power,
                (double)t_struct.consumer[consumer_number].percentages,
                (size_t)t_struct.consumer[consumer_number].status});

        json[field_name::DATA] = json_data;

        str_json = json.dump();
    }
    catch(const std::exception& e){
        std::cout << "Sensors to json error!" << std::endl;
        std::cerr << e.what() << '\n';
    }
    return str_json;
};

std::string mode_to_json(const Mode& t_struct){
    std::string str_json;
    try{
        nlohmann::json json;
        json[field_name::TYPE] = (int)StructType::MODE;

        nlohmann::json json_data;
        json_data[field_name::mode::MODE] = t_struct.mode;
        json_data[field_name::mode::K] = t_struct.k;

        json[field_name::DATA] = json_data;

        str_json = json.dump();
    }
    catch(const std::exception& e){
        std::cout << "Mode to json error!" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return str_json;
};

size_t get_mode_from_json(Mode& t_struct, const std::string& str_json){
    try{
        nlohmann::json json = nlohmann::json::parse(str_json);
        t_struct.mode = (int)json[field_name::mode::MODE];
        // t_struct.k = (double)json[field_name::mode::K];
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cout << "Mode from json error!" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
};

size_t get_switch_relay_from_json(SwitchRelay& t_struct, const std::string& str_json){
    try{
        nlohmann::json json = nlohmann::json::parse(str_json);
        t_struct.relayNumber = (int)json[field_name::switch_relay::RN];
        t_struct.status = (int)json[field_name::switch_relay::STATUS];
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cout << "SwitchRelay from json error!" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
};

std::string consumers_to_json(const Consumers& t_struct){
    std::string str_json;
    try{
        nlohmann::json json;
        json[field_name::TYPE] = (int)StructType::CONSUMERS_DATA;

        nlohmann::json json_data;
        for (size_t i = 0; i < t_struct.size(); i++){
            nlohmann::json group = nlohmann::json::array();
            for(const auto& elem : t_struct[i]){
                nlohmann::json consumer;
                consumer[field_name::consumer::NAME] = elem.name;
                consumer[field_name::consumer::CONSUMPTION] = elem.consumption;
                group.push_back(consumer);
            }
            json_data["G" + std::to_string(i + 1)] = group;
        }
        json[field_name::DATA] = json_data;

        str_json = json.dump();
    }
    catch(const std::exception& e){
        std::cout << "Consumers to json error!" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return str_json;
};

std::string graphs_to_json(const PredictedPowers& predicted_struct, const ActualPowers& actual_struct){
    std::string str_json;
    try
    {
        nlohmann::json json;
        json[field_name::TYPE] = (int)StructType::DAY_POWER_DATA;

        nlohmann::json json_data;

        nlohmann::json prediction = nlohmann::json::array();
        for(const auto& elem: predicted_struct){
            auto elem_array = nlohmann::json::array(
            {(std::string)elem.date,
            (double)elem.generated_power,
            (double)elem.consumption_power});

            prediction.push_back(elem_array);
        }

        nlohmann::json reality = nlohmann::json::array();
        for(const auto& elem: actual_struct){
            auto elem_array = nlohmann::json::array(
            {(std::string)elem.date,
            (double)elem.solar,
            (double)elem.wind,
            (double)elem.generator,
            (double)elem.consumer[0],
            (double)elem.consumer[1],
            (double)elem.consumer[2]});

            reality.push_back(elem_array);
        }

        json_data[field_name::graph::PREDICTION] = prediction;
        json_data[field_name::graph::REALITY] = reality;

        json[field_name::DATA] = json_data;

        str_json = json.dump();
    }
    catch(const std::exception& e)
    {
        std::cout << "Graphs to json error!" << std::endl;
        std::cerr << e.what() << '\n';
    }
    
};