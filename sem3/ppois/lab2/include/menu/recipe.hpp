#pragma once
#include "menu/ingredient.hpp"
#include <map>
#include <memory>
#include <string>

class Recipe
{
public:
    Recipe(std::string name, int portions, int prep_time_seconds);
    void AddIngredient(std::shared_ptr<Ingredient> ingredient, int amount);
    int GetPortions() const;
    int GetPrepTimeSeconds() const;
    double GetCostPerPortion() const;
    int GetCaloriesPerPortion() const;
    const std::map<std::shared_ptr<Ingredient>, int> &GetIngredients() const;

private:
    std::string _name;
    int _portions;
    int _prep_time_seconds;
    std::map<std::shared_ptr<Ingredient>, int> _ingredients;
};