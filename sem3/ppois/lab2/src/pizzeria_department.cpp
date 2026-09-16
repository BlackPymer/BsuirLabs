#include "pizzeria_department.hpp"
#include <type_traits>
#include <thread>
#include <mutex>

template <typename T>
std::shared_ptr<T> PizzaDepartment::Hire(const Human &human)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
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
}

template std::shared_ptr<PizzeriaWaiter> PizzaDepartment::Hire<PizzeriaWaiter>(const Human &);
template std::shared_ptr<PizzeriaCooker> PizzaDepartment::Hire<PizzeriaCooker>(const Human &);
template std::shared_ptr<PizzeriaCookerIntern> PizzaDepartment::Hire<PizzeriaCookerIntern>(const Human &);

PizzaDepartment::PizzaDepartment(std::string address)
    : _address(address), _free_cookers(0),
      _menu({Pizza("Margherita"), Pizza("Pepperoni"), Pizza("Vegetarian")})
{
}
std::string PizzaDepartment::GetAddress()
{
    return _address;
}
const std::vector<Pizza> &PizzaDepartment::GetMenu() const
{
    return _menu;
}
void PizzaDepartment::TakeOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady)
{
    std::shared_ptr<PizzeriaWaiter> waiter;
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (_free_cookers == 0)
            throw AllCooksAreBusyException();
        auto least_busy = _waiters.begin();
        for (auto it = _waiters.begin(); it != _waiters.end(); ++it)
        {
            if (it->second < least_busy->second)
                least_busy = it;
        }

        waiter = least_busy->first;
        least_busy->second++;
    }
    {
        std::lock_guard<std::mutex> lock(_order_mtx);
        _active_orders++;
    }
    waiter->GetOrder(order, onOrderReady);
}
void PizzaDepartment::GiveOrderToCook(Order order, std::function<void(Order)> onOrderCooked)
{
    std::shared_ptr<PizzeriaCooker> cooker;
    {
        std::lock_guard<std::mutex> lock(mtx);
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
            cooker = free_cooker->first;
        }
    }
    if (cooker)
    {
        cooker->CookOrder(order, onOrderCooked);
        return;
    }
    std::shared_ptr<PizzeriaCookerIntern> intern;
    {
        std::lock_guard<std::mutex> lock(mtx);
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
            intern = free_intern->first;
        }
    }
    if(intern)
    {
        intern->CookOrder(order, onOrderCooked);
        return;
    }
}
void PizzaDepartment::CookingFinished(PizzeriaCookerIntern &intern)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
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
}
void PizzaDepartment::CookingFinished(PizzeriaCooker &cooker)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
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
}
void PizzaDepartment::DeliverFinished(PizzeriaWaiter &waiter)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto it = _waiters.begin(); it != _waiters.end(); ++it)
        {
            if (it->first.get() == &waiter)
            {
                it->second--;
                return;
            }
        }
    }
}
void PizzaDepartment::OrderFinished()
{
    std::lock_guard<std::mutex> lock(_order_mtx);
    _active_orders--;
    if (_active_orders == 0)
        _order_cv.notify_all();
}
void PizzaDepartment::WaitForAllOrders()
{
    std::unique_lock<std::mutex> lock(_order_mtx);
    _order_cv.wait(lock, [this] { return _active_orders == 0; });
}