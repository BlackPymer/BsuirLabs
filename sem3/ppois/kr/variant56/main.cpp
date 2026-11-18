#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <exception>

namespace Warehouse
{

    class Good
    {
    public:
        Good(std::string name)
        {
            name_ = name;
            id_ = last_id++;
        }
        std::string getName() const
        {
            return name_;
        }
        int getId() const { return id_; }

    private:
        inline static int last_id = 1;
        std::string name_;
        int id_;
    };

    class Box
    {
    public:
        Box(std::shared_ptr<Good> good, int quantity) : good_(good), quantity_(quantity) {};
        int getQuantity() { return quantity_; }
        std::shared_ptr<Good> getGood() const { return good_; }
        void decreaseQuantity(int quantity)
        {
            if (quantity_ < quantity)
                throw std::invalid_argument("Quantity is too large");
            quantity_ -= quantity;
        }
        void increaseQuantity(int quantity)
        {
            quantity_ += quantity;
        }

    private:
        std::shared_ptr<Good> good_;
        int quantity_;
    };

    class Rack
    {
    public:
        Rack(int id, int capacity) : id_(id), capacity_(capacity) {};
        bool AddBoxOnRack(std::shared_ptr<Box> bx)
        {
            if (bx->getQuantity() < capacity_ - curr_fill_)
            {
                box_on_rack_.push_back(*bx.get());
                curr_fill_ += bx->getQuantity();
                return true;
            }
            else
            {
                return false;
            }
        }

        bool AddInBox(std::shared_ptr<Box> bx, int quanty)
        {
            if (bx->getQuantity() < capacity_ - curr_fill_)
            {
                bx->increaseQuantity(quanty);
                curr_fill_ += bx->getQuantity();
                return true;
            }
            else
            {
                return false;
            }
        }

        std::shared_ptr<Box> findBox(int good_id) const
        {
            for (int i = 0; i < box_on_rack_.size(); ++i)
            {
                if (box_on_rack_[i].getGood()->getId() == good_id)
                {
                    return std::make_shared<Box>(box_on_rack_[i]);
                }
            }
            return nullptr;
        }

        void deleteBox(int good_id)
        {
            for (int i = 0; i < box_on_rack_.size(); ++i)
            {
                if (box_on_rack_[i].getGood()->getId() == good_id)
                {
                    box_on_rack_.erase(box_on_rack_.begin() + i);
                }
            }
        }

        std::vector<Box> &getAllBoxFromRack() { return box_on_rack_; }

    private:
        int id_;
        int capacity_;
        int curr_fill_ = 0;
        std::vector<Box> box_on_rack_;
    };

    class Waybill
    {
    public:
        Waybill(std::vector<Good> goods, std::vector<int> total_number) : goods_(goods), total_number_(total_number) {};
        std::vector<Good> &getGoodsList()
        {
            return goods_;
        }
        std::vector<int> &getTotalNumber() { return total_number_; }

    private:
        std::vector<Good> goods_;
        std::vector<int> total_number_;
    };

    class Sklad
    {
    public:
        Sklad()
        {
            racks_ = std::vector<Rack>();
        }

        std::vector<Rack> getAllRacks() { return racks_; }

        void addRack(int id, int capacity)
        {
            Rack rk(id, capacity);
            racks_.push_back(rk);
        }

        int getGoodCount(int id) const
        {
            for (const Rack &rack : racks_)
            {
                auto get = rack.findBox(id);
                if (get != nullptr)
                {
                    return get->getQuantity();
                }
            }
            return 0;
        }

        std::shared_ptr<Box> getGood(int id, int quantity)
        {
            for (Rack &rack : racks_)
            {
                auto get = rack.findBox(id);
                if (get == nullptr)
                    continue;
                if (get->getQuantity() > quantity)
                {
                    get->decreaseQuantity(quantity);
                    std::shared_ptr<Box> box = std::make_shared<Box>(Box(get->getGood(), quantity));
                    return box;
                }
                else if (get->getQuantity() == quantity)
                {
                    rack.deleteBox(id);
                    return get;
                }
                else
                {
                    return nullptr;
                }
            }
            return nullptr;
        }

