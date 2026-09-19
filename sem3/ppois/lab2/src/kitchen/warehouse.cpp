#include "kitchen/warehouse.hpp"
#include "common/pizzeria_exceptions.hpp"

Warehouse::Warehouse() : _reorder_level(0), _restock_pending(false)
{
}

void Warehouse::Restock(std::shared_ptr<Ingredient> ingredient, int quantity)
{
    _stock[ingredient] += quantity;
    _restock_pending = false;
}

bool Warehouse::CheckAvailability(std::shared_ptr<Ingredient> ingredient, int quantity) const
{
    auto it = _stock.find(ingredient);
    return it != _stock.end() && it->second >= quantity;
}

void Warehouse::Take(std::shared_ptr<Ingredient> ingredient, int quantity)
{
    auto it = _stock.find(ingredient);
    if (it == _stock.end() || it->second < quantity)
        throw ItemOutOfStockException();
    it->second -= quantity;
    _restock_pending = it->second <= _reorder_level;
}

int Warehouse::GetStock(std::shared_ptr<Ingredient> ingredient) const
{
    auto it = _stock.find(ingredient);
    if (it == _stock.end())
        return 0;
    return it->second;
}

void Warehouse::SetReorderLevel(int level) { _reorder_level = level; }
int Warehouse::GetReorderLevel() const { return _reorder_level; }
bool Warehouse::IsRestockPending() const { return _restock_pending; }