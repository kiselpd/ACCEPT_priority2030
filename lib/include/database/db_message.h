#ifndef DB_MESSAGE_H
#define DB_MESSAGE_H

#include <iostream>
#include <vector>

#include "json.hpp"

typedef std::vector<std::vector<std::string>> db_value;

class DBBaseMessage
{
public:
    virtual ~DBBaseMessage() = default;
    
    virtual std::string createRequest() const;
    virtual size_t getStatus() const;
    virtual std::pair<size_t, db_value> getAnswer() const;
};

class DBSelectRequest : public DBBaseMessage
{
public:
    std::string createRequest() const;

    std::string _source;
    std::vector<std::string> _target;
    std::string _option;
    size_t _limit = 0;
};

class DBInsertRequest : public DBBaseMessage
{
public:
    std::string _source;
    std::vector<std::string> _target;
    std::vector<std::vector<std::string>> _value;

    std::string createRequest() const;
};


class DBUpdateRequest : public DBBaseMessage
{
public:
    std::string _source;
    std::vector<std::string> _target;
    std::string _option;

    std::string createRequest() const;
};


class DBDeleteRequest : public DBBaseMessage
{
public:
    std::string _source;
    std::string _option;

    std::string createRequest() const;
};


class DBBaseAnswer : public DBBaseMessage
{
public:
    DBBaseAnswer(const std::string& str_json);

    size_t getStatus() const;
    std::pair<size_t, db_value> getAnswer() const;

private:
    void setStatus_(const nlohmann::json& json);
    void setCount_(const nlohmann::json& json);
    void setValue_(const nlohmann::json& json);

    size_t status_;
    size_t count_;
    db_value value_;
};

#endif /*DB_MESSAGE_H*/