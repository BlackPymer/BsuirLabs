#pragma once
#include <string>

class LoyaltyProgram
{
public:
    LoyaltyProgram();
    void RegisterVisit();
    void AddPoints(double amount_spent);
    bool SpendPoints(int points_to_spend);
    int GetPoints() const;
    int GetVisits() const;
    std::string GetTier() const;

private:
    int _points;
    int _visits;
};