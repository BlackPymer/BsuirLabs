#include "order.hpp"

Order::Order(std::vector<std::pair<Pizza, int>> pizzas)
{
    for (const std::pair<Pizza, int> &a : pizzas)
    {
        _pizzas[a.first] = a.second;
    }
    _status = PENDING;
}
