#include "payment/mobile_payment.hpp"
#include "common/pizzeria_exceptions.hpp"

MobilePayment::MobilePayment(double amount, std::string phone, std::string token)
    : Payment(amount), _phone(phone), _token(token)
{
}

bool MobilePayment::ValidateToken() const { return _token.size() >= 4; }

bool MobilePayment::Process()
{
    if (!ValidateToken())
    {
        _status = PAYMENT_DECLINED;
        throw PaymentDeclinedException();
    }
    _status = PAYMENT_PAID;
    return true;
}

std::string MobilePayment::GetPhone() const { return _phone; }