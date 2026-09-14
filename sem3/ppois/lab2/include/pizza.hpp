#pragma once
#include <string>
class Pizza
{
public:
    Pizza(std::string name);
    std::string GetName();
    bool operator<(const Pizza &other) const;

private:
    std::string _name;
};