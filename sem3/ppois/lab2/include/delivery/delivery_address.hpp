#pragma once
#include <string>

class DeliveryAddress
{
public:
    DeliveryAddress(std::string street, std::string building, std::string apartment, int floor, std::string phone);
    std::string GetStreet() const;
    std::string GetFullAddress() const;
    std::string GetPhone() const;
    int GetFloor() const;

private:
    std::string _street;
    std::string _building;
    std::string _apartment;
    int _floor;
    std::string _phone;
};