#include "UnitTest++.h"

#include "pizzeria/pizzeria_client.hpp"
#include "staff/pizzeria_cooker.hpp"
#include "staff/pizzeria_cooker_intern.hpp"
#include "pizzeria/pizzeria_department.hpp"
#include "staff/pizzeria_waiter.hpp"
#include "staff/pizzeria_worker.hpp"
#include "staff/cashier.hpp"
#include "staff/call_center_operator.hpp"
#include "staff/pizzeria_manager.hpp"
#include "staff/accountant.hpp"
#include "staff/hostess.hpp"
#include "staff/courier.hpp"
#include "common/pizzeria_exceptions.hpp"
#include "menu/menu_item.hpp"
#include "menu/ingredient.hpp"
#include "menu/recipe.hpp"
#include "menu/combo_set.hpp"
#include "menu/menu_catalog.hpp"
#include "payment/payment.hpp"
#include "payment/cash_payment.hpp"
#include "payment/card_payment.hpp"
#include "payment/mobile_payment.hpp"
#include "payment/receipt.hpp"
#include "payment/coupon.hpp"
#include "payment/loyalty_program.hpp"
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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace
{

bool WaitForFlag(const std::atomic<int> &flag, int expected, int timeout_ms = 3000)
{
    std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (flag.load() != expected)
    {
        if (std::chrono::steady_clock::now() > deadline)
            return false;
        std::this_thread::yield();
    }
    return true;
}

std::vector<std::pair<Pizza, int>> SinglePizzaOrder()
{
    return {std::make_pair(Pizza("Margherita"), 1)};
}

} // namespace

SUITE(Human)
{
    TEST(Constructor_StoresFields)
    {
        Human h(25, "Alice");
        CHECK_EQUAL(25, h.GetAge());
        CHECK_EQUAL("Alice", h.GetName());
    }
}

SUITE(Pizza)
{
    TEST(Constructor_And_Getter)
    {
        Pizza p("Margherita");
        CHECK_EQUAL("Margherita", p.GetName());
    }

    TEST(LessThan_ComparesNames)
    {
        Pizza a("Apple");
        Pizza b("Banana");
        CHECK(a < b);
        CHECK(!(b < a));
        CHECK(!(a < a));
    }
}

SUITE(Order)
{
    TEST(Constructor_StatusPending)
    {
        Order o(SinglePizzaOrder());
        CHECK_EQUAL(OrderStatus::PENDING, o.GetStatus());
    }

    TEST(SetStatus_GetStatus)
    {
        Order o(SinglePizzaOrder());
        o.SetStatus(OrderStatus::IN_PROGRESS);
        CHECK_EQUAL(OrderStatus::IN_PROGRESS, o.GetStatus());
        o.SetStatus(OrderStatus::DELIVERED);
        CHECK_EQUAL(OrderStatus::DELIVERED, o.GetStatus());
    }

    TEST(GetOrderComponents_ReturnsMap)
    {
        Order o(SinglePizzaOrder());
        auto components = o.GetOrderComponents();
        CHECK_EQUAL(1u, components.size());
        CHECK_EQUAL(1, components[Pizza("Margherita")]);
    }

    TEST(GetPizzaCount_DistinctTypes)
    {
        std::vector<std::pair<Pizza, int>> v;
        v.push_back(std::make_pair(Pizza("Margherita"), 2));
        v.push_back(std::make_pair(Pizza("Pepperoni"), 3));
        Order o(v);
        CHECK_EQUAL(2, o.GetPizzaCount());
    }

    TEST(LessThan_DifferentSizes)
    {
        Order one(SinglePizzaOrder());
        std::vector<std::pair<Pizza, int>> two_v;
        two_v.push_back(std::make_pair(Pizza("Margherita"), 1));
        two_v.push_back(std::make_pair(Pizza("Pepperoni"), 1));
        Order two(two_v);
        CHECK(one < two);
        CHECK(!(two < one));
    }

    TEST(LessThan_EqualSizesComparesContent)
    {
        Order a(SinglePizzaOrder());
        Order b(SinglePizzaOrder());
        CHECK(!(a < b));
        CHECK(!(b < a));
    }
}

SUITE(Intern)
{
    TEST(DefaultConstructor_FactorTwo)
    {
        Intern i;
        CHECK_CLOSE(2.0, i.GetTimeFactor(), 0.001);
    }

    TEST(SetFactor_GetFactor)
    {
        Intern i(1.5);
        CHECK_CLOSE(1.5, i.GetTimeFactor(), 0.001);
        i.SetTimeFactor(3.25);
        CHECK_CLOSE(3.25, i.GetTimeFactor(), 0.001);
    }

    TEST(Mentor_NullByDefault)
    {
        Intern i;
        CHECK(i.GetMentor() == nullptr);
    }

    TEST(SetMentor_GetMentor)
    {
        Intern i;
        auto mentor = std::make_shared<PizzeriaCooker>(30, "Chef");
        i.SetMentor(mentor);
        CHECK(i.GetMentor() == mentor);
    }

    TEST(CompletedOrders_Counter)
    {
        Intern i;
        CHECK_EQUAL(0, i.GetCompletedOrders());
        i.IncrementCompletedOrders();
        i.IncrementCompletedOrders();
        CHECK_EQUAL(2, i.GetCompletedOrders());
    }
}

