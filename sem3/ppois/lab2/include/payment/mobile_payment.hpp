#pragma once
#include "payment/payment.hpp"
#include <string>

class MobilePayment : public Payment
{
public:
    MobilePayment(double amount, std::string phone, std::string token);
    bool ValidateToken() const;
    bool Process() override;
    std::string GetPhone() const;

private:
    std::string _phone;
    std::string _token;
};