        bool addGood(std::shared_ptr<Box> box)
        {
            for (Rack &rack : racks_)
            {
                std::shared_ptr<Box> get = rack.findBox(box->getGood()->getId());
                if (get != nullptr)
                {
                    bool check = rack.AddInBox(get, box->getQuantity());
                    if (check)
                        return true;
                    else
                    {
                        box->increaseQuantity(get->getQuantity());
                        rack.deleteBox(get->getGood()->getId());
                    }
                }
                else
                {
                    bool check = rack.AddBoxOnRack(box);
                    if (check)
                        return true;
                }
            }
            return false;
        }

    private:
        std::vector<Rack> racks_;
    };

    class StoreKeeper
    {
    public:
        StoreKeeper(std::shared_ptr<Sklad> input, std::shared_ptr<Sklad> output) : input_(input), output_(output) {};

        bool deliver(Waybill delivery)
        {
            // ни при каких условиях эта хуета не должна оказаться в продакшен коде! не дай бог нахуй
            //  так как это не является частью основной бизнес-логики и может привести к утечкам и ошибкам в работе системы
            //  временное решение  на демо версии 18.11.2026(полная хуета)
            auto list = delivery.getGoodsList();
            for (int i = 0; i < list.size(); ++i)
            {
                auto box = input_->getGood(list[i].getId(), delivery.getTotalNumber()[i]);
                if (box == nullptr)
                {
                    std::cout << "SOSITE HUY CHERTI!!!!!!!" << "\n";
                    return false;
                }
                auto answer = output_->addGood(box);
                if (!answer) {
                    std::cout << "I NEED MORE SPACE PIDARASY!!!!!!!!!!!!!!!" << "\n";
                    return false;
                }
                return true;
            }
        }

    private:
        std::shared_ptr<Sklad> input_;
        std::shared_ptr<Sklad> output_;
    };

    class Manager
    {
    public:
        Manager(std::shared_ptr<Sklad> warehouse, std::shared_ptr<Sklad> input_zone, std::shared_ptr<Sklad> out_put_zone) : warehouse_(warehouse), input_zone_(input_zone), output_zone_(out_put_zone) {};
        void print_report()
        {
            if (warehouse_ != nullptr)
            {
                std::cout << "Зона склада" << std::endl;
                for (int i = 0; i < warehouse_->getAllRacks().size(); ++i)
                {
                    Rack rc = warehouse_->getAllRacks()[i];
                    for (int j = 0; j < rc.getAllBoxFromRack().size(); ++j)
                    {
                        std::cout << rc.getAllBoxFromRack()[j].getGood()->getName() << ": " << rc.getAllBoxFromRack()[j].getQuantity() << std::endl;
                    }
                }
            }

            if (input_zone_ != nullptr)
            {
                std::cout << "Зона загрузки" << std::endl;
                for (int i = 0; i < input_zone_->getAllRacks().size(); ++i)
                {
                    Rack rc = input_zone_->getAllRacks()[i];
                    for (int j = 0; j < rc.getAllBoxFromRack().size(); ++j)
                    {
                        std::cout << rc.getAllBoxFromRack()[j].getGood()->getName() << ": " << rc.getAllBoxFromRack()[j].getQuantity() << std::endl;
                    }
                }
            }

            if (output_zone_ != nullptr)
            {
                std::cout << "Зона выгрузки" << std::endl;
                for (int i = 0; i < output_zone_->getAllRacks().size(); ++i)
                {
                    Rack rc = output_zone_->getAllRacks()[i];
                    for (int j = 0; j < rc.getAllBoxFromRack().size(); ++j)
                    {
                        std::cout << rc.getAllBoxFromRack()[j].getGood()->getName() << ": " << rc.getAllBoxFromRack()[j].getQuantity() << std::endl;
                    }
                }
            }
        }

    private:
        std::shared_ptr<Sklad> warehouse_;
        std::shared_ptr<Sklad> input_zone_;
        std::shared_ptr<Sklad> output_zone_;
    };
}

int main()
{
    using namespace Warehouse;
    return 0;
}