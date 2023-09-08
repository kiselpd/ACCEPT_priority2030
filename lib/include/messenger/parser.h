#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>

#include "additional.hpp"
#include "json.hpp"

size_t get_from_json(std::pair<int, std::string> info, const std::string& str_json);

std::string datagram_to_json(const Datagram& t_struct);
size_t get_datagram_from_json(Datagram& t_struct, const std::string& str_json);

std::string auth_to_json(const Auth& t_struct);
size_t get_auth_from_json(Auth& t_struct, const std::string& str_json);

std::string full_sensors_to_json(const FullSensors& t_struct);

std::string mode_to_json(const Mode& t_struct);
size_t get_mode_from_json(Mode& t_struct, const std::string& str_json);

size_t get_switch_relay_from_json(SwitchRelay& t_struct, const std::string& str_json);

std::string consumers_to_json(const Consumers& t_struct);

#endif /*PARSER_H*/