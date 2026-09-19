#include "pizzeria/pizzeria_client.hpp"
#include "pizzeria/pizzeria_department.hpp"
#include "staff/cashier.hpp"
#include "staff/call_center_operator.hpp"
#include "staff/pizzeria_manager.hpp"
#include "staff/accountant.hpp"
#include "staff/hostess.hpp"
#include "staff/courier.hpp"
#include "menu/menu_item.hpp"
#include "menu/ingredient.hpp"
#include "menu/recipe.hpp"
#include "menu/combo_set.hpp"
#include "menu/menu_catalog.hpp"
#include "payment/cash_payment.hpp"
#include "delivery/delivery_address.hpp"
#include "delivery/delivery_zone.hpp"
#include "delivery/delivery_route.hpp"
#include "delivery/delivery_tracker.hpp"
#include "delivery/delivery_fee_calculator.hpp"
#include "kitchen/kitchen.hpp"
#include "kitchen/oven.hpp"
#include "kitchen/refrigerator.hpp"
#include "kitchen/warehouse.hpp"
#include "kitchen/supplier.hpp"
#include "kitchen/quality_controller.hpp"
#include <iostream>
#include <memory>

int main()
{
    auto department = std::make_shared<PizzaDepartment>("Main Street");
    department->Hire<PizzeriaWaiter>(Human(20, "Mike"));
    for (int i = 0; i < 2; i++)
        department->Hire<PizzeriaCooker>(Human(20, "Mike"));
    department->Hire<PizzeriaCookerIntern>(Human(20, "Mike"));

    auto cashier = department->Hire<Cashier>(Human(28, "Nancy"));
    auto manager = department->Hire<PizzeriaManager>(Human(40, "Bob"));
    auto operator_ = department->Hire<CallCenterOperator>(Human(24, "Lily"));
    auto accountant = department->Hire<Accountant>(Human(35, "Grace"));
    department->Hire<Hostess>(Human(22, "Emma"));
    auto courier = department->Hire<Courier>(Human(26, "Jim"));

    manager->OpenStore();
    operator_->AnswerCall();
    operator_->TakeOrder({std::make_pair(Pizza("Margherita"), 2)});
    operator_->TransferToKitchen();

    PizzeriaClient client1(30, "John Doe", department);
    client1.OrderPizza(department->GetMenu());

    PizzeriaClient client2(50, "Walter White", department);
    client2.OrderPizza(department->GetMenu());

    PizzeriaClient client3(21, "Peter Parker", department);
    client3.OrderPizza(department->GetMenu());

    cashier->OpenShift(1000);
    std::shared_ptr<Payment> payment = std::make_shared<CashPayment>(500, 1000);
    cashier->AcceptPayment(payment, 500);
    accountant->RecordRevenue(500);
    accountant->RecordPayroll(accountant->CalculateSalary(200, 8));

    auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
    auto dough = std::make_shared<Ingredient>("Dough", 3, 200, false, 30);
    Recipe recipe("Margherita", 1, 120);
    recipe.AddIngredient(cheese, 200);
    recipe.AddIngredient(dough, 300);

    Refrigerator fridge;
    fridge.Store(cheese, 1000, 30);
    fridge.Retrieve(cheese, 200);

    Warehouse warehouse;
    Supplier supplier("MilkFarm", 2);
    supplier.SetPriceFor(cheese, 4);
    warehouse.Restock(cheese, 100);
    warehouse.Take(cheese, 20);

    auto oven = std::make_shared<Oven>();
    Kitchen kitchen;
    kitchen.OpenKitchen();
    kitchen.AddOven(oven);
    oven->Preheat(250);
    oven->SetLoad(1);

    Order cooked({std::make_pair(Pizza("Margherita"), 1)});
    cooked.SetStatus(IN_PROGRESS);
    QualityController quality;
    if (quality.InspectOrder(cooked))
        quality.ApproveOrder(cooked);

    MenuCatalog catalog("Main Menu");
    catalog.AddItem(std::make_shared<Pizza>("Margherita"));
    ComboSet combo("Lunch combo", 10);
    combo.AddItem(catalog.FindByName("Margherita"), 1);
    combo.AddItem(std::make_shared<MenuItem>("Cola", "drink", 100, 500, 200), 1);
    catalog.AddItem(std::make_shared<Pizza>("Pepperoni"));

    auto address = std::make_shared<DeliveryAddress>("Main Street", "12", "5", 3, "+123");
    auto zone = std::make_shared<DeliveryZone>("Center", 100, 30);
    zone->AddCoveredStreet("Main Street");
    auto route = std::make_shared<DeliveryRoute>();
    route->AddStop(address);
    route->Optimize();

    courier->AcceptDelivery(route);
    DeliveryTracker tracker;
    tracker.SetOnUpdate([](std::string status)
                        { std::cout << "Order status: " << status << std::endl; });
    tracker.StartTracking(1);
    tracker.UpdateStatus("out_for_delivery");
    tracker.UpdateStatus("delivered");
    courier->UpdateLocation("Main Street");
    courier->AddRating(5);
    courier->CompleteDelivery();

    DeliveryFeeCalculator fee_calculator(20, 1.5);
    std::cout << "Delivery fee: " << fee_calculator.CalculateFee(zone, route->GetStopCount(), true) << std::endl;

    department->WaitForAllOrders();
    manager->CloseStore();
    return 0;
}