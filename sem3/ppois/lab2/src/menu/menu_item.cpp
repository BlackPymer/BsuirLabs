#include "menu/menu_item.hpp"

MenuItem::MenuItem(std::string name, std::string category, double price, int weight, int calories)
    : _name(name), _category(category), _price(price), _weight(weight), _calories(calories)
{
}

std::string MenuItem::GetName() const { return _name; }
std::string MenuItem::GetCategory() const { return _category; }
double MenuItem::GetPrice() const { return _price; }
int MenuItem::GetWeight() const { return _weight; }
int MenuItem::GetCalories() const { return _calories; }
void MenuItem::SetPrice(double price) { _price = price; }
double MenuItem::GetTotalPrice() const { return _price; }
int MenuItem::GetTotalCalories() const { return _calories; }