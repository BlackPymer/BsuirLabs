#include "delivery/delivery_address.hpp"

DeliveryAddress::DeliveryAddress(std::string street, std::string building, std::string apartment, int floor, std::string phone)
    : _street(street), _building(building), _apartment(apartment), _floor(floor), _phone(phone)
{
}

std::string DeliveryAddress::GetStreet() const { return _street; }
std::string DeliveryAddress::GetFullAddress() const { return _street + ", " + _building + ", apt. " + _apartment; }
std::string DeliveryAddress::GetPhone() const { return _phone; }
int DeliveryAddress::GetFloor() const { return _floor; }