#include "pizza.hpp"

Pizza::Pizza(std::string name)
{
    _name = name;
}
std::string Pizza::GetName() { return _name; }
bool Pizza::operator<(const Pizza &other) const { return _name < other._name; }