#include "common/pizzeria_exceptions.hpp"

PizzeriaException::PizzeriaException(std::string message) : _message(message)
{
}

const char *PizzeriaException::what() const noexcept
{
    return _message.c_str();
}