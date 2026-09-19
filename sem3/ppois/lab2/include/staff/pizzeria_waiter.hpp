#pragma once
#include "staff/pizzeria_worker.hpp"
#include "order/order.hpp"
#include <functional>
#include <map>
#include <memory>
#include <mutex>
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
    std::mutex mtx;
    void _onOrderCooked(Order order);
    std::map<Order, std::function<void()>> orders;
};
