#include "menu/combo_set.hpp"

ComboSet::ComboSet(std::string name, double discount_percent)
    : MenuItem(name, "combo", 0, 0, 0), _discount_percent(discount_percent)
{
}

void ComboSet::AddItem(std::shared_ptr<MenuItem> item, int count)
{
    _items[item] += count;
}

void ComboSet::RemoveItem(std::shared_ptr<MenuItem> item)
{
    _items.erase(item);
}

int ComboSet::GetItemCount() const
{
    int total = 0;
    for (const auto &p : _items)
        total += p.second;
    return total;
}

double ComboSet::GetTotalPrice() const
{
    double total = 0;
    for (const auto &p : _items)
        total += p.first->GetTotalPrice() * p.second;
    return total * (1.0 - _discount_percent / 100.0);
}

int ComboSet::GetTotalCalories() const
{
    int total = 0;
    for (const auto &p : _items)
        total += p.first->GetTotalCalories() * p.second;
    return total;
}