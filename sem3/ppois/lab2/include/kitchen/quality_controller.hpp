#pragma once
#include "order/order.hpp"

class QualityController
{
public:
    QualityController();
    bool InspectOrder(Order order) const;
    void ApproveOrder(Order order);
    void RejectOrder(Order order);
    int GetApprovedCount() const;
    int GetRejectedCount() const;

private:
    int _approved;
    int _rejected;
};