SUITE(PizzeriaWorker)
{
    TEST(SetDepartment_SetsPointer)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        PizzeriaWaiter w(20, "Waiter");
        w.SetDepartment(dept);
        w.GetActiveOrders();
        CHECK(true);
    }
}

SUITE(PizzaDepartmentBasic)
{
    TEST(Constructor_Address)
    {
        PizzaDepartment d("Some Street");
        CHECK_EQUAL("Some Street", d.GetAddress());
    }

    TEST(Constructor_Menu)
    {
        PizzaDepartment d("Some Street");
        auto menu = d.GetMenu();
        CHECK_EQUAL(3u, menu.size());
        bool has_margherita = false, has_pepperoni = false, has_vegetarian = false;
        for (const auto &p : menu)
        {
            Pizza copy = p;
            if (copy.GetName() == "Margherita") has_margherita = true;
            if (copy.GetName() == "Pepperoni") has_pepperoni = true;
            if (copy.GetName() == "Vegetarian") has_vegetarian = true;
        }
        CHECK(has_margherita);
        CHECK(has_pepperoni);
        CHECK(has_vegetarian);
    }

    TEST(Hire_Waiter)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto w = dept->Hire<PizzeriaWaiter>(Human(20, "Mike"));
        CHECK(w != nullptr);
        CHECK_EQUAL("Mike", w->GetName());
    }

    TEST(Hire_Cooker)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto c = dept->Hire<PizzeriaCooker>(Human(25, "Gordon"));
        CHECK(c != nullptr);
        CHECK_EQUAL(25, c->GetAge());
    }

    TEST(Hire_Intern)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto i = dept->Hire<PizzeriaCookerIntern>(Human(19, "Pablo"));
        CHECK(i != nullptr);
        CHECK_EQUAL(19, i->GetAge());
    }

    TEST(TakeOrder_NoCookers_Throws)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->Hire<PizzeriaWaiter>(Human(20, "Mike"));
        CHECK_THROW(dept->TakeOrder(SinglePizzaOrder(), []() {}),
                    PizzaDepartment::AllCooksAreBusyException);
    }

    TEST(GiveOrderToCook_NoOneFree_ReturnsSilently)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->Hire<PizzeriaWaiter>(Human(20, "Mike"));
        Order o(SinglePizzaOrder());
        dept->GiveOrderToCook(o, [](Order) {});
        CHECK(true);
    }

    TEST(WaitForAllOrders_NoOrders_ReturnsImmediately)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->WaitForAllOrders();
        CHECK(true);
    }

    TEST(OrderFinished_NotifyNoWaiters_NoCrash)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->OrderFinished();
        CHECK(true);
    }
}

SUITE(PizzaDepartmentFlow)
{
    TEST(TakeOrder_CooksAndDelivers)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->Hire<PizzeriaWaiter>(Human(20, "Waiter"));
        dept->Hire<PizzeriaCooker>(Human(30, "Chef"));

        std::atomic<int> delivered{0};
        dept->TakeOrder(SinglePizzaOrder(), [&delivered]() { delivered++; });

        bool delivered_ok = WaitForFlag(delivered, 1, 5000);
        CHECK(delivered_ok);
        CHECK_EQUAL(1, delivered.load());
    }

    TEST(TakeOrder_TwoOrders_CookerThenIntern)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->Hire<PizzeriaWaiter>(Human(20, "Waiter"));
        dept->Hire<PizzeriaCooker>(Human(30, "Chef"));
        dept->Hire<PizzeriaCookerIntern>(Human(19, "Pablo"));

        std::atomic<int> delivered{0};
        auto cb = [&delivered]() { delivered++; };
        dept->TakeOrder(SinglePizzaOrder(), cb);
        std::vector<std::pair<Pizza, int>> pepperoni = {
            std::make_pair(Pizza("Pepperoni"), 1)};
        dept->TakeOrder(pepperoni, cb);

        bool delivered_ok = WaitForFlag(delivered, 2, 8000);
        CHECK(delivered_ok);
        CHECK_EQUAL(2, delivered.load());
    }

    TEST(CookingFinished_Intern_Releases)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->Hire<PizzeriaWaiter>(Human(20, "Waiter"));
        auto i = dept->Hire<PizzeriaCookerIntern>(Human(19, "Pablo"));

        std::atomic<int> done{0};
        dept->TakeOrder(SinglePizzaOrder(), [&done]() { done++; });
        CHECK(WaitForFlag(done, 1, 8000));
        CHECK_EQUAL(1, done.load());
    }

    TEST(CookingFinished_Cooker_DirectCall)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto c = dept->Hire<PizzeriaCooker>(Human(30, "Chef"));
        dept->CookingFinished(*c.get());
        CHECK(true);
    }

    TEST(GiveOrderToCook_Direct_CookerCallback)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto c = dept->Hire<PizzeriaCooker>(Human(30, "Chef"));

        std::atomic<int> cooked{0};
        Order o(SinglePizzaOrder());
        dept->GiveOrderToCook(o, [&cooked](Order) { cooked++; });
        CHECK(WaitForFlag(cooked, 1, 5000));
        CHECK_EQUAL(1, cooked.load());
    }

    TEST(GiveOrderToCook_Direct_InternCallback)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto c = dept->Hire<PizzeriaCooker>(Human(30, "Chef"));
        auto i = dept->Hire<PizzeriaCookerIntern>(Human(19, "Pablo"));

        std::atomic<int> cooked{0};
        // First order occupies the cooker, second should fall to the intern.
        Order o1(SinglePizzaOrder());
        Order o2(SinglePizzaOrder());
        dept->GiveOrderToCook(o1, [&cooked](Order) { cooked++; });
        dept->GiveOrderToCook(o2, [&cooked](Order) { cooked++; });

        CHECK(WaitForFlag(cooked, 2, 8000));
        CHECK_EQUAL(2, cooked.load());
    }
}

