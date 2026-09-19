#pragma once
#include <string>

class MenuItem
{
public:
    MenuItem(std::string name, std::string category, double price, int weight, int calories);
    std::string GetName() const;
    std::string GetCategory() const;
    double GetPrice() const;
    int GetWeight() const;
    int GetCalories() const;
    void SetPrice(double price);
    virtual double GetTotalPrice() const;
    virtual int GetTotalCalories() const;

private:
    std::string _name;
    std::string _category;
    double _price;
    int _weight;
    int _calories;
};