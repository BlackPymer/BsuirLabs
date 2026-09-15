#pragma once
#include <string>
class Human
{
public:
    int GetAge() const;
    std::string GetName() const;
    Human(int age, std::string name);

private:
    int _age;
    std::string _name;
};