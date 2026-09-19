#pragma once
#include "delivery/delivery_zone.hpp"
#include <memory>

class DeliveryFeeCalculator
{
public:
    DeliveryFeeCalculator(double per_stop_fee, double rush_multiplier);
    double CalculateFee(std::shared_ptr<DeliveryZone> zone, int stop_count, bool is_rush_hour) const;
    double GetPerStopFee() const;

private:
    double _per_stop_fee;
    double _rush_multiplier;
};