#pragma once
#include "staff/pizzeria_worker.hpp"
#include "order/order.hpp"
#include <string>
#include <vector>

class CallCenterOperator : virtual public PizzeriaWorker
{
public:
    CallCenterOperator(int age, std::string name);
    int AnswerCall();
    void TakeOrder(const std::vector<std::pair<Pizza, int>> &order);
    void TransferToKitchen();
    void EndCall();
    int GetOrdersTaken() const;
    int GetCallsHandled() const;

private:
    bool _active_line;
    int _calls_handled;
    int _orders_taken;
    std::vector<std::pair<Pizza, int>> _current_order;
};