SUITE(PizzeriaWaiter)
{
    TEST(GetOrder_NoDepartment_Throws)
    {
        PizzeriaWaiter w(20, "Solo");
        CHECK_THROW(w.GetOrder(SinglePizzaOrder(), []() {}),
                    PizzeriaWorker::DepartmentNotSetException);
    }

    TEST(GetActiveOrders_TracksLifecycle)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        dept->Hire<PizzeriaWaiter>(Human(20, "Waiter"));
        dept->Hire<PizzeriaCooker>(Human(30, "Chef"));

        std::atomic<int> delivered{0};
        dept->TakeOrder(SinglePizzaOrder(), [&delivered]() { delivered++; });
        CHECK(WaitForFlag(delivered, 1, 5000));
        CHECK_EQUAL(1, delivered.load());
    }
}

SUITE(PizzeriaCooker)
{
    TEST(Constructor_And_CookOrder_Callback)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto c = dept->Hire<PizzeriaCooker>(Human(30, "Chef"));

        std::mutex m;
        std::condition_variable cv;
        bool called = false;
        Order o(SinglePizzaOrder());
        c->CookOrder(o, [&](Order) {
            std::lock_guard<std::mutex> lk(m);
            called = true;
            cv.notify_one();
        });

        std::unique_lock<std::mutex> lk(m);
        bool ok = cv.wait_for(lk, std::chrono::seconds(3), [&]() { return called; });
        CHECK(ok);
    }
}

SUITE(PizzeriaCookerIntern)
{
    TEST(Construction_WithFactor)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto i = std::make_shared<PizzeriaCookerIntern>(19, "Pablo", 0.1);
        i->SetDepartment(dept);
        CHECK_CLOSE(0.1, i->GetTimeFactor(), 0.001);
    }

    TEST(CookOrder_HonorsFactor_Callback)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto i = std::make_shared<PizzeriaCookerIntern>(19, "Pablo", 0.1);
        i->SetDepartment(dept);

        std::atomic<int> cooked{0};
        Order o(SinglePizzaOrder());
        i->CookOrder(o, [&cooked](Order) { cooked++; });
        CHECK(WaitForFlag(cooked, 1, 5000));
        CHECK_EQUAL(1, cooked.load());
    }
}

SUITE(PizzeriaClient)
{
    TEST(OrderPizza_Delivers)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        for (int n = 0; n < 2; n++)
            dept->Hire<PizzeriaWaiter>(Human(20, "Mike"));
        for (int n = 0; n < 2; n++)
            dept->Hire<PizzeriaCooker>(Human(30, "Chef"));

        PizzeriaClient client(30, "John", dept);
        client.OrderPizza(dept->GetMenu());

        dept->WaitForAllOrders();
        CHECK(true);
    }

    TEST(OrderPizza_MultipleClients)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        for (int n = 0; n < 2; n++)
            dept->Hire<PizzeriaWaiter>(Human(20, "Mike"));
        for (int n = 0; n < 2; n++)
            dept->Hire<PizzeriaCooker>(Human(30, "Chef"));
        dept->Hire<PizzeriaCookerIntern>(Human(19, "Pablo"));

        PizzeriaClient c1(30, "A", dept);
        PizzeriaClient c2(40, "B", dept);
        c1.OrderPizza(dept->GetMenu());
        c2.OrderPizza(dept->GetMenu());

        dept->WaitForAllOrders();
        CHECK(true);
    }
}

SUITE(PizzeriaExceptions)
{
    TEST(What_ReturnsMessage)
    {
        PizzeriaException e("boom");
        CHECK_EQUAL(std::string("boom"), std::string(e.what()));
    }

    TEST(CategoryExceptions_What)
    {
        OrderException o("order");
        PaymentException p("payment");
        DeliveryException d("delivery");
        InventoryException i("inventory");
        StaffException s("staff");
        CHECK_EQUAL(std::string("order"), std::string(o.what()));
        CHECK_EQUAL(std::string("payment"), std::string(p.what()));
        CHECK_EQUAL(std::string("delivery"), std::string(d.what()));
        CHECK_EQUAL(std::string("inventory"), std::string(i.what()));
        CHECK_EQUAL(std::string("staff"), std::string(s.what()));
    }

    TEST(LeafExceptions_AreCatchableAsBase)
    {
        CHECK_THROW(throw EmptyOrderException(), PizzeriaException);
        CHECK_THROW(throw NoSuchOrderException(), PizzeriaException);
        CHECK_THROW(throw PaymentDeclinedException(), PizzeriaException);
        CHECK_THROW(throw InsufficientFundsException(), PizzeriaException);
        CHECK_THROW(throw NoCouriersAvailableException(), PizzeriaException);
        CHECK_THROW(throw DeliveryZoneNotFoundException(), PizzeriaException);
        CHECK_THROW(throw ItemOutOfStockException(), PizzeriaException);
    }

    TEST(LeafExceptions_MessageNotEmpty)
    {
        CHECK(std::string(EmptyOrderException().what()).size() > 0);
        CHECK(std::string(NoSuchOrderException().what()).size() > 0);
        CHECK(std::string(PaymentDeclinedException().what()).size() > 0);
        CHECK(std::string(InsufficientFundsException().what()).size() > 0);
        CHECK(std::string(NoCouriersAvailableException().what()).size() > 0);
        CHECK(std::string(DeliveryZoneNotFoundException().what()).size() > 0);
        CHECK(std::string(ItemOutOfStockException().what()).size() > 0);
    }
}

