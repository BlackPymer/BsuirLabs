#include "payment/coupon.hpp"

Coupon::Coupon(std::string code, double discount_percent, int expiry_day, int max_uses, double min_order)
    : _code(code), _discount_percent(discount_percent), _expiry_day(expiry_day),
      _max_uses(max_uses), _used_count(0), _min_order(min_order)
{
}

bool Coupon::IsExpired(int current_day) const { return current_day > _expiry_day; }

bool Coupon::Validate(double order_total, int current_day) const
{
    return !IsExpired(current_day) && _used_count < _max_uses && order_total >= _min_order;
}

bool Coupon::Apply()
{
    if (_used_count >= _max_uses)
        return false;
    _used_count++;
    return true;
}

double Coupon::GetDiscount() const { return _discount_percent; }
std::string Coupon::GetCode() const { return _code; }
int Coupon::GetUsedCount() const { return _used_count; }