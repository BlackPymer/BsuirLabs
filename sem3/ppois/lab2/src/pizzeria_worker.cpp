#include "pizzeria_worker.hpp"
#include "pizzeria_department.hpp"

PizzeriaWorker::PizzeriaWorker(int age, std::string name) : Human(age, name)
{
}

void PizzeriaWorker::SetDepartment(std::shared_ptr<PizzaDepartment> department)
{
    _department = department;
}