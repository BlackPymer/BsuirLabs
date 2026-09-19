#include "staff/hostess.hpp"

Hostess::Hostess(int age, std::string name, int tables_total)
    : PizzeriaWorker(age, name), Human(age, name), _tables_total(tables_total),
      _tables_occupied(0), _reserved_tables(0)
{
}

bool Hostess::SeatGuest(int party_size)
{
    if (_tables_occupied + party_size > _tables_total)
        return false;
    _tables_occupied += party_size;
    return true;
}

void Hostess::ManageReservation(int party_size)
{
    _reserved_tables += party_size;
    _tables_occupied += party_size;
}

void Hostess::AddToWaitingList(int party_size)
{
    _waiting_list.push_back(party_size);
}

int Hostess::EstimateWaitTime() const
{
    int guests = 0;
    for (const auto &party : _waiting_list)
        guests += party;
    return guests * 2;
}

int Hostess::GetTablesOccupied() const { return _tables_occupied; }
int Hostess::GetWaitingParties() const { return _waiting_list.size(); }