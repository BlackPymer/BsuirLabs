#pragma once
#include <functional>
#include <string>
#include <vector>

class DeliveryTracker
{
public:
    DeliveryTracker();
    void StartTracking(int order_id);
    void UpdateStatus(std::string status);
    void SetOnUpdate(std::function<void(std::string)> on_update);
    std::string GetStatus() const;
    int GetUpdateCount() const;
    int GetOrderId() const;
    bool IsTracking() const;

private:
    int _order_id;
    bool _tracking;
    std::string _current_status;
    std::vector<std::string> _updates;
    std::function<void(std::string)> _on_update;
};