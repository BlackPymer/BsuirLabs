#pragma once
#include "staff/pizzeria_worker.hpp"
#include <map>
#include <string>

class PizzeriaManager : virtual public PizzeriaWorker
{
public:
    PizzeriaManager(int age, std::string name);
    void OpenStore();
    void CloseStore();
    bool IsStoreOpen() const;
    void HandleComplaint(std::string complaint);
    bool ApproveDiscount(std::string code, double percent);
    void SetSchedule(std::string day, int workers);
    int GetScheduledWorkers(std::string day) const;
    int GetComplaintsResolved() const;

private:
    bool _store_open;
    int _complaints_resolved;
    std::map<std::string, double> _active_discounts;
    std::map<std::string, int> _schedule;
};