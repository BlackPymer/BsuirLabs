#include "delivery/delivery_tracker.hpp"
#include "common/pizzeria_exceptions.hpp"

DeliveryTracker::DeliveryTracker() : _order_id(0), _tracking(false), _current_status("none")
{
}

void DeliveryTracker::StartTracking(int order_id)
{
    _order_id = order_id;
    _tracking = true;
    _current_status = "pending";
}

void DeliveryTracker::UpdateStatus(std::string status)
{
    if (!_tracking)
        throw NoSuchOrderException();
    _current_status = status;
    _updates.push_back(status);
    if (_on_update)
        _on_update(status);
}

void DeliveryTracker::SetOnUpdate(std::function<void(std::string)> on_update)
{
    _on_update = on_update;
}

std::string DeliveryTracker::GetStatus() const { return _current_status; }
int DeliveryTracker::GetUpdateCount() const { return _updates.size(); }
int DeliveryTracker::GetOrderId() const { return _order_id; }
bool DeliveryTracker::IsTracking() const { return _tracking; }