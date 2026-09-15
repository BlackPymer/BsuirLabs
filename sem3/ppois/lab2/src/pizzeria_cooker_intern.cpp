#include "pizzeria_cooker_intern.hpp"
#include <chrono>
#include <thread>
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
    std::this_thread::sleep_for(std::chrono::milliseconds((long)(order.GetPizzaCount()*_time_factor*1000)));
    onOrderCooked(order);
}