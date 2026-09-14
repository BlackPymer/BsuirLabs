#pragma once
#include <string>
class Pizza
{
public:
    Pizza(std::string name);
    std::string GetName();

private:
    std::string _name;
};