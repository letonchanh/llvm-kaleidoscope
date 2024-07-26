#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>

struct Error
{
    std::string message;
    int code; // Optional error code

    Error(const std::string &msg, int c = 0) : message(msg), code(c) {}
};

#endif