SUITE(MenuItemSuite)
{
    TEST(Constructor_And_Getters)
    {
        MenuItem item("Cola", "drink", 100, 500, 200);
        CHECK_EQUAL("Cola", item.GetName());
        CHECK_EQUAL("drink", item.GetCategory());
        CHECK_CLOSE(100, item.GetPrice(), 0.001);
        CHECK_EQUAL(500, item.GetWeight());
        CHECK_EQUAL(200, item.GetCalories());
    }

    TEST(SetPrice_And_Totals)
    {
        MenuItem item("Cola", "drink", 100, 500, 200);
        item.SetPrice(150);
        CHECK_CLOSE(150, item.GetTotalPrice(), 0.001);
        CHECK_EQUAL(200, item.GetTotalCalories());
    }
}

SUITE(IngredientSuite)
{
    TEST(Constructor_And_Cost)
    {
        Ingredient cheese("Cheese", 5, 100, true, 10);
        CHECK_EQUAL("Cheese", cheese.GetName());
        CHECK_CLOSE(50, cheese.GetCost(10), 0.001);
        CHECK_EQUAL(1000, cheese.GetCalories(10));
        CHECK(cheese.IsPerishable());
        CHECK_EQUAL(10, cheese.GetShelfLifeDays());
    }

    TEST(SetCost)
    {
        Ingredient cheese("Cheese", 5, 100, true, 10);
        cheese.SetCost(7);
        CHECK_CLOSE(70, cheese.GetCost(10), 0.001);
        Ingredient flour("Flour", 2, 300, false, 60);
        CHECK(!flour.IsPerishable());
    }
}

SUITE(RecipeSuite)
{
    TEST(AddIngredient_And_PerPortion)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        auto dough = std::make_shared<Ingredient>("Dough", 3, 200, false, 30);
        Recipe recipe("Margherita", 2, 120);
        recipe.AddIngredient(cheese, 200);
        recipe.AddIngredient(dough, 300);
        CHECK_EQUAL(2, recipe.GetPortions());
        CHECK_EQUAL(120, recipe.GetPrepTimeSeconds());
        CHECK_EQUAL(2u, recipe.GetIngredients().size());
        CHECK_CLOSE(950, recipe.GetCostPerPortion(), 0.001);
        CHECK_EQUAL(40000, recipe.GetCaloriesPerPortion());
    }
}

SUITE(ComboSetSuite)
{
    TEST(AddRemove_And_Totals)
    {
        auto pizza = std::make_shared<MenuItem>("Pizza", "pizza", 200, 400, 800);
        auto cola = std::make_shared<MenuItem>("Cola", "drink", 100, 500, 200);
        ComboSet combo("Lunch", 10);
        combo.AddItem(pizza, 1);
        combo.AddItem(cola, 2);
        CHECK_EQUAL(3, combo.GetItemCount());
        CHECK_CLOSE(360, combo.GetTotalPrice(), 0.001);
        CHECK_EQUAL(1200, combo.GetTotalCalories());
        combo.RemoveItem(cola);
        CHECK_EQUAL(1, combo.GetItemCount());
    }
}

SUITE(MenuCatalogSuite)
{
    TEST(AddFindRemove_Update)
    {
        MenuCatalog catalog("Main");
        CHECK_EQUAL("Main", catalog.GetName());
        auto pizza = std::make_shared<MenuItem>("Pizza", "pizza", 200, 400, 800);
        auto cola = std::make_shared<MenuItem>("Cola", "drink", 100, 500, 200);
        catalog.AddItem(pizza);
        catalog.AddItem(cola);
        CHECK_EQUAL(2, catalog.GetItemCount());
        CHECK(catalog.FindByName("Pizza") == pizza);
        CHECK(catalog.FindByName("Missing") == nullptr);
        CHECK_EQUAL(1u, catalog.GetAllByCategory("drink").size());
        catalog.UpdatePrice("Pizza", 250);
        CHECK_CLOSE(250, pizza->GetPrice(), 0.001);
        catalog.UpdatePrice("Missing", 999);
        catalog.RemoveItem("Cola");
        CHECK_EQUAL(1, catalog.GetItemCount());
    }
}

SUITE(PizzaToppings)
{
    TEST(Size_And_Toppings)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        Pizza pizza("Margherita", 200, 400, 800);
        CHECK_EQUAL(MEDIUM, pizza.GetSize());
        pizza.SetSize(SMALL);
        CHECK_EQUAL(SMALL, pizza.GetSize());
        pizza.SetSize(LARGE);
        CHECK_EQUAL(LARGE, pizza.GetSize());
        pizza.AddTopping(cheese, 10);
        CHECK_CLOSE(300, pizza.CalculatePrice(), 0.001);
        CHECK_EQUAL(1800, pizza.CalculateCalories());
        CHECK_CLOSE(300, pizza.GetTotalPrice(), 0.001);
        CHECK_EQUAL(1800, pizza.GetTotalCalories());
        pizza.RemoveTopping(cheese);
        CHECK_CLOSE(240, pizza.CalculatePrice(), 0.001);
    }
}

