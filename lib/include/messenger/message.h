#ifndef MESSAGE_H
#define MESSAGE_H

#include <queue>
#include <variant>
#include <memory>
#include <cstring>
#include <mutex>
#include "additional.h"
#include <vector>
#include <iostream>

typedef std::variant<SwitchRelay, Mode, SensorsData, FullSensorsData, StructType, DBResult> struct_variant;

class BaseMessage
{
public:
    virtual std::shared_ptr<char[]> getBuffer() const;
    virtual std::string getJson() const;
    virtual size_t setJson(const std::string& json);
    virtual size_t getBufferSize() const;
    virtual Type getType() const = 0;
    virtual struct_variant getStruct() const = 0;
};

class TypeMessage : public BaseMessage
{
public:
    TypeMessage(const Type& type);
    std::string getJson() const override;
    std::shared_ptr<char[]> getBuffer() const override;
    Type getType() const override;
    struct_variant getStruct() const override;

private:
    Type type_;
};


class SwitchRelayMessage : public BaseMessage
{
public:   
    std::shared_ptr<char[]> getBuffer() const override;
    size_t setJson(const std::string& json) override;
    Type getType() const override;
    struct_variant getStruct() const override;
    size_t getBufferSize() const override;

private:
    SwitchRelay t_struct_;
};


class SwitchModeMessage : public BaseMessage
{
public:
    SwitchModeMessage(const Mode& mode);
    SwitchModeMessage(){};
    virtual std::shared_ptr<char[]> getBuffer() const;
    virtual size_t setJson(const std::string& json);
    Type getType() const override;
    struct_variant getStruct() const override;
    size_t getBufferSize() const override;

private:
    Mode t_struct_;
};


class SensorsDataMessage : public BaseMessage
{
public:
    SensorsDataMessage(const SensorsData& t_struct);
    Type getType() const override;
    
    struct_variant getStruct() const override;
    size_t getBufferSize() const override;

private:
    SensorsData t_struct_;
};


class FullSensorsDataMessage : public BaseMessage
{
public:
    FullSensorsDataMessage(const FullSensorsData& t_struct);
    std::string getJson() const override;
    Type getType() const override;

    struct_variant getStruct() const override;
    size_t getBufferSize() const override;

private:
    FullSensorsData t_struct_;
};

class FromDBMessage : public BaseMessage
{
public:
    FromDBMessage(const size_t& count, const std::vector<std::vector<std::string>>& result);
    struct_variant getStruct() const override;
    Type getType() const override;

private:
    size_t count_;
    std::vector<std::vector<std::string>> result_;
};

#endif /*MESSAGE_H*/