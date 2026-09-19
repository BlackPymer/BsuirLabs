#pragma once
#include "menu/ingredient.hpp"
#include <map>
#include <memory>

class Warehouse
{
public:
    Warehouse();
    void Restock(std::shared_ptr<Ingredient> ingredient, int quantity);
    bool CheckAvailability(std::shared_ptr<Ingredient> ingredient, int quantity) const;
    void Take(std::shared_ptr<Ingredient> ingredient, int quantity);
    int GetStock(std::shared_ptr<Ingredient> ingredient) const;
    void SetReorderLevel(int level);
    int GetReorderLevel() const;
    bool IsRestockPending() const;

private:
    std::map<std::shared_ptr<Ingredient>, int> _stock;
    int _reorder_level;
    bool _restock_pending;
};