#include "pizzeria_client.hpp"
#include "pizzeria_department.hpp"
#include <memory>
int main()
{
    auto department = std::make_shared<PizzaDepartment>("Main Street");
    department->Hire<PizzeriaWaiter>(Human(20, "Mike"));
    for (int i = 0; i < 2; i++)
        department->Hire<PizzeriaCooker>(Human(20, "Mike"));
    department->Hire<PizzeriaCookerIntern>(Human(20, "Mike"));

    PizzeriaClient client1(30, "John Doe", department);
    client1.OrderPizza(department->GetMenu());

    PizzeriaClient client2(50, "Walter White", department);
    client2.OrderPizza(department->GetMenu());

    PizzeriaClient client3(21, "Peter Parker", department);
    client3.OrderPizza(department->GetMenu());

    department->WaitForAllOrders();
    return 0;
}