#include "kitchen/quality_controller.hpp"

QualityController::QualityController() : _approved(0), _rejected(0)
{
}

bool QualityController::InspectOrder(Order order) const
{
    return order.GetStatus() != PENDING;
}

void QualityController::ApproveOrder(Order order)
{
    if (order.GetStatus() != PENDING)
        _approved++;
}

void QualityController::RejectOrder(Order order)
{
    if (order.GetStatus() == PENDING)
        _rejected++;
}

int QualityController::GetApprovedCount() const { return _approved; }
int QualityController::GetRejectedCount() const { return _rejected; }