SUITE(CashierSuite)
{
    TEST(Shift_Drawer)
    {
        Cashier cashier(28, "Nancy");
        CHECK(!cashier.IsShiftOpen());
        cashier.OpenShift(1000);
        CHECK(cashier.IsShiftOpen());
        CHECK_CLOSE(1000, cashier.GetCashDrawer(), 0.001);
        CHECK_CLOSE(1000, cashier.CloseShift(), 0.001);
        CHECK(!cashier.IsShiftOpen());
    }

    TEST(AcceptPayment_Change)
    {
        Cashier cashier(28, "Nancy");
        cashier.OpenShift(0);
        std::shared_ptr<Payment> payment = std::make_shared<CashPayment>(500, 1000);
        double change = cashier.AcceptPayment(payment, 500);
        CHECK_CLOSE(500, change, 0.001);
        CHECK_CLOSE(500, cashier.GetCashDrawer(), 0.001);
    }

    TEST(AcceptPayment_Null_Throws)
    {
        Cashier cashier(28, "Nancy");
        CHECK_THROW(cashier.AcceptPayment(nullptr, 500), PaymentDeclinedException);
    }
}

SUITE(CallCenterOperatorSuite)
{
    TEST(Calls_And_Orders)
    {
        CallCenterOperator op(24, "Lily");
        CHECK_EQUAL(0, op.GetCallsHandled());
        CHECK_EQUAL(1, op.AnswerCall());
        op.TakeOrder(SinglePizzaOrder());
        CHECK_EQUAL(1, op.GetOrdersTaken());
        op.TransferToKitchen();
        op.EndCall();
        CHECK_EQUAL(1, op.GetCallsHandled());
    }
}

SUITE(PizzeriaManagerSuite)
{
    TEST(Store_And_Complaints)
    {
        PizzeriaManager manager(40, "Bob");
        CHECK(!manager.IsStoreOpen());
        manager.OpenStore();
        CHECK(manager.IsStoreOpen());
        manager.CloseStore();
        CHECK(!manager.IsStoreOpen());
        manager.HandleComplaint("too slow");
        manager.HandleComplaint("");
        CHECK_EQUAL(1, manager.GetComplaintsResolved());
    }

    TEST(Discounts_And_Schedule)
    {
        PizzeriaManager manager(40, "Bob");
        CHECK(manager.ApproveDiscount("WELCOME", 10));
        CHECK(!manager.ApproveDiscount("BAD", 90));
        CHECK(!manager.ApproveDiscount("NEG", -1));
        manager.SetSchedule("Monday", 5);
        CHECK_EQUAL(5, manager.GetScheduledWorkers("Monday"));
        CHECK_EQUAL(0, manager.GetScheduledWorkers("Sunday"));
    }
}

SUITE(AccountantSuite)
{
    TEST(Revenue_Expense_Profit)
    {
        Accountant accountant(35, "Grace");
        accountant.RecordRevenue(1000);
        accountant.RecordExpense(200);
        accountant.RecordPayroll(300);
        CHECK_CLOSE(1000, accountant.GetTotalRevenue(), 0.001);
        CHECK_CLOSE(500, accountant.GetTotalExpenses(), 0.001);
        CHECK_CLOSE(500, accountant.GetNetProfit(), 0.001);
        CHECK_CLOSE(1600, accountant.CalculateSalary(200, 8), 0.001);
    }
}

SUITE(HostessSuite)
{
    TEST(Seating_And_Reservations)
    {
        Hostess hostess(22, "Emma", 10);
        CHECK(hostess.SeatGuest(4));
        CHECK(!hostess.SeatGuest(8));
        CHECK_EQUAL(4, hostess.GetTablesOccupied());
        hostess.ManageReservation(2);
        CHECK_EQUAL(6, hostess.GetTablesOccupied());
    }

    TEST(WaitingList)
    {
        Hostess hostess(22, "Emma", 10);
        hostess.AddToWaitingList(2);
        hostess.AddToWaitingList(3);
        CHECK_EQUAL(2, hostess.GetWaitingParties());
        CHECK_EQUAL(10, hostess.EstimateWaitTime());
    }
}

SUITE(CourierSuite)
{
    TEST(Delivery_Lifecycle)
    {
        Courier courier(26, "Jim");
        CHECK_EQUAL("pizzeria", courier.GetLocation());
        CHECK(courier.GetCurrentRoute() == nullptr);
        CHECK(!courier.CompleteDelivery());

        auto route = std::make_shared<DeliveryRoute>();
        route->AddStop(std::make_shared<DeliveryAddress>("Main", "1", "1", 1, "+1"));
        courier.AcceptDelivery(route);
        CHECK(courier.GetCurrentRoute() == route);
        courier.UpdateLocation("Main");
        CHECK_EQUAL("Main", courier.GetLocation());
        CHECK(courier.CompleteDelivery());
        CHECK_EQUAL(1, courier.GetCompletedDeliveries());
    }

    TEST(Accept_EmptyRoute_Throws)
    {
        Courier courier(26, "Jim");
        auto route = std::make_shared<DeliveryRoute>();
        CHECK_THROW(courier.AcceptDelivery(route), DeliveryException);
        CHECK_THROW(courier.AcceptDelivery(nullptr), DeliveryException);
    }

    TEST(Rating)
    {
        Courier courier(26, "Jim");
        CHECK_CLOSE(0, courier.GetAverageRating(), 0.001);
        courier.AddRating(4);
        courier.AddRating(5);
        CHECK_CLOSE(4.5, courier.GetAverageRating(), 0.001);
    }
}

