#pragma once
#include "staff/pizzeria_worker.hpp"
#include "delivery/delivery_route.hpp"
#include <memory>
#include <string>

class Courier : virtual public PizzeriaWorker
{
public:
    Courier(int age, std::string name);
    void AcceptDelivery(std::shared_ptr<DeliveryRoute> route);
    void UpdateLocation(std::string location);
    bool CompleteDelivery();
    void AddRating(int stars);
    int GetCompletedDeliveries() const;
    double GetAverageRating() const;
    std::shared_ptr<DeliveryRoute> GetCurrentRoute() const;
    std::string GetLocation() const;

private:
    std::shared_ptr<DeliveryRoute> _current_route;
    std::string _location;
    int _completed_deliveries;
    int _rating_total;
    int _rating_count;
};