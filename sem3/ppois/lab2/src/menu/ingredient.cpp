#include "menu/ingredient.hpp"

Ingredient::Ingredient(std::string name, double cost_per_unit, int calories_per_unit, bool perishable, int shelf_life_days)
    : _name(name), _cost_per_unit(cost_per_unit), _calories_per_unit(calories_per_unit),
      _is_perishable(perishable), _shelf_life_days(shelf_life_days)
{
}

std::string Ingredient::GetName() const { return _name; }
double Ingredient::GetCost(int amount) const { return _cost_per_unit * amount; }
int Ingredient::GetCalories(int amount) const { return _calories_per_unit * amount; }
bool Ingredient::IsPerishable() const { return _is_perishable; }
int Ingredient::GetShelfLifeDays() const { return _shelf_life_days; }
void Ingredient::SetCost(double cost_per_unit) { _cost_per_unit = cost_per_unit; }