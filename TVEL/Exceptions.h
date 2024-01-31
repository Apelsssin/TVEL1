#pragma once
#include <exception>
#include <iostream>
#include <string>

class Exceptions : public std::exception
{
private:
    std::string message;
public:
    Exceptions(const std::string& message) {
        this->message = message;
    }
    const char* what() const noexcept override;
};