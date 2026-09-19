#pragma once
#include "staff/pizzeria_cooker.hpp"
#include "human/intern.hpp"
#include <functional>

class PizzeriaCookerIntern : virtual public PizzeriaCooker, public Intern
{
public:
    PizzeriaCookerIntern(int age, std::string name);
    PizzeriaCookerIntern(int age, std::string name, double time_factor);
    void CookOrder(Order order, std::function<void(Order)> onOrderCooked) override;
};
