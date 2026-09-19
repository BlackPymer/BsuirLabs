#include "payment/loyalty_program.hpp"

LoyaltyProgram::LoyaltyProgram() : _points(0), _visits(0)
{
}

void LoyaltyProgram::RegisterVisit() { _visits++; }
void LoyaltyProgram::AddPoints(double amount_spent) { _points += (int)amount_spent; }

bool LoyaltyProgram::SpendPoints(int points_to_spend)
{
    if (points_to_spend > _points)
        return false;
    _points -= points_to_spend;
    return true;
}

int LoyaltyProgram::GetPoints() const { return _points; }
int LoyaltyProgram::GetVisits() const { return _visits; }

std::string LoyaltyProgram::GetTier() const
{
    if (_points >= 1000)
        return "gold";
    if (_points >= 500)
        return "silver";
    return "bronze";
}