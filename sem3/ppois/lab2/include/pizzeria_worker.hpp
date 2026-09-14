#pragma once
#include "human.hpp"

class PizzeriaWorker : virtual public Human
{
public:
    PizzeriaWorker(int age, std::string name);
};