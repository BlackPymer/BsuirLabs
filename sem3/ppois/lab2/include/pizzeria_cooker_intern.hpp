#pragma once
#include "pizzeria_cooker.hpp"
#include "intern.hpp"

class PizzeriaCookerIntern : virtual public PizzeriaCooker, public Intern
{
public:
    PizzeriaCookerIntern(int age, std::string name);
    PizzeriaCookerIntern(int age, std::string name, double time_factor);
    void CookOrder(Order order, void *onOrderCooked) override;
};
