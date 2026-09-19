#pragma once
#include "menu/menu_item.hpp"
#include <map>
#include <memory>
#include <string>

class ComboSet : public MenuItem
{
public:
    ComboSet(std::string name, double discount_percent);
    void AddItem(std::shared_ptr<MenuItem> item, int count);
    void RemoveItem(std::shared_ptr<MenuItem> item);
    int GetItemCount() const;
    double GetTotalPrice() const override;
    int GetTotalCalories() const override;

private:
    std::map<std::shared_ptr<MenuItem>, int> _items;
    double _discount_percent;
};