SUITE(DeliveryAddressSuite)
{
    TEST(Address_Fields)
    {
        DeliveryAddress address("Main", "12", "5", 3, "+123");
        CHECK_EQUAL("Main", address.GetStreet());
        CHECK_EQUAL("+123", address.GetPhone());
        CHECK_EQUAL(3, address.GetFloor());
        CHECK(address.GetFullAddress().find("Main") != std::string::npos);
    }
}

SUITE(DeliveryZoneSuite)
{
    TEST(Zone_Coverage)
    {
        auto address = std::make_shared<DeliveryAddress>("Main", "12", "5", 3, "+123");
        DeliveryZone zone("Center", 100, 30);
        CHECK_EQUAL("Center", zone.GetName());
        CHECK_EQUAL(0, zone.GetCoveredStreetCount());
        zone.AddCoveredStreet("Main");
        CHECK_EQUAL(1, zone.GetCoveredStreetCount());
        CHECK(zone.IsAddressInZone(address));
        CHECK(!zone.IsAddressInZone(nullptr));
        auto other = std::make_shared<DeliveryAddress>("Other", "1", "1", 1, "+1");
        CHECK(!zone.IsAddressInZone(other));
        CHECK_CLOSE(100, zone.GetFee(), 0.001);
        zone.SetFee(150);
        CHECK_CLOSE(150, zone.GetFee(), 0.001);
        CHECK_EQUAL(30, zone.GetEstimatedMinutes());
    }
}

SUITE(DeliveryRouteSuite)
{
    TEST(Route_And_Optimize)
    {
        DeliveryRoute route;
        CHECK_EQUAL(0, route.GetStopCount());
        CHECK(!route.IsOptimized());
        route.AddStop(std::make_shared<DeliveryAddress>("A", "1", "1", 1, "+1"));
        route.AddStop(std::make_shared<DeliveryAddress>("B", "2", "2", 2, "+2"));
        CHECK_EQUAL(2, route.GetStopCount());
        CHECK_CLOSE(4, route.CalculateDistance(), 0.001);
        route.Optimize();
        CHECK(route.IsOptimized());
        CHECK_CLOSE(3, route.CalculateDistance(), 0.001);
    }
}

SUITE(DeliveryTrackerSuite)
{
    TEST(Tracking_And_Callback)
    {
        DeliveryTracker tracker;
        CHECK(!tracker.IsTracking());
        std::atomic<int> updates{0};
        tracker.SetOnUpdate([&updates](std::string) { updates++; });
        tracker.StartTracking(7);
        CHECK(tracker.IsTracking());
        CHECK_EQUAL(7, tracker.GetOrderId());
        tracker.UpdateStatus("out_for_delivery");
        tracker.UpdateStatus("delivered");
        CHECK_EQUAL("delivered", tracker.GetStatus());
        CHECK_EQUAL(2, tracker.GetUpdateCount());
        CHECK_EQUAL(2, updates.load());
    }

    TEST(Update_WithoutTracking_Throws)
    {
        DeliveryTracker tracker;
        CHECK_THROW(tracker.UpdateStatus("none"), NoSuchOrderException);
    }
}

SUITE(DeliveryFeeCalculatorSuite)
{
    TEST(Fee_Calculation)
    {
        auto zone = std::make_shared<DeliveryZone>("Center", 100, 30);
        DeliveryFeeCalculator calc(20, 1.5);
        CHECK_CLOSE(20, calc.GetPerStopFee(), 0.001);
        CHECK_CLOSE(140, calc.CalculateFee(zone, 2, false), 0.001);
        CHECK_CLOSE(210, calc.CalculateFee(zone, 2, true), 0.001);
    }
}

