#pragma once
#include "human.hpp"
#include <memory>

class PizzaDepartment;

class PizzeriaWorker : virtual public Human
{
public:
    PizzeriaWorker(int age, std::string name);
    void SetDepartment(std::shared_ptr<PizzaDepartment> department);
    class DepartmentNotSetException : public std::exception
    {
    };

protected:
    std::shared_ptr<PizzaDepartment> _department;
};