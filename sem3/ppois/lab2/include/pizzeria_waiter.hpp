#pragma once
#include "pizzeria_worker.hpp"
#include "order.hpp"
#include <map>
#include <memory>

class PizzaDepartment;

class PizzeriaWaiter : virtual private PizzeriaWorker
{
public:
    PizzeriaWaiter(int age, std::string name);
    using PizzeriaWorker::GetName;
    void GetOrder(Order order, void *onOrderReady);
    int GetActiveOrders();
    void SetDepartment(std::shared_ptr<PizzaDepartment> department);

private:
    void _onOrderCooked();
    std::map<Order, void *> orders;
    std::shared_ptr<PizzaDepartment> _department;
};