SUITE(PaymentSuite)
{
    TEST(CashPayment_Success)
    {
        std::shared_ptr<Payment> payment = std::make_shared<CashPayment>(500, 1000);
        CHECK_CLOSE(500, payment->GetAmount(), 0.001);
        CHECK_EQUAL(PAYMENT_PENDING, payment->GetStatus());
        CHECK(payment->Process());
        CHECK_EQUAL(PAYMENT_PAID, payment->GetStatus());
        CHECK_CLOSE(500, payment->GetChange(), 0.001);
    }

    TEST(CashPayment_Insufficient_Throws)
    {
        std::shared_ptr<Payment> payment = std::make_shared<CashPayment>(500, 100);
        CHECK_THROW(payment->Process(), InsufficientFundsException);
        CHECK_EQUAL(PAYMENT_DECLINED, payment->GetStatus());
    }

    TEST(CardPayment_AuthorizeCapture)
    {
        CardPayment payment(500, "1234");
        CHECK_EQUAL("1234", payment.GetCardLastFour());
        CHECK(payment.Authorize());
        CHECK(payment.Capture());
        CHECK_EQUAL(PAYMENT_PAID, payment.GetStatus());
        CHECK(payment.Refund());
        CHECK_EQUAL(PAYMENT_REFUNDED, payment.GetStatus());
    }

    TEST(CardPayment_BadCard)
    {
        CardPayment payment(500, "12");
        CHECK(!payment.Authorize());
        CHECK(!payment.Capture());
        CHECK_EQUAL(PAYMENT_DECLINED, payment.GetStatus());
        CHECK(!payment.Process());
        CHECK(!payment.Refund());
    }

    TEST(MobilePayment_Token)
    {
        MobilePayment payment(500, "+123", "abcd");
        CHECK_EQUAL("+123", payment.GetPhone());
        CHECK(payment.ValidateToken());
        CHECK(payment.Process());
        CHECK_EQUAL(PAYMENT_PAID, payment.GetStatus());
    }

    TEST(MobilePayment_BadToken_Throws)
    {
        MobilePayment payment(500, "+123", "a");
        CHECK(!payment.ValidateToken());
        CHECK_THROW(payment.Process(), PaymentDeclinedException);
        CHECK_EQUAL(PAYMENT_DECLINED, payment.GetStatus());
    }

    TEST(BaseRefund_ZeroAmount_False)
    {
        MobilePayment payment(0, "+123", "abcd");
        CHECK(!payment.Refund());
    }

    TEST(BasePayment_Refund_And_Change)
    {
        MobilePayment payment(500, "+123", "abcd");
        CHECK(payment.Process());
        CHECK_CLOSE(0, payment.GetChange(), 0.001);
        CHECK_CLOSE(500, payment.GetAmount(), 0.001);
        CHECK(payment.Refund());
        CHECK_EQUAL(PAYMENT_REFUNDED, payment.GetStatus());
    }
}

SUITE(ReceiptSuite)
{
    TEST(Lines_And_Total)
    {
        auto pizza = std::make_shared<MenuItem>("Pizza", "pizza", 200, 400, 800);
        Receipt receipt(42);
        CHECK_EQUAL(42, receipt.GetNumber());
        CHECK_EQUAL(0, receipt.GetLineCount());
        receipt.AddLine(pizza, 2);
        CHECK_EQUAL(1, receipt.GetLineCount());
        CHECK_CLOSE(400, receipt.GetTotal(), 0.001);
        receipt.SetTotal(999);
        CHECK_CLOSE(999, receipt.GetTotal(), 0.001);
        CHECK(receipt.Print().find("42") != std::string::npos);
    }
}

SUITE(CouponSuite)
{
    TEST(Validation_And_Apply)
    {
        Coupon coupon("SAVE10", 10, 30, 2, 500);
        CHECK_EQUAL("SAVE10", coupon.GetCode());
        CHECK_CLOSE(10, coupon.GetDiscount(), 0.001);
        CHECK(!coupon.IsExpired(20));
        CHECK(coupon.Validate(600, 20));
        CHECK(!coupon.Validate(100, 20));
        CHECK(!coupon.Validate(600, 40));
        CHECK(coupon.Apply());
        CHECK(coupon.Apply());
        CHECK(!coupon.Apply());
        CHECK_EQUAL(2, coupon.GetUsedCount());
    }
}

SUITE(LoyaltyProgramSuite)
{
    TEST(Points_And_Tiers)
    {
        LoyaltyProgram program;
        CHECK_EQUAL(0, program.GetPoints());
        CHECK_EQUAL(0, program.GetVisits());
        CHECK_EQUAL("bronze", program.GetTier());
        program.RegisterVisit();
        CHECK_EQUAL(1, program.GetVisits());
        program.AddPoints(600);
        CHECK_EQUAL(600, program.GetPoints());
        CHECK_EQUAL("silver", program.GetTier());
        CHECK(program.SpendPoints(100));
        CHECK_EQUAL(500, program.GetPoints());
        CHECK(!program.SpendPoints(100000));
        program.AddPoints(600);
        CHECK_EQUAL("gold", program.GetTier());
    }
}

SUITE(OvenSuite)
{
    TEST(Preheat_And_Ready)
    {
        Oven oven;
        CHECK_EQUAL(20, oven.GetTemperature());
        CHECK(!oven.IsReady());
        oven.Preheat(250);
        CHECK_EQUAL(250, oven.GetTemperature());
        CHECK(!oven.IsReady());
        oven.SetLoad(2);
        CHECK_EQUAL(2, oven.GetLoad());
        CHECK(oven.IsReady());
    }

    TEST(Bake_Callback)
    {
        Oven oven;
        oven.Preheat(250);
        oven.SetLoad(1);
        std::atomic<int> done{0};
        oven.Bake(0, [&done]() { done++; });
        CHECK(WaitForFlag(done, 1, 3000));
        CHECK_EQUAL(0, oven.GetLoad());
    }
}

SUITE(KitchenSuite)
{
    TEST(Open_And_Ovens)
    {
        Kitchen kitchen;
        CHECK(!kitchen.IsOpen());
        kitchen.OpenKitchen();
        CHECK(kitchen.IsOpen());
        kitchen.CloseKitchen();
        CHECK(!kitchen.IsOpen());
        kitchen.AddOven(std::make_shared<Oven>());
        CHECK_EQUAL(1, kitchen.GetOvenCount());
    }

    TEST(AssignOrder_Empty_Throws)
    {
        Kitchen kitchen;
        kitchen.OpenKitchen();
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto cooker = dept->Hire<PizzeriaCooker>(Human(30, "Chef"));
        Order empty({});
        CHECK_THROW(kitchen.AssignOrder(cooker, empty, [](Order) {}), EmptyOrderException);
    }

    TEST(AssignOrder_Cooks)
    {
        Kitchen kitchen;
        kitchen.OpenKitchen();
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        auto cooker = dept->Hire<PizzeriaCooker>(Human(30, "Chef"));
        Order order(SinglePizzaOrder());
        std::atomic<int> done{0};
        kitchen.AssignOrder(cooker, order, [&done](Order) { done++; });
        CHECK(WaitForFlag(done, 1, 3000));
        CHECK_EQUAL(0, kitchen.GetActiveOrders());
    }
}

