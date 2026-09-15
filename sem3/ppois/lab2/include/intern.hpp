#pragma once
#include <memory>

class PizzeriaCooker;

class Intern
{
public:
    Intern(double time_factor = 2.0);
    double GetTimeFactor();
    void SetTimeFactor(double factor);
    std::shared_ptr<PizzeriaCooker> GetMentor();
    void SetMentor(std::shared_ptr<PizzeriaCooker> mentor);
    int GetCompletedOrders();
    void IncrementCompletedOrders();

protected:
    double _time_factor;

private:
    std::shared_ptr<PizzeriaCooker> _mentor;
    int _completed_orders;
};
