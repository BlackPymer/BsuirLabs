#include "human.hpp"

int Human::GetAge()
{
    return _age;
}
std::string Human::GetName()
{
    return _name;
}
Human::Human(int age, std::string name)
{
    _age = age;
    _name = name;
}