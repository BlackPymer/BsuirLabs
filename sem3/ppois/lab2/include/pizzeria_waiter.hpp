#pragma once
#include "pizzeria_worker.hpp"
#include "order.hpp"
#include <functional>
#include <map>
#include <memory>
#include <vector>

class PizzeriaWaiter : virtual private PizzeriaWorker
{
public:
    PizzeriaWaiter(int age, std::string name);
    using PizzeriaWorker::GetName;
    using PizzeriaWorker::SetDepartment;
    void GetOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady);
    int GetActiveOrders();
private:
    void _onOrderCooked(Order order);
    std::map<Order, std::function<void()>> orders;
};