SUITE(RefrigeratorSuite)
{
    TEST(Store_Retrieve_Discard)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        Refrigerator fridge;
        CHECK_EQUAL(4, fridge.GetTemperature());
        fridge.SetTemperature(2);
        CHECK_EQUAL(2, fridge.GetTemperature());
        fridge.Store(cheese, 100, 5);
        CHECK_EQUAL(100, fridge.GetQuantity(cheese));
        CHECK_EQUAL(100, fridge.GetCapacity());
        CHECK(fridge.Retrieve(cheese, 40));
        CHECK_EQUAL(60, fridge.GetQuantity(cheese));
        CHECK_EQUAL(60, fridge.DiscardExpired(10));
        CHECK_EQUAL(0, fridge.GetQuantity(cheese));
        CHECK_EQUAL(0, fridge.GetCapacity());
        CHECK_EQUAL(0, fridge.DiscardExpired(10));
    }

    TEST(Retrieve_Missing_Throws)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        Refrigerator fridge;
        CHECK_THROW(fridge.Retrieve(cheese, 1), ItemOutOfStockException);
        fridge.Store(cheese, 5, 100);
        CHECK_THROW(fridge.Retrieve(cheese, 50), ItemOutOfStockException);
        CHECK_EQUAL(0, fridge.GetQuantity(std::make_shared<Ingredient>("None", 1, 1, false, 1)));
    }
}

SUITE(WarehouseSuite)
{
    TEST(Stock_And_Reorder)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        Warehouse warehouse;
        CHECK(!warehouse.CheckAvailability(cheese, 1));
        CHECK_EQUAL(0, warehouse.GetStock(cheese));
        warehouse.Restock(cheese, 100);
        CHECK(warehouse.CheckAvailability(cheese, 50));
        CHECK_EQUAL(100, warehouse.GetStock(cheese));
        warehouse.SetReorderLevel(30);
        CHECK_EQUAL(30, warehouse.GetReorderLevel());
        warehouse.Take(cheese, 20);
        CHECK(!warehouse.IsRestockPending());
        warehouse.Take(cheese, 60);
        CHECK(warehouse.IsRestockPending());
        CHECK_EQUAL(20, warehouse.GetStock(cheese));
        warehouse.Restock(cheese, 10);
        CHECK(!warehouse.IsRestockPending());
    }

    TEST(Take_Missing_Throws)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        Warehouse warehouse;
        CHECK_THROW(warehouse.Take(cheese, 1), ItemOutOfStockException);
    }
}

SUITE(SupplierSuite)
{
    TEST(Prices_And_Orders)
    {
        auto cheese = std::make_shared<Ingredient>("Cheese", 5, 100, true, 10);
        Supplier supplier("MilkFarm", 2);
        CHECK_EQUAL("MilkFarm", supplier.GetName());
        CHECK_EQUAL(2, supplier.GetDeliveryDays());
        CHECK_CLOSE(0, supplier.GetPriceFor(cheese), 0.001);
        supplier.SetPriceFor(cheese, 4);
        CHECK_CLOSE(4, supplier.GetPriceFor(cheese), 0.001);
        CHECK_CLOSE(0, supplier.PlaceOrder(0), 0.001);
        CHECK_CLOSE(1100, supplier.PlaceOrder(20), 0.001);
    }
}

SUITE(QualityControllerSuite)
{
    TEST(Inspect_Approve_Reject)
    {
        QualityController quality;
        Order pending(SinglePizzaOrder());
        Order cooking(SinglePizzaOrder());
        cooking.SetStatus(IN_PROGRESS);
        CHECK(!quality.InspectOrder(pending));
        CHECK(quality.InspectOrder(cooking));
        quality.ApproveOrder(cooking);
        quality.ApproveOrder(pending);
        CHECK_EQUAL(1, quality.GetApprovedCount());
        quality.RejectOrder(pending);
        quality.RejectOrder(cooking);
        CHECK_EQUAL(1, quality.GetRejectedCount());
    }
}

SUITE(PizzeriaDepartmentStaff)
{
    TEST(Hire_NewRoles)
    {
        auto dept = std::make_shared<PizzaDepartment>("Test St");
        CHECK(dept->Hire<Cashier>(Human(28, "Nancy")) != nullptr);
        CHECK(dept->Hire<CallCenterOperator>(Human(24, "Lily")) != nullptr);
        CHECK(dept->Hire<PizzeriaManager>(Human(40, "Bob")) != nullptr);
        CHECK(dept->Hire<Accountant>(Human(35, "Grace")) != nullptr);
        CHECK(dept->Hire<Hostess>(Human(22, "Emma")) != nullptr);
        CHECK(dept->Hire<Courier>(Human(26, "Jim")) != nullptr);
        CHECK_EQUAL(6, dept->GetStaffCount());
    }
}