#include "pizzeria_client.hpp"
#include "pizzeria_department.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
PizzeriaClient::PizzeriaClient(int age, std::string name, std::shared_ptr<PizzaDepartment> department)
    : Human(age, name), _department(department)
{
    _department = department;
    _order = std::vector<Pizza>();
}

void PizzeriaClient::OrderPizza(std::vector<Pizza> menu)
{
    std::vector<std::pair<Pizza, int>> order = std::vector<std::pair<Pizza, int>>();
    for (auto &pizza : menu)
    {
        int count = std::rand() % 6;
        if (count > 0)
            order.push_back(std::make_pair(pizza, count));
    }
    _department->TakeOrder(order, std::bind(&PizzeriaClient::_OnPizzaDelivered, this));
}
void PizzeriaClient::_OnPizzaDelivered()
{
    std::cout << "Pizza delivered!" << std::endl;
}