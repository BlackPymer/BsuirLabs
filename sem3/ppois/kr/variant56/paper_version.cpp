// Вариант 56 — «бумажная» версия: минимум синтаксиса, максимум предметной области.
// Влезает на 2 листа A4 (~120 строк) и пишется за ~50-60 минут.
// #include <iostream> <vector> <deque> <string> <stdexcept> — подразумеваются.


namespace warehouse
{

    struct Good
    { // товар: уникальный id + название
        int id;
        std::string name;
    };

    struct Item
    { // позиция: товар + количество
        Good good;
        int qty;
    };
    class Rack
    {
    public:
        Rack(int code, int cap) : code_(code), cap_(cap), used_(0) {}

        int code() const { return code_; }
        int freeSpace() const { return cap_ - used_; }
        const std::vector<Item> &items() const { return items_; }

        void place(const Item &it)
        { // разместить партию
            if (it.qty > freeSpace())
                throw std::runtime_error("нет места на стеллаже");
            for (Item &cur : items_)
            { // тот же товар — суммируем
                if (cur.good.id == it.good.id)
                {
                    cur.qty += it.qty;
                    used_ += it.qty;
                    return;
                }
            }
            items_.push_back(it);
            used_ += it.qty;
        }

        Item take(int goodId, int qty)
        { // забрать партию
            for (auto it = items_.begin(); it != items_.end(); ++it)
            {
                if (it->good.id != goodId)
                    continue;
                if (it->qty < qty)
                    throw std::runtime_error("недостаточно товара");
                Item res{it->good, qty};
                if (it->qty == qty)
                    items_.erase(it);
                else
                    it->qty -= qty;
                used_ -= qty;
                return res;
            }
            throw std::runtime_error("товар не найден");
        }

        int qtyOf(int goodId) const
        {
            for (const Item &it : items_)
                if (it.good.id == goodId)
                    return it.qty;
            return 0;
        }

    private:
        int code_, cap_, used_;
        std::vector<Item> items_;
    };

    class Zone
    { // зона: приёмка / хранение / отгрузка
    public:
        Zone(std::string name) : name_(name) {}
        const std::string &name() const { return name_; }
        void addRack(int code, int cap) { racks_.push_back(Rack(code, cap)); }
        int rackCount() const { return racks_.size(); }
        Rack &rack(int i) { return racks_[i]; }
        const Rack &rack(int i) const { return racks_[i]; }

        int qtyOf(int goodId) const
        { // ищем по ВСЕМ стеллажам
            int sum = 0;
            for (const Rack &r : racks_)
                sum += r.qtyOf(goodId);
            return sum;
        }
        int freeSpace() const
        {
            int sum = 0;
            for (const Rack &r : racks_)
                sum += r.freeSpace();
            return sum;
        }

    private:
        std::string name_;
        std::vector<Rack> racks_;
    };

    class Waybill
    { // накладная: на приёмку или на отгрузку
    public:
        void add(const Good &g, int qty) { items_.push_back({g, qty}); }
        const std::vector<Item> &items() const { return items_; }

    private:
        std::vector<Item> items_;
    };

    class StoreKeeper
    { // кладовщик: накладная -> стеллажи
    public:
        void receive(Zone &storage, const Waybill &wb)
        {
            for (const Item &it : wb.items())
            { // сначала проверка места
                if (storage.freeSpace() < it.qty)
                    throw std::runtime_error("на складе нет места");
            }
            for (const Item &it : wb.items())
            {
                Rack *target = nullptr;
                for (int i = 0; i < storage.rackCount(); ++i)
                    if (storage.rack(i).freeSpace() >= it.qty)
                    {
                        target = &storage.rack(i);
                        break;
                    }
                if (!target)
                    throw std::runtime_error("нет места");
                target->place(it);
            }
        }
    };

    class Picker
    { // комплектовщик: стеллажи -> зона отгрузки
    public:
        void ship(Zone &storage, Zone &shipping, const Waybill &wb)
        {
            for (const Item &it : wb.items()) // проверка наличия
                if (storage.qtyOf(it.good.id) < it.qty)
                    throw std::runtime_error("недостаточно товара на складе");
            if (shipping.freeSpace() < wbTotal(wb))
                throw std::runtime_error("нет места в зоне отгрузки");

            for (const Item &it : wb.items())
            {
                int left = it.qty;
                for (int i = 0; i < storage.rackCount() && left > 0; ++i)
                {
                    int have = storage.rack(i).qtyOf(it.good.id);
                    if (have <= 0)
                        continue;
                    int part = (have < left) ? have : left;
                    Item taken = storage.rack(i).take(it.good.id, part);
                    shipping.rack(0).place(taken); // в демо одна зона отгрузки
                    left -= part;
                }
            }
        }

    private:
        int wbTotal(const Waybill &wb) const
        {
            int s = 0;
            for (const Item &it : wb.items())
                s += it.qty;
            return s;
        }
    };

    class Manager
    { // начальник: зоны, стеллажи, отчёт
    public:
        // deque: ссылки на зоны не инвалидируются при создании новых (важно для ролей)
        Zone &createZone(std::string name)
        {
            zones_.emplace_back(name);
            return zones_.back();
        }
        void addRack(Zone &z, int code, int cap) { z.addRack(code, cap); }
        void report(const Zone &z) const
        {
            std::cout << "Зона: " << z.name() << "\n";
            for (int i = 0; i < z.rackCount(); ++i)
                for (const Item &it : z.rack(i).items())
                    std::cout << "  " << it.good.name << ": " << it.qty << "\n";
        }

    private:
        std::deque<Zone> zones_;
    };

} // namespace warehouse
