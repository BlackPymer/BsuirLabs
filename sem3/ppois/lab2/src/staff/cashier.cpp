#include "staff/cashier.hpp"
#include "common/pizzeria_exceptions.hpp"

Cashier::Cashier(int age, std::string name)
    : PizzeriaWorker(age, name), Human(age, name), _cash_drawer(0), _shift_open(false)
{
}

void Cashier::OpenShift(double starting_cash)
{
    _shift_open = true;
    _cash_drawer = starting_cash;
}

double Cashier::CloseShift()
{
    _shift_open = false;
    return _cash_drawer;
}

double Cashier::AcceptPayment(std::shared_ptr<Payment> payment, double price)
{
    if (payment == nullptr || !payment->Process())
        throw PaymentDeclinedException();
    _cash_drawer += price;
    return payment->GetChange();
}

bool Cashier::IsShiftOpen() const { return _shift_open; }
double Cashier::GetCashDrawer() const { return _cash_drawer; }