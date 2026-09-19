#pragma once
#include "menu/menu_item.hpp"
#include "menu/ingredient.hpp"
#include <map>
#include <memory>
#include <string>

enum PizzaSize
{
    SMALL,
    MEDIUM,
    LARGE
};

class Pizza : public MenuItem
{
public:
    Pizza(std::string name);
    Pizza(std::string name, double base_price, int weight, int calories);
    void SetSize(PizzaSize size);
    PizzaSize GetSize() const;
    void AddTopping(std::shared_ptr<Ingredient> ingredient, int amount);
    void RemoveTopping(std::shared_ptr<Ingredient> ingredient);
    double CalculatePrice() const;
    int CalculateCalories() const;
    double GetTotalPrice() const override;
    int GetTotalCalories() const override;
    bool operator<(const Pizza &other) const;

private:
    PizzaSize _size;
    double _size_multiplier;
    std::map<std::shared_ptr<Ingredient>, int> _toppings;
};