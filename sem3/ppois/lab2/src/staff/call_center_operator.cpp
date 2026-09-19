#include "staff/call_center_operator.hpp"

CallCenterOperator::CallCenterOperator(int age, std::string name)
    : PizzeriaWorker(age, name), Human(age, name), _active_line(false), _calls_handled(0), _orders_taken(0)
{
}

int CallCenterOperator::AnswerCall()
{
    _active_line = true;
    _calls_handled++;
    return _calls_handled;
}

void CallCenterOperator::TakeOrder(const std::vector<std::pair<Pizza, int>> &order)
{
    _current_order = order;
    _orders_taken++;
}

void CallCenterOperator::TransferToKitchen()
{
    _active_line = false;
}

void CallCenterOperator::EndCall()
{
    _active_line = false;
    _current_order.clear();
}

int CallCenterOperator::GetOrdersTaken() const { return _orders_taken; }
int CallCenterOperator::GetCallsHandled() const { return _calls_handled; }