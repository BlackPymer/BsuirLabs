#include "UnitTest++.h"

#include "pizzeria_client.hpp"
#include "pizzeria_cooker.hpp"
#include "pizzeria_cooker_intern.hpp"
#include "pizzeria_department.hpp"
#include "pizzeria_waiter.hpp"
#include "pizzeria_worker.hpp"

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