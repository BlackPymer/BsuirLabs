#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <vector>
#include "human/human.hpp"
#include "staff/pizzeria_worker.hpp"
#include "staff/pizzeria_cooker.hpp"
#include "staff/pizzeria_cooker_intern.hpp"
#include "staff/pizzeria_waiter.hpp"
#include "staff/cashier.hpp"
#include "staff/call_center_operator.hpp"
#include "staff/pizzeria_manager.hpp"
#include "staff/accountant.hpp"
#include "staff/hostess.hpp"
#include "staff/courier.hpp"
#include "menu/pizza.hpp"
#include <condition_variable>
#include <mutex>

class PizzaDepartment : public std::enable_shared_from_this<PizzaDepartment>
{
public:
    PizzaDepartment(std::string address);
    std::string GetAddress();
    const std::vector<Pizza> &GetMenu() const;
    void TakeOrder(std::vector<std::pair<Pizza, int>> order, std::function<void()> onOrderReady);
    void GiveOrderToCook(Order order, std::function<void(Order)> onOrderCooked);
    void OrderFinished();
    void WaitForAllOrders();
    int GetStaffCount();

    template <typename T>
    std::shared_ptr<T> Hire(const Human &human);

    void CookingFinished(PizzeriaCookerIntern &intern);
    void CookingFinished(PizzeriaCooker &cooker);
    void DeliverFinished(PizzeriaWaiter &waiter);
    class AllCooksAreBusyException : public std::exception
    {
    };

private:
    std::mutex mtx;
    std::string _address;
    const std::vector<Pizza> _menu;
    std::map<std::shared_ptr<PizzeriaWaiter>, int> _waiters;
    std::map<std::shared_ptr<PizzeriaCooker>, bool> _cookers;
    std::map<std::shared_ptr<PizzeriaCookerIntern>, bool> _interns;
    std::map<std::shared_ptr<PizzeriaWorker>, int> _staff;
    int _free_cookers;
    int _active_orders = 0;
    std::mutex _order_mtx;
    std::condition_variable _order_cv;
};
