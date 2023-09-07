#include "parser.h"
#include "field_name.h"

//ready
std::string make_type(int type){
    nlohmann::json message;
    message["type"] = type;
    return message.dump();
};

size_t get_switch_relay_from_json(SwitchRelay& t_struct, const std::string& json_message){
    try{
        nlohmann::json message = nlohmann::json::parse(json_message);
        t_struct.relayNumber = (int)message[field_name::switch_relay::RN];
        t_struct.status = (int)message[field_name::switch_relay::STATUS];

        std::cout << "Relay " << t_struct.relayNumber << " is " << t_struct.status << std::endl;
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl << "Switch Relay parsing error!" << std::endl;
        return EXIT_FAILURE;
    }
};

size_t get_mode_from_json(Mode& t_struct, const std::string& json_message){
    try{
        nlohmann::json message = nlohmann::json::parse(json_message);
        t_struct.mode = (int)message[field_name::mode::MODE];
        t_struct.k = 0;//!!!
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl << "Switch Mode parsing error!" << std::endl;
        return EXIT_FAILURE;
    }
};

size_t get_auth_from_json(AuthData& t_struct, const std::string& json_message){
    try{
        nlohmann::json message = nlohmann::json::parse(json_message);
        t_struct.login = message["login"];
        t_struct.password = message["password"];
        return EXIT_SUCCESS;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl << "Switch Auth parsing error!" << std::endl;
        return EXIT_FAILURE;
    }
};

std::string get_data_from_json(const std::string& json_message){
    nlohmann::json message = nlohmann::json::parse(json_message);
    auto data = message[field_name::DATA];
    return data.dump();
};

int get_type_from_json(const std::string& json_message){
    nlohmann::json message = nlohmann::json::parse(json_message);
    int type = message[field_name::TYPE];
    return type;
};

std::string full_sensors_data_to_json(const FullSensorsData& t_struct){
    std::string str_json;
    try{
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

        for(size_t consumer_number = 1; consumer_number <= CONSUMERS_NUMBER; consumer_number++)
            json_data[std::to_string(consumer_number)] = nlohmann::json::array(
                {(double)t_struct.consumer[consumer_number - 1].voltage,
                (double)t_struct.consumer[consumer_number - 1].current,
                (double)t_struct.consumer[consumer_number - 1].power,
                (double)t_struct.consumer[consumer_number - 1].percentages,
                (size_t)t_struct.consumer[consumer_number - 1].status});

        nlohmann::json json;
        json[field_name::TYPE] = Type::SENSORS_DATA;
        json[field_name::DATA] = json_data;

        str_json = json.dump();
    }
    catch(const std::exception& e){
        std::cerr << e.what() << std::endl << "Sensors data parsing to json error!" << std::endl;
    }
    
    return str_json;
};