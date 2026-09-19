#pragma once
#include "delivery/delivery_address.hpp"
#include <memory>
#include <string>
#include <vector>

class DeliveryZone
{
public:
    DeliveryZone(std::string name, double base_fee, int estimated_minutes);
    void AddCoveredStreet(std::string street);
    bool IsAddressInZone(std::shared_ptr<DeliveryAddress> address) const;
    double GetFee() const;
    void SetFee(double fee);
    int GetEstimatedMinutes() const;
    std::string GetName() const;
    int GetCoveredStreetCount() const;

private:
    std::string _name;
    double _base_fee;
    int _estimated_minutes;
    std::vector<std::string> _covered_streets;
};