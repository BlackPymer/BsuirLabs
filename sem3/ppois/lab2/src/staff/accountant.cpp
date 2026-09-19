#include "staff/accountant.hpp"

Accountant::Accountant(int age, std::string name)
    : PizzeriaWorker(age, name), Human(age, name), _total_revenue(0), _total_expenses(0), _total_payroll(0)
{
}

void Accountant::RecordRevenue(double amount) { _total_revenue += amount; }
void Accountant::RecordExpense(double amount) { _total_expenses += amount; }
void Accountant::RecordPayroll(double amount) { _total_payroll += amount; }
double Accountant::GetTotalRevenue() const { return _total_revenue; }
double Accountant::GetTotalExpenses() const { return _total_expenses + _total_payroll; }
double Accountant::GetNetProfit() const { return _total_revenue - GetTotalExpenses(); }
double Accountant::CalculateSalary(double hourly_rate, int hours) const { return hourly_rate * hours; }