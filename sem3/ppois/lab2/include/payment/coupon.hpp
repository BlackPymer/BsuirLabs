#pragma once
#include <string>

class Coupon
{
public:
    Coupon(std::string code, double discount_percent, int expiry_day, int max_uses, double min_order);
    bool IsExpired(int current_day) const;
    bool Validate(double order_total, int current_day) const;
    bool Apply();
    double GetDiscount() const;
    std::string GetCode() const;
    int GetUsedCount() const;

private:
    std::string _code;
    double _discount_percent;
    int _expiry_day;
    int _max_uses;
    int _used_count;
    double _min_order;
};