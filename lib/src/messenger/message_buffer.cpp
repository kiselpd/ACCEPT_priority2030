#include "message_buffer.h"
#include <cstring>

// EspMessageBuffer
EspMessageBuffer::EspMessageBuffer() :
    type_buffer_(new char[type_buffer_size_]){};

char* EspMessageBuffer::getTypeBuffer(){return type_buffer_.get();};

char* EspMessageBuffer::getStructBuffer(){return struct_buffer_.get();};

size_t EspMessageBuffer::convertTypeBuffer(){
    Datagram datagram = get_struct<Datagram>(type_buffer_);
    type_ = (EspStructType)datagram.type;
    size_ = datagram.size;

    if(size_){
        std::shared_ptr<char[]> tmp_buffer{new char[size_]};
        struct_buffer_ = tmp_buffer;
    }
    return size_;
};

size_t EspMessageBuffer::getType() const{return type_;};

size_t EspMessageBuffer::getSize() const{return size_;};


// ClientMessageBuffer
ClientMessageBuffer::ClientMessageBuffer(const size_t& buffer_size){
    buffer_.resize(buffer_size);
};

std::vector<char>& ClientMessageBuffer::get(){
    std::fill(buffer_.begin(), buffer_.end(), 0);
    return buffer_;
};

std::string ClientMessageBuffer::getString() const{
    return std::string{buffer_.begin(), buffer_.end()};
};

size_t ClientMessageBuffer::size() const{
    return buffer_.size();
};