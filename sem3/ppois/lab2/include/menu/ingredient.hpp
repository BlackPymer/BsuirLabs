#pragma once
#include <string>

class Ingredient
{
public:
    Ingredient(std::string name, double cost_per_unit, int calories_per_unit, bool perishable, int shelf_life_days);
    std::string GetName() const;
    double GetCost(int amount) const;
    int GetCalories(int amount) const;
    bool IsPerishable() const;
    int GetShelfLifeDays() const;
    void SetCost(double cost_per_unit);

private:
    std::string _name;
    double _cost_per_unit;
    int _calories_per_unit;
    bool _is_perishable;
    int _shelf_life_days;
};