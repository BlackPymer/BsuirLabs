#include "staff/pizzeria_waiter.hpp"
#include "pizzeria/pizzeria_department.hpp"
#include <mutex>
PizzeriaWaiter::PizzeriaWaiter(int age, std::string name)
    : Human(age, name), PizzeriaWorker(age, name)
{
}

void PizzeriaWaiter::GetOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady)
{
    Order o(order);
    o.SetStatus(OrderStatus::IN_PROGRESS);
    {
        std::lock_guard<std::mutex> lock(mtx);
        orders[o] = onOrderReady;
    }
    if (_department != nullptr)
    {
        _department->GiveOrderToCook(o, std::bind(&PizzeriaWaiter::_onOrderCooked, this, std::placeholders::_1));
    }
    else
        throw DepartmentNotSetException();
}
int PizzeriaWaiter::GetActiveOrders()
{
    std::lock_guard<std::mutex> lock(mtx);
    return orders.size();
}

void PizzeriaWaiter::_onOrderCooked(Order order)
{
    std::function<void()> onOrderReady;
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = orders.find(order);
        if (it == orders.end())
            return;
        onOrderReady = it->second;
        orders.erase(it);
    }
    _department->DeliverFinished(*this);
    onOrderReady();
    _department->OrderFinished();
    order.SetStatus(OrderStatus::DELIVERED);
}
