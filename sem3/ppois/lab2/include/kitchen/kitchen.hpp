#pragma once
#include "order/order.hpp"
#include <functional>
#include <memory>
#include <vector>

class Oven;
class PizzeriaCooker;

class Kitchen
{
public:
    Kitchen();
    void OpenKitchen();
    void CloseKitchen();
    bool IsOpen() const;
    void AddOven(std::shared_ptr<Oven> oven);
    int GetOvenCount() const;
    void AssignOrder(std::shared_ptr<PizzeriaCooker> cooker, Order order, std::function<void(Order)> onOrderCooked);
    int GetActiveOrders() const;

private:
    bool _open;
    int _active_orders;
    std::vector<std::shared_ptr<Oven>> _ovens;
};