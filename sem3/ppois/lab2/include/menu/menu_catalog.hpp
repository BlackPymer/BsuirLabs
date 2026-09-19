#pragma once
#include "menu/menu_item.hpp"
#include <memory>
#include <string>
#include <vector>

class MenuCatalog
{
public:
    MenuCatalog(std::string name);
    void AddItem(std::shared_ptr<MenuItem> item);
    void RemoveItem(std::string name);
    std::shared_ptr<MenuItem> FindByName(std::string name) const;
    std::vector<std::shared_ptr<MenuItem>> GetAllByCategory(std::string category) const;
    void UpdatePrice(std::string name, double price);
    int GetItemCount() const;
    std::string GetName() const;

private:
    std::string _name;
    std::vector<std::shared_ptr<MenuItem>> _items;
};