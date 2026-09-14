#pragma once
#include "pizza.hpp"
#include <map>

enum OrderStatus
{
    PENDING,
    IN_PROGRESS,
    DELIVERED
};

class Order
{
public:
    Order(std::map<Pizza, int> pizzas);
    OrderStatus GetStatus();
    void SetStatus(OrderStatus status);
    std::map<Pizza, int> GetOrderComponents();
    int GetPizzaCount();
    bool operator<(const Order &other) const;

private:
    std::map<Pizza, int> _pizzas;
    OrderStatus _status;
};