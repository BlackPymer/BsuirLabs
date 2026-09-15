#include "pizzeria_cooker.hpp"
#include <chrono>
#include <thread>
PizzeriaCooker::PizzeriaCooker(int age, std::string name) : PizzeriaWorker(age, name), Human(age, name)
{
}

void PizzeriaCooker::CookOrder(Order order, std::function<void(Order)> onOrderCooked)
{
    std::this_thread::sleep_for(std::chrono::seconds(order.GetPizzaCount()));
    onOrderCooked(order);
}
