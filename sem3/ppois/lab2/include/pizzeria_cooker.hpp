#pragma once
#include "pizzeria_worker.hpp"
#include "order.hpp"
class PizzeriaCooker : virtual public PizzeriaWorker
{
public:
    PizzeriaCooker(int age, std::string name);
    virtual void CookOrder(Order order, void *onOrderCooked);
};