#include "pizzeria_department.hpp"
#include <type_traits>

template <typename T>
std::shared_ptr<T> PizzaDepartment::Hire(const Human &human)
{
    auto worker = std::make_shared<T>(human.GetAge(), human.GetName());
    worker->SetDepartment(shared_from_this());
    if constexpr (std::is_same_v<T, PizzeriaWaiter>)
    {
        _waiters.insert(std::make_pair(worker, 0));
    }
    else if constexpr (std::is_same_v<T, PizzeriaCooker>)
    {
        _cookers.insert(std::make_pair(worker, false));
        _free_cookers++;
    }
    else
    {
        static_assert(std::is_same_v<T, PizzeriaCookerIntern>, "Unsupported employee type");
        _interns.insert(std::make_pair(worker, false));
        _free_cookers++;
    }
    return worker;
}

template std::shared_ptr<PizzeriaWaiter> PizzaDepartment::Hire<PizzeriaWaiter>(const Human &);
template std::shared_ptr<PizzeriaCooker> PizzaDepartment::Hire<PizzeriaCooker>(const Human &);
template std::shared_ptr<PizzeriaCookerIntern> PizzaDepartment::Hire<PizzeriaCookerIntern>(const Human &);

PizzaDepartment::PizzaDepartment(std::string address, int waiters, int cooks, int interns)
    : _address(address), _free_cookers(0)
{
    for (int i = 0; i < waiters; i++)
        Hire<PizzeriaWaiter>(Human(20, "Mike"));

    for (int i = 0; i < cooks; i++)
        Hire<PizzeriaCooker>(Human(20, "Mike"));

    for (int i = 0; i < interns; i++)
        Hire<PizzeriaCookerIntern>(Human(20, "Mike"));
}
std::string PizzaDepartment::GetAddress()
{
    return _address;
}
void PizzaDepartment::TakeOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady)
{
    if (_free_cookers == 0)
        throw AllCooksAreBusyException();
    auto least_busy = _waiters.begin();
    for (auto it = _waiters.begin(); it != _waiters.end(); ++it)
    {
        if (it->second < least_busy->second)
            least_busy = it;
    }
    least_busy->first->GetOrder(order, onOrderReady);
    least_busy->second++;
}
void PizzaDepartment::GiveOrderToCook(Order order, std::function<void()> onOrderCooked)
{
    auto free_cooker = _cookers.end();
    for (auto it = _cookers.begin(); it != _cookers.end(); ++it)
    {
        if (!it->second)
        {
            free_cooker = it;
            break;
        }
    }
    if (free_cooker != _cookers.end())
    {
        free_cooker->second = true;
        _free_cookers--;
        free_cooker->first->CookOrder(order, onOrderCooked);
        return;
    }
    auto free_intern = _interns.end();
    for (auto it = _interns.begin(); it != _interns.end(); ++it)
    {
        if (!it->second)
        {
            free_intern = it;
            break;
        }
    }
    if (free_intern != _interns.end())
    {
        free_intern->second = true;
        _free_cookers--;
        free_intern->first->CookOrder(order, onOrderCooked);
        return;
    }
}
void PizzaDepartment::CookingFinished(PizzeriaCookerIntern &intern)
{
    for (auto it = _interns.begin(); it != _interns.end(); ++it)
    {
        if (it->first.get() == &intern)
        {
            it->second = false;
            _free_cookers++;
            return;
        }
    }
}
void PizzaDepartment::CookingFinished(PizzeriaCooker &cooker)
{
    for (auto it = _cookers.begin(); it != _cookers.end(); ++it)
    {
        if (it->first.get() == &cooker)
        {
            it->second = false;
            _free_cookers++;
            return;
        }
    }
}
void PizzaDepartment::DeliverFinished(PizzeriaWaiter &waiter)
{
    for (auto it = _waiters.begin(); it != _waiters.end(); ++it)
    {
        if (it->first.get() == &waiter)
        {
            it->second--;
            return;
        }
    }
}