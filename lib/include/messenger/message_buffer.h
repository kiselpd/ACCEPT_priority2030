#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H

#include <vector>
#include <memory>
#include <iostream>

#include "additional.h"

const size_t STRUCT_TYPE_SIZE = 8;
const size_t DEFAULT_CLIENT_BUFFER_SIZE = 2024;

class EspMessageBuffer
{
public:
    EspMessageBuffer();

    char* getTypeBuffer();
    char* getStructBuffer();

    size_t getTypeBuffer() const;
    size_t getStructBuffer() const;

    size_t convertTypeBuffer();

    size_t getType() const;
    size_t getSize() const;
    
private:
    std::shared_ptr<char[]> type_buffer_;
    std::shared_ptr<char[]> struct_buffer_;

    size_t size_;
    Type type_;

    const size_t type_buffer_size_ = 8;
};


class ClientMessageBuffer
{
public:
    ClientMessageBuffer(const size_t& buffer_size = DEFAULT_CLIENT_BUFFER_SIZE);

    std::vector<char>& get();

    std::string getString() const;
    size_t size() const;

private:
    std::vector<char> buffer_;
};

#endif /*MESSAGE_BUFFER_H*/