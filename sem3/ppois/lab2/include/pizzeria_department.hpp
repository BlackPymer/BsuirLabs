#pragma once
#include <string>
#include <vector>
#include <memory>
#include "pizzeria_cooker.hpp"
#include "pizzeria_waiter.hpp"

class PizzaDepartment
{
public:
    PizzaDepartment(std::string address);
    std::string GetAddress();

private:
    std::string _address;
    std::vector<std::shared_ptr<PizzeriaCooker>> _cookers;
    std::vector<std::shared_ptr<PizzeriaWaiter>> _waiters;
    std::vector<std::shared_ptr<PizzeriaCooker>> _free_cookers;
    std::vector<std::shared_ptr<PizzeriaWaiter>> _free_waiters;
};