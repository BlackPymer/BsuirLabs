#pragma once
#include <string>
#include <map>
#include <memory>
#include "pizzeria_cooker.hpp"
#include "pizzeria_cooker_intern.hpp"
#include "pizzeria_waiter.hpp"

class PizzaDepartment
{
public:
    PizzaDepartment(std::string address, int waiters, int cooks, int interns = 0);
    std::string GetAddress();
    void TakeOrder(Order order, void *onOrderReady);
    void GiveOrderToCook(std::shared_ptr<PizzeriaWaiter> waiter, Order order, void *onOrderCooked);

private:
    std::string _address;
    std::map<std::shared_ptr<PizzeriaWaiter>, int> _waiters;
    std::map<std::shared_ptr<PizzeriaCooker>, bool> _cookers;
    std::map<std::shared_ptr<PizzeriaCookerIntern>, bool> _interns;
};
