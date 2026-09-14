#include "pizzeria_department.hpp"

PizzaDepartment::PizzaDepartment(std::string address, int waiters, int cooks, int interns = 0)
{
    _address = address;
    _waiters = std::map<std::shared_ptr<PizzeriaWaiter>, int>();
    _cookers = std::map<std::shared_ptr<PizzeriaCooker>, bool>();
    _interns = std::map<std::shared_ptr<PizzeriaCookerIntern>, bool>();

    for (int i = 0; i < waiters; i++)
        _waiters.insert(std::make_pair(std::make_shared<PizzeriaWaiter>(), 0));

    for (int i = 0; i < cooks; i++)
        _cookers.insert(std::make_pair(std::make_shared<PizzeriaCooker>(), false));

    for (int i = 0; i < interns; i++)
        _cookers.insert(std::make_pair(std::make_shared<PizzeriaCookerIntern>(), false));
}