#include "kitchen/refrigerator.hpp"
#include "common/pizzeria_exceptions.hpp"

Refrigerator::Refrigerator() : _temperature(4), _capacity(0)
{
}

void Refrigerator::Store(std::shared_ptr<Ingredient> ingredient, int quantity, int expiry_day)
{
    _content[ingredient] += quantity;
    _expiry[ingredient] = expiry_day;
    _capacity += quantity;
}

bool Refrigerator::Retrieve(std::shared_ptr<Ingredient> ingredient, int quantity)
{
    auto it = _content.find(ingredient);
    if (it == _content.end() || it->second < quantity)
        throw ItemOutOfStockException();
    it->second -= quantity;
    _capacity -= quantity;
    if (it->second == 0)
        _content.erase(it);
    return true;
}

int Refrigerator::GetQuantity(std::shared_ptr<Ingredient> ingredient) const
{
    auto it = _content.find(ingredient);
    if (it == _content.end())
        return 0;
    return it->second;
}

void Refrigerator::SetTemperature(int temperature) { _temperature = temperature; }
int Refrigerator::GetTemperature() const { return _temperature; }

int Refrigerator::DiscardExpired(int current_day)
{
    int discarded = 0;
    for (auto it = _expiry.begin(); it != _expiry.end();)
    {
        if (it->second < current_day)
        {
            auto content_it = _content.find(it->first);
            if (content_it != _content.end())
            {
                discarded += content_it->second;
                _capacity -= content_it->second;
                _content.erase(content_it);
            }
            it = _expiry.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return discarded;
}

int Refrigerator::GetCapacity() const { return _capacity; }