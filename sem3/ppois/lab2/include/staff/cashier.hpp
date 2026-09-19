#pragma once
#include "staff/pizzeria_worker.hpp"
#include "payment/payment.hpp"
#include <memory>

class Cashier : virtual public PizzeriaWorker
{
public:
    Cashier(int age, std::string name);
    void OpenShift(double starting_cash);
    double CloseShift();
    double AcceptPayment(std::shared_ptr<Payment> payment, double price);
    bool IsShiftOpen() const;
    double GetCashDrawer() const;

private:
    double _cash_drawer;
    bool _shift_open;
};