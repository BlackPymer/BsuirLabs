#include "pizzeria_waiter.hpp"
#include "pizzeria_department.hpp"
PizzeriaWaiter::PizzeriaWaiter(int age, std::string name)
    : Human(age, name), PizzeriaWorker(age, name)
{
    orders = std::map<Order, std::function<void()>>();
}

void PizzeriaWaiter::GetOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady)
{
    orders[Order(order)] = onOrderReady;
    if (_department != nullptr)
    {
        _department->GiveOrderToCook(Order(order), [this](){ _onOrderCooked(); });
    }
    else
        throw DepartmentNotSetException();
}