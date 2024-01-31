#include "Exceptions.h"
const char* Exceptions::what() const noexcept
{
    return message.c_str();
}