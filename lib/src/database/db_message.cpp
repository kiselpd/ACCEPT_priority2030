#include "db_message.h"

#include <boost/algorithm/string/join.hpp>

// DBBaseMessage
std::string DBBaseMessage::createRequest() const{
    return std::string();
};

size_t DBBaseMessage::getStatus() const{
    return EXIT_SUCCESS;
};

std::tuple<size_t, db_value> DBBaseMessage::getAnswer() const{
    return std::tuple<size_t, db_value>();
};

// DBSelectRequest
std::string DBSelectRequest::createRequest() const{
    std::string str_request;

    if(!_source.empty()){
        nlohmann::json request;
        nlohmann::json header;
        nlohmann::json body;

        header["type"] = "GET";
        body["source"] = _source;

        if(!_target.empty())
            body["target"] = "[" + boost::algorithm::join(_target, ","); + "]";

        if(!_option.empty())
            body["option"] = _option;

        if(_limit)
            body["limit"] = _limit;

        request["header"] = header;
        request["body"] = body;

        str_request = request.dump();
    }

    return str_request;
};

// DBInsertRequest
std::string DBInsertRequest::createRequest() const{
    std::string str_request;

    if(!_source.empty()){
        nlohmann::json request;
        nlohmann::json header;
        nlohmann::json body;

        header["type"] = "PUT";
        body["source"] = _source;

        if(!_target.empty())
            body["target"] = "[" + boost::algorithm::join(_target, ","); + "]";

        if(!_value.empty()){
            std::vector<std::string> value;
            for(const auto& elem : _value)
                value.push_back("[" + boost::algorithm::join(elem, ",") + "]");
            body["value"] = "[" + boost::algorithm::join(value, ",") + "]";
        }

        request["header"] = header;
        request["body"] = body;

        str_request = request.dump();
    }

    return str_request;
};

// DBUpdateRequest
std::string DBUpdateRequest::createRequest() const{
    std::string str_request;

    if(!_source.empty()){
        nlohmann::json request;
        nlohmann::json header;
        nlohmann::json body;

        header["type"] = "POST";
        body["source"] = _source;

        if(!_target.empty())
            body["target"] = "[" + boost::algorithm::join(_target, ","); + "]";

        if(!_option.empty())
            body["option"] = _option;

        request["header"] = header;
        request["body"] = body;

        str_request = request.dump();
    }

    return str_request;
};

// DBDeleteRequest
std::string DBDeleteRequest::createRequest() const{
    std::string str_request;

    if(!_source.empty()){
        nlohmann::json request;
        nlohmann::json header;
        nlohmann::json body;

        header["type"] = "DELETE";
        body["source"] = _source;

        if(!_option.empty())
            body["option"] = _option;

        request["header"] = header;
        request["body"] = body;

        str_request = request.dump();
    }

    return str_request;
};

// DBBaseAnswer
DBBaseAnswer::DBBaseAnswer(const std::string& str_json){
    nlohmann::json json = nlohmann::json::parse(str_json);
    this->setStatus_(json);

    if(status_ == 200){
        this->setCount_(json);
        if(count_)
            this->setValue_(json);
    }
};

void DBBaseAnswer::setStatus_(const nlohmann::json& json){
    if(json.contains("header"))
        status_ = json["header"];
};

void DBBaseAnswer::setCount_(const nlohmann::json& json){
    if(json.contains("count"))
        count_ = json["count"];
};

void DBBaseAnswer::setValue_(const nlohmann::json& json){
    if(json.contains("value"))
        value_ = json["value"].get<db_value>();
};

size_t DBBaseAnswer::getStatus() const{
    return status_;
};

std::tuple<size_t, db_value> DBBaseAnswer::getAnswer() const{
    return std::tuple<size_t, db_value>(count_, value_);
};