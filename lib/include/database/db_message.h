#ifndef DB_MESSAGE_H
#define DB_MESSAGE_H

#include <iostream>
#include <vector>

class DBBaseRequest
{
public:
    virtual std::string createRequest() const = 0;
};

class DBSelectRequest : public DBBaseRequest
{
public:
    std::string createRequest() const override;

    std::string _source;
    std::vector<std::string> _target;
    std::string _option;
    size_t _limit = 0;
};

class DBInsertRequest : public DBBaseRequest
{
public:
    std::string _source;
    std::vector<std::string> _target;
    std::vector<std::vector<std::string>> _value;

    std::string createRequest() const override;
};


class DBUpdateRequest : public DBBaseRequest
{
public:
    std::string _source;
    std::vector<std::string> _target;
    std::string _option;

    std::string createRequest() const override;
};


class DBDeleteRequest : public DBBaseRequest
{
public:
    std::string _source;
    std::string _option;

    std::string createRequest() const override;
};


class DBAnswerBody
{
public:
    DBAnswerBody(const std::string& answer_body);

    size_t _count;
    std::vector<std::vector<std::string>> _value;
};

std::pair<size_t, DBAnswerBody> parseAnswer(const std::string& answer);

#endif /*DB_MESSAGE_H*/