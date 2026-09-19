#include "payment/card_payment.hpp"

CardPayment::CardPayment(double amount, std::string card_last_four)
    : Payment(amount), _card_last_four(card_last_four), _authorized(false), _captured(false)
{
}

bool CardPayment::Authorize()
{
    _authorized = _card_last_four.size() == 4;
    return _authorized;
}

bool CardPayment::Capture()
{
    if (!_authorized)
    {
        _status = PAYMENT_DECLINED;
        return false;
    }
    _captured = true;
    _status = PAYMENT_PAID;
    return true;
}

bool CardPayment::Process() { return Authorize() && Capture(); }

bool CardPayment::Refund()
{
    if (!_captured)
        return false;
    _status = PAYMENT_REFUNDED;
    return true;
}

std::string CardPayment::GetCardLastFour() const { return _card_last_four; }