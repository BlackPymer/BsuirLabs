#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <vector>
#include "human.hpp"
#include "pizzeria_cooker.hpp"
#include "pizzeria_cooker_intern.hpp"
#include "pizzeria_waiter.hpp"

class PizzaDepartment : public std::enable_shared_from_this<PizzaDepartment>
{
public:
    PizzaDepartment(std::string address, int waiters, int cooks, int interns = 0);
    std::string GetAddress();
    void TakeOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady);
    void GiveOrderToCook(Order order, std::function<void()> onOrderCooked);

    template <typename T>
    std::shared_ptr<T> Hire(const Human &human);

    void CookingFinished(PizzeriaCookerIntern &intern);
    void CookingFinished(PizzeriaCooker &cooker);
    void DeliverFinished(PizzeriaWaiter &waiter);
    class AllCooksAreBusyException : public std::exception
    {
    };

private:
    std::string _address;
    std::map<std::shared_ptr<PizzeriaWaiter>, int> _waiters;
    std::map<std::shared_ptr<PizzeriaCooker>, bool> _cookers;
    std::map<std::shared_ptr<PizzeriaCookerIntern>, bool> _interns;
    int _free_cookers;
};
