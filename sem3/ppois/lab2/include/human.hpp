#pragma once
#include <string>
class Human
{
public:
    int GetAge();
    std::string GetName();
    Human(int age, std::string name);

private:
    int _age;
    std::string _name;
};