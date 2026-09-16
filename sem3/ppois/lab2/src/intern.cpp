#include "intern.hpp"

Intern::Intern(double time_factor) : _time_factor(time_factor), _completed_orders(0) {}

double Intern::GetTimeFactor() { return _time_factor; }
void Intern::SetTimeFactor(double factor) { _time_factor = factor; }

std::shared_ptr<PizzeriaCooker> Intern::GetMentor() { return _mentor; }
void Intern::SetMentor(std::shared_ptr<PizzeriaCooker> mentor) { _mentor = mentor; }

int Intern::GetCompletedOrders() { return _completed_orders; }
void Intern::IncrementCompletedOrders() { _completed_orders++; }
