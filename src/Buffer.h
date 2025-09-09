#pragma once

#include <string>

class Buffer
{
private:
    std::string buf;

public:
    Buffer();
    ~Buffer();
    
    const char* c_str();
    ssize_t size();
    void append(const char* _str, int _size);
    void clear();
    void getline();
    void setBuf(const char* _str);

};