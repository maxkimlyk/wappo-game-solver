#ifndef __UPDATE_STRING_HPP__
#define __UPDATE_STRING_HPP__

#include <iostream>
#include <sstream>
#include <string>

struct reset
{
};

class update_string
{
    std::ostream& stream_;
    size_t count_;

public:
    update_string(std::ostream& stream) : stream_(stream), count_(0)
    {
    }

    template <class Object>
    update_string& operator<< (const Object& object)
    {
        std::ostringstream tmp_stream;
        tmp_stream << object;
        std::string tmp = tmp_stream.str();
        count_ += tmp.size();
        stream_ << tmp;
        return *this;
    }

    update_string& operator<< (reset)
    {
        std::string tmp(count_, ' ');
        stream_ << "\r" << tmp << "\r";
        count_ = 0;
        return *this;
    }
};

#endif
