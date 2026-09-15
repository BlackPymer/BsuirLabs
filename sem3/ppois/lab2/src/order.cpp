#include "order.hpp"

Order::Order(std::vector<std::pair<Pizza, int>> pizzas)
{
    for (const std::pair<Pizza, int> &a : pizzas)
    {
        _pizzas[a.first] = a.second;
    }
    _status = PENDING;
}
OrderStatus Order::GetStatus() { return _status; }
void Order::SetStatus(OrderStatus status) { _status = status; }
std::map<Pizza, int> Order::GetOrderComponents() { return _pizzas; }
int Order::GetPizzaCount() { return _pizzas.size(); }
bool Order::operator<(const Order &order) const
{
    return _pizzas.size() < order._pizzas.size();
}