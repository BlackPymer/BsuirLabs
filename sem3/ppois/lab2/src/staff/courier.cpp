#include "staff/courier.hpp"
#include "common/pizzeria_exceptions.hpp"

Courier::Courier(int age, std::string name)
    : PizzeriaWorker(age, name), Human(age, name), _location("pizzeria"),
      _completed_deliveries(0), _rating_total(0), _rating_count(0)
{
}

void Courier::AcceptDelivery(std::shared_ptr<DeliveryRoute> route)
{
    if (route == nullptr || route->GetStopCount() == 0)
        throw DeliveryException("Courier cannot accept an empty route");
    _current_route = route;
}

void Courier::UpdateLocation(std::string location) { _location = location; }

bool Courier::CompleteDelivery()
{
    if (_current_route == nullptr)
        return false;
    _completed_deliveries++;
    _current_route = nullptr;
    return true;
}

void Courier::AddRating(int stars)
{
    _rating_total += stars;
    _rating_count++;
}

int Courier::GetCompletedDeliveries() const { return _completed_deliveries; }

double Courier::GetAverageRating() const
{
    if (_rating_count == 0)
        return 0;
    return (double)_rating_total / _rating_count;
}

std::shared_ptr<DeliveryRoute> Courier::GetCurrentRoute() const { return _current_route; }
std::string Courier::GetLocation() const { return _location; }