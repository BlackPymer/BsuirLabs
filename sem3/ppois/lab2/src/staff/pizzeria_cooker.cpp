#include "staff/pizzeria_cooker.hpp"
#include "pizzeria/pizzeria_department.hpp"
#include <chrono>
#include <thread>
PizzeriaCooker::PizzeriaCooker(int age, std::string name) : PizzeriaWorker(age, name), Human(age, name)
{
}

void PizzeriaCooker::CookOrder(Order order, std::function<void(Order)> onOrderCooked)
{
    auto dept = _department;
    std::thread([this, dept, order, onOrderCooked]() mutable
                {
            std::this_thread::sleep_for(std::chrono::seconds(order.GetPizzaCount()));
    onOrderCooked(order);
    dept->CookingFinished(*this); })
        .detach();
}
