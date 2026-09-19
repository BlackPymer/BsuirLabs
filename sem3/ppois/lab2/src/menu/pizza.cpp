#include "menu/pizza.hpp"

Pizza::Pizza(std::string name)
    : MenuItem(name, "pizza", 200, 400, 800), _size(MEDIUM), _size_multiplier(1.0)
{
}

Pizza::Pizza(std::string name, double base_price, int weight, int calories)
    : MenuItem(name, "pizza", base_price, weight, calories), _size(MEDIUM), _size_multiplier(1.0)
{
}

void Pizza::SetSize(PizzaSize size)
{
    _size = size;
    _size_multiplier = (size == SMALL) ? 0.8 : (size == LARGE) ? 1.2 : 1.0;
}

PizzaSize Pizza::GetSize() const { return _size; }

void Pizza::AddTopping(std::shared_ptr<Ingredient> ingredient, int amount)
{
    _toppings[ingredient] += amount;
}

void Pizza::RemoveTopping(std::shared_ptr<Ingredient> ingredient)
{
    _toppings.erase(ingredient);
}

double Pizza::CalculatePrice() const
{
    double total = GetPrice();
    for (const auto &p : _toppings)
        total += p.first->GetCost(p.second);
    return total * _size_multiplier;
}

int Pizza::CalculateCalories() const
{
    int total = GetCalories();
    for (const auto &p : _toppings)
        total += p.first->GetCalories(p.second);
    return total;
}

double Pizza::GetTotalPrice() const { return CalculatePrice(); }
int Pizza::GetTotalCalories() const { return CalculateCalories(); }
bool Pizza::operator<(const Pizza &other) const { return GetName() < other.GetName(); }