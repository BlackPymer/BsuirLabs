#include "pizzeria_cooker_intern.hpp"
#include <chrono>
#include <thread>
#include "pizzeria_department.hpp"
PizzeriaCookerIntern::PizzeriaCookerIntern(int age, std::string name)
    : PizzeriaCooker(age, name), Intern(), Human(age, name), PizzeriaWorker(age, name)
{
}

PizzeriaCookerIntern::PizzeriaCookerIntern(int age, std::string name, double time_factor)
    : PizzeriaCooker(age, name), Intern(time_factor), Human(age, name), PizzeriaWorker(age, name)
{
}
void PizzeriaCookerIntern::CookOrder(Order order, std::function<void(Order)> onOrderCooked)
{
    double time_factor = _time_factor;
    auto dept = _department;

    std::thread([this, dept, order, onOrderCooked, time_factor]() mutable
                {
    std::this_thread::sleep_for(std::chrono::milliseconds((long)(order.GetPizzaCount()*time_factor*1000)));
    onOrderCooked(order);
    dept->CookingFinished(*this); })
        .detach();
}