#include "human/human.hpp"

int Human::GetAge() const
{
    return _age;
}
std::string Human::GetName() const
{
    return _name;
}
Human::Human(int age, std::string name)
{
    _age = age;
    _name = name;
}