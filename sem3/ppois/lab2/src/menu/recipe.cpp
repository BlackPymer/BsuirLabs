#include "menu/recipe.hpp"

Recipe::Recipe(std::string name, int portions, int prep_time_seconds)
    : _name(name), _portions(portions), _prep_time_seconds(prep_time_seconds)
{
}

void Recipe::AddIngredient(std::shared_ptr<Ingredient> ingredient, int amount)
{
    _ingredients[ingredient] += amount;
}

int Recipe::GetPortions() const { return _portions; }
int Recipe::GetPrepTimeSeconds() const { return _prep_time_seconds; }

double Recipe::GetCostPerPortion() const
{
    double total = 0;
    for (const auto &p : _ingredients)
        total += p.first->GetCost(p.second);
    return total / _portions;
}

int Recipe::GetCaloriesPerPortion() const
{
    int total = 0;
    for (const auto &p : _ingredients)
        total += p.first->GetCalories(p.second);
    return total / _portions;
}

const std::map<std::shared_ptr<Ingredient>, int> &Recipe::GetIngredients() const { return _ingredients; }