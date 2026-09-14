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
    std::map<Pizza,int> GetOrderComponents();
    
private:
    std::map<Pizza, int> _pizzas;
    OrderStatus _status;
};