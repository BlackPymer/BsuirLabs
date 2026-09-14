#pragma once
#include "pizzeria_cooker.hpp"

class PizzeriaCookerIntern : virtual public PizzeriaCooker
{
public:
    PizzeriaCookerIntern(int age, std::string name);
    void CookOrder(Order order, void *onOrderCooked) override;
};