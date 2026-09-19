#include "payment/payment.hpp"

Payment::Payment(double amount) : _status(PAYMENT_PENDING), _amount(amount)
{
}

bool Payment::Refund()
{
    if (_amount <= 0)
        return false;
    _status = PAYMENT_REFUNDED;
    return true;
}

double Payment::GetChange() const { return 0; }
double Payment::GetAmount() const { return _amount; }
PaymentStatus Payment::GetStatus() const { return _status; }