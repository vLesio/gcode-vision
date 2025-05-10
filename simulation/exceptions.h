#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

class InvalidCameraModeException : public std::runtime_error {
public:
    InvalidCameraModeException(const std::string& msg)
        : std::runtime_error("[Camera Error] " + msg) {}
};

#endif
