#pragma once
#include "payment/payment.hpp"
#include <string>

class CardPayment : public Payment
{
public:
    CardPayment(double amount, std::string card_last_four);
    bool Authorize();
    bool Capture();
    bool Process() override;
    bool Refund() override;
    std::string GetCardLastFour() const;

private:
    std::string _card_last_four;
    bool _authorized;
    bool _captured;
};