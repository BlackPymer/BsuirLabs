#include "delivery/delivery_fee_calculator.hpp"

DeliveryFeeCalculator::DeliveryFeeCalculator(double per_stop_fee, double rush_multiplier)
    : _per_stop_fee(per_stop_fee), _rush_multiplier(rush_multiplier)
{
}

double DeliveryFeeCalculator::CalculateFee(std::shared_ptr<DeliveryZone> zone, int stop_count, bool is_rush_hour) const
{
    double base = zone->GetFee() + _per_stop_fee * stop_count;
    if (is_rush_hour)
        base *= _rush_multiplier;
    return base;
}

double DeliveryFeeCalculator::GetPerStopFee() const { return _per_stop_fee; }