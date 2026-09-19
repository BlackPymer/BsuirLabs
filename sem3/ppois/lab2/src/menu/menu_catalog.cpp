#include "menu/menu_catalog.hpp"

MenuCatalog::MenuCatalog(std::string name) : _name(name)
{
}

void MenuCatalog::AddItem(std::shared_ptr<MenuItem> item)
{
    _items.push_back(item);
}

void MenuCatalog::RemoveItem(std::string name)
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
    {
        if ((*it)->GetName() == name)
        {
            _items.erase(it);
            return;
        }
    }
}

std::shared_ptr<MenuItem> MenuCatalog::FindByName(std::string name) const
{
    for (const auto &item : _items)
    {
        if (item->GetName() == name)
            return item;
    }
    return nullptr;
}

std::vector<std::shared_ptr<MenuItem>> MenuCatalog::GetAllByCategory(std::string category) const
{
    std::vector<std::shared_ptr<MenuItem>> result;
    for (const auto &item : _items)
    {
        if (item->GetCategory() == category)
            result.push_back(item);
    }
    return result;
}

void MenuCatalog::UpdatePrice(std::string name, double price)
{
    auto item = FindByName(name);
    if (item != nullptr)
        item->SetPrice(price);
}

int MenuCatalog::GetItemCount() const { return _items.size(); }
std::string MenuCatalog::GetName() const { return _name; }