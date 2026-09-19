#pragma once
#include "payment/payment.hpp"

class CashPayment : public Payment
{
public:
    CashPayment(double amount, double paid_amount);
    bool Process() override;
    double GetChange() const override;

private:
    double _paid_amount;
    double _change;
};