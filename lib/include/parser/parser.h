#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include "additional.h"
#include "json.hpp"
std::string make_type(int type);

// ready
int get_type_from_json(const std::string& json_message);
std::string get_data_from_json(const std::string& json_message);

size_t get_switch_relay_from_json(SwitchRelay& t_struct, const std::string& json_message);
size_t get_mode_from_json(Mode& t_struct, const std::string& json_message);

std::string full_sensors_data_to_json(const FullSensorsData& t_struct);
// 


#endif /*PARSER_H*/