#include "kitchen/kitchen.hpp"
#include "kitchen/oven.hpp"
#include "staff/pizzeria_cooker.hpp"
#include "common/pizzeria_exceptions.hpp"

Kitchen::Kitchen() : _open(false), _active_orders(0)
{
}

void Kitchen::OpenKitchen() { _open = true; }
void Kitchen::CloseKitchen() { _open = false; }
bool Kitchen::IsOpen() const { return _open; }

void Kitchen::AddOven(std::shared_ptr<Oven> oven)
{
    _ovens.push_back(oven);
}

int Kitchen::GetOvenCount() const { return _ovens.size(); }

void Kitchen::AssignOrder(std::shared_ptr<PizzeriaCooker> cooker, Order order, std::function<void(Order)> onOrderCooked)
{
    if (order.GetPizzaCount() == 0)
        throw EmptyOrderException();
    _active_orders++;
    cooker->CookOrder(order, [this, onOrderCooked](Order cooked)
                      {
        _active_orders--;
        onOrderCooked(cooked); });
}

int Kitchen::GetActiveOrders() const { return _active_orders; }