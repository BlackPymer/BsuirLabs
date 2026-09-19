#pragma once
#include "staff/pizzeria_worker.hpp"

class Accountant : virtual public PizzeriaWorker
{
public:
    Accountant(int age, std::string name);
    void RecordRevenue(double amount);
    void RecordExpense(double amount);
    void RecordPayroll(double amount);
    double GetTotalRevenue() const;
    double GetTotalExpenses() const;
    double GetNetProfit() const;
    double CalculateSalary(double hourly_rate, int hours) const;

private:
    double _total_revenue;
    double _total_expenses;
    double _total_payroll;
};