#pragma once
#include "menu/ingredient.hpp"
#include <map>
#include <memory>

class Refrigerator
{
public:
    Refrigerator();
    void Store(std::shared_ptr<Ingredient> ingredient, int quantity, int expiry_day);
    bool Retrieve(std::shared_ptr<Ingredient> ingredient, int quantity);
    int GetQuantity(std::shared_ptr<Ingredient> ingredient) const;
    void SetTemperature(int temperature);
    int GetTemperature() const;
    int DiscardExpired(int current_day);
    int GetCapacity() const;

private:
    int _temperature;
    int _capacity;
    std::map<std::shared_ptr<Ingredient>, int> _content;
    std::map<std::shared_ptr<Ingredient>, int> _expiry;
};