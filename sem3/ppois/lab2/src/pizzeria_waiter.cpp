#include "pizzeria_waiter.hpp"
#include "pizzeria_department.hpp"
PizzeriaWaiter::PizzeriaWaiter(int age, std::string name)
    : Human(age, name), PizzeriaWorker(age, name)
{
}

void PizzeriaWaiter::GetOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady)
{
    Order o(order);
    orders[o] = onOrderReady;
    if (_department != nullptr)
    {
        _department->GiveOrderToCook(o, std::bind(&PizzeriaWaiter::_onOrderCooked, this, o));
    }
    else
        throw DepartmentNotSetException();
}
int PizzeriaWaiter::GetActiveOrders()
{
    return orders.size();
}

void PizzeriaWaiter::_onOrderCooked(Order order)
{
    auto it = orders.find(order);
    if (it != orders.end())
    {
        auto onOrderReady = it->second;
        orders.erase(it);
        _department->DeliverFinished(*this);
        onOrderReady();
    }
}
