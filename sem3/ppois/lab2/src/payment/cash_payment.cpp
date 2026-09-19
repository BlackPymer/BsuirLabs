#include "payment/cash_payment.hpp"
#include "common/pizzeria_exceptions.hpp"

CashPayment::CashPayment(double amount, double paid_amount)
    : Payment(amount), _paid_amount(paid_amount), _change(0)
{
}

bool CashPayment::Process()
{
    if (_paid_amount < GetAmount())
    {
        _status = PAYMENT_DECLINED;
        throw InsufficientFundsException();
    }
    _change = _paid_amount - GetAmount();
    _status = PAYMENT_PAID;
    return true;
}

double CashPayment::GetChange() const { return _change; }