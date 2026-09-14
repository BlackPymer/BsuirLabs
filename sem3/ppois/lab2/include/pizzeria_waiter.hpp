#pragma once
#include "pizzeria_worker.hpp"
#include "order.hpp"
#include <map>
class PizzeriaWaiter : virtual private PizzeriaWorker
{
public:
    PizzeriaWaiter(int age, std::string name);
    using PizzeriaWorker::GetName;
    void GetOrder(Order order, void *onOrderReady);

private:
    void _onOrderCooked();
    std::map<Order, void *> orders;
};