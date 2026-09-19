#pragma once
#include "menu/menu_item.hpp"
#include <map>
#include <memory>
#include <string>

class Receipt
{
public:
    Receipt(int number);
    void AddLine(std::shared_ptr<MenuItem> item, int count);
    void SetTotal(double total);
    double GetTotal() const;
    int GetNumber() const;
    int GetLineCount() const;
    std::string Print() const;

private:
    int _number;
    double _total;
    double _discount;
    std::map<std::shared_ptr<MenuItem>, int> _lines;
};