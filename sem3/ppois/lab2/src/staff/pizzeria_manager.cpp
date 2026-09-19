#include "staff/pizzeria_manager.hpp"

PizzeriaManager::PizzeriaManager(int age, std::string name)
    : PizzeriaWorker(age, name), Human(age, name), _store_open(false), _complaints_resolved(0)
{
}

void PizzeriaManager::OpenStore() { _store_open = true; }
void PizzeriaManager::CloseStore() { _store_open = false; }
bool PizzeriaManager::IsStoreOpen() const { return _store_open; }

void PizzeriaManager::HandleComplaint(std::string complaint)
{
    if (!complaint.empty())
        _complaints_resolved++;
}

bool PizzeriaManager::ApproveDiscount(std::string code, double percent)
{
    if (percent < 0 || percent > 50)
        return false;
    _active_discounts[code] = percent;
    return true;
}

void PizzeriaManager::SetSchedule(std::string day, int workers)
{
    _schedule[day] = workers;
}

int PizzeriaManager::GetScheduledWorkers(std::string day) const
{
    auto it = _schedule.find(day);
    if (it == _schedule.end())
        return 0;
    return it->second;
}

int PizzeriaManager::GetComplaintsResolved() const { return _complaints_resolved; }