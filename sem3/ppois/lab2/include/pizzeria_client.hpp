#pragma once
#include "human.hpp"
#include "pizza.hpp"
#include "pizzeria_department.hpp"
#include <vector>
#include <memory>

class PizzeriaClient : virtual public Human
{
public:
    PizzeriaClient(int age, std::string name, std::shared_ptr<PizzaDepartment> department);
    void OrderPizza(std::vector<Pizza> pizzas, void *onOrderReady);

private:
    std::vector<Pizza> _order;
    std::shared_ptr<PizzaDepartment> _department;
};
