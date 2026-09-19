#pragma once
#include "staff/pizzeria_worker.hpp"
#include <vector>

class Hostess : virtual public PizzeriaWorker
{
public:
    Hostess(int age, std::string name, int tables_total = 10);
    bool SeatGuest(int party_size);
    void ManageReservation(int party_size);
    void AddToWaitingList(int party_size);
    int EstimateWaitTime() const;
    int GetTablesOccupied() const;
    int GetWaitingParties() const;

private:
    int _tables_total;
    int _tables_occupied;
    int _reserved_tables;
    std::vector<int> _waiting_list;
};