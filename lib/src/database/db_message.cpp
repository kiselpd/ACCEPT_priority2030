#include "db_message.h"

#include "json.hpp"
#include <boost/algorithm/string/join.hpp>

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

// DBAnswerBody
DBAnswerBody::DBAnswerBody(const std::string& answer_body){
    nlohmann::json json_body = nlohmann::json::parse(answer_body);
    if(json_body.contains("count"))
        this->_count = json_body["count"];

    if(json_body.contains("value"))
        this->_value = json_body["value"].get<std::vector<std::vector<std::string>>>();
};

std::pair<size_t, DBAnswerBody> parseAnswer(const std::string& answer){
    nlohmann::json json_body = nlohmann::json::parse(answer);
    size_t status;

    if(json_body.contains("header"))
        status = json_body["header"];

    nlohmann::json body;

    if(json_body.contains("body"))
        body = json_body["body"];

    DBAnswerBody answer_body(body.dump());

    return {status, answer_body};
}