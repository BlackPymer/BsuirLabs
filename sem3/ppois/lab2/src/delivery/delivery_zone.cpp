#include "delivery/delivery_zone.hpp"

DeliveryZone::DeliveryZone(std::string name, double base_fee, int estimated_minutes)
    : _name(name), _base_fee(base_fee), _estimated_minutes(estimated_minutes)
{
}

void DeliveryZone::AddCoveredStreet(std::string street)
{
    _covered_streets.push_back(street);
}

bool DeliveryZone::IsAddressInZone(std::shared_ptr<DeliveryAddress> address) const
{
    if (address == nullptr)
        return false;
    for (const auto &street : _covered_streets)
    {
        if (street == address->GetStreet())
            return true;
    }
    return false;
}

double DeliveryZone::GetFee() const { return _base_fee; }
void DeliveryZone::SetFee(double fee) { _base_fee = fee; }
int DeliveryZone::GetEstimatedMinutes() const { return _estimated_minutes; }
std::string DeliveryZone::GetName() const { return _name; }
int DeliveryZone::GetCoveredStreetCount() const { return _covered_streets.size(); }