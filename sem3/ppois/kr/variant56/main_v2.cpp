// Вариант 56 — чистая реализация предметной области «Склад».
//
// Сущности:
//   Good   — товар (уникальный id + название);
//   Box    — партия товара на стеллаже (неизменяемое значение: good + quantity);
//   Rack   — стеллаж (код, вместимость, занятое место, партии);
//   Zone   — зона склада (приёмка / хранение / отгрузка), содержит стеллажи;
//   Sklad  — склад, содержит зоны;
//   Waybill — накладная (приёмка / отгрузка), список позиций «товар + количество»;
//   StoreKeeper — кладовщик: раскладывает товар из накладной на приёмку по стеллажам;
//   Picker      — комплектовщик: собирает товар по накладной на отгрузку в зону отгрузки;
//   Manager     — начальник склада: создаёт/удаляет зоны и стеллажи, строит отчёт.
//
// Ключевые принципы, которых не было в старой версии:
//   * состояние склада живёт в одном месте (Rack), снаружи отдаются const-ссылки;
//   * операции «разместить» и «отгрузить» атомарны: либо всё, либо ничего;
//   * ошибки — исключения, а не std::cout из бизнес-логики;
//   * отсутствие копирования сущностей «на всякий случай» (нет копий Box в Rack и т.п.).

#include <algorithm>
#include <cstddef>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
// дипсик лох
namespace warehouse
{

    // ---------------------------------------------------------------------------
    // Товар
    // ---------------------------------------------------------------------------

    class Good
    {
    public:
        explicit Good(std::string name) : name_(std::move(name)), id_(next_id_++)
        {
            if (name_.empty())
            {
                throw std::invalid_argument("Good: название товара не может быть пустым");
            }
        }

        [[nodiscard]] int id() const noexcept { return id_; }
        [[nodiscard]] const std::string &name() const noexcept { return name_; }

    private:
        inline static int next_id_ = 1;

        std::string name_;
        int id_;
    };

    // ---------------------------------------------------------------------------
    // Партия товара
    // ---------------------------------------------------------------------------

    class Box
    {
    public:
        Box(std::shared_ptr<const Good> good, int quantity)
            : good_(std::move(good)), quantity_(quantity)
        {
            if (!good_)
            {
                throw std::invalid_argument("Box: товар не задан");
            }
            if (quantity_ <= 0)
            {
                throw std::invalid_argument("Box: количество должно быть положительным");
            }
        }

        [[nodiscard]] const Good &good() const noexcept { return *good_; }
        [[nodiscard]] std::shared_ptr<const Good> goodPtr() const noexcept { return good_; }
        [[nodiscard]] int goodId() const noexcept { return good_->id(); }
        [[nodiscard]] int quantity() const noexcept { return quantity_; }

        // Партия неизменяема: объединение — это новая партия, а не мутация старой.
        [[nodiscard]] Box withAdded(int extra) const
        {
            return Box(good_, quantity_ + extra);
        }

    private:
        std::shared_ptr<const Good> good_;
        int quantity_;
    };

    // ---------------------------------------------------------------------------
    // Стеллаж
    // ---------------------------------------------------------------------------

    class Rack
    {
    public:
        Rack(int code, int capacity) : code_(code), capacity_(capacity)
        {
            if (capacity_ <= 0)
            {
                throw std::invalid_argument("Rack: вместимость должна быть положительной");
            }
        }

        [[nodiscard]] int code() const noexcept { return code_; }
        [[nodiscard]] int capacity() const noexcept { return capacity_; }
        [[nodiscard]] int used() const noexcept { return used_; }
        [[nodiscard]] int freeSpace() const noexcept { return capacity_ - used_; }
        [[nodiscard]] const std::vector<Box> &boxes() const noexcept { return boxes_; }

        [[nodiscard]] bool canFit(int quantity) const noexcept
        {
            return quantity > 0 && quantity <= freeSpace();
        }

        [[nodiscard]] bool contains(int good_id) const noexcept
        {
            return quantityOf(good_id) > 0;
        }

        [[nodiscard]] int quantityOf(int good_id) const noexcept
        {
            for (const Box &box : boxes_)
            {
                if (box.goodId() == good_id)
                {
                    return box.quantity();
                }
            }
            return 0;
        }

        // Размещает партию целиком. Если не влезает — исключение, состояние не меняется.
        void place(const Box &box)
        {
            if (!canFit(box.quantity()))
            {
                throw std::overflow_error("Стеллаж " + std::to_string(code_) +
                                          ": не хватает места для " + std::to_string(box.quantity()) +
                                          " ед. (свободно " + std::to_string(freeSpace()) + ")");
            }

            for (Box &existing : boxes_)
            {
                if (existing.goodId() == box.goodId())
                {
                    existing = existing.withAdded(box.quantity());
                    used_ += box.quantity();
                    return;
                }
            }

            boxes_.push_back(box);
            used_ += box.quantity();
        }

        // Забирает quantity единиц товара good_id. Если столько нет — исключение.
        Box take(int good_id, int quantity)
        {
            if (quantity <= 0)
            {
                throw std::invalid_argument("Rack::take: количество должно быть положительным");
            }

            for (auto it = boxes_.begin(); it != boxes_.end(); ++it)
            {
                if (it->goodId() != good_id)
                {
                    continue;
                }
                if (it->quantity() < quantity)
                {
                    throw std::out_of_range("Стеллаж " + std::to_string(code_) +
                                            ": товара id=" + std::to_string(good_id) +
                                            " только " + std::to_string(it->quantity()) +
                                            ", запрошено " + std::to_string(quantity));
                }

                Box taken(it->goodPtr(), quantity);
                if (it->quantity() == quantity)
                {
                    boxes_.erase(it);
                }
                else
                {
                    *it = Box(it->goodPtr(), it->quantity() - quantity);
                }
                used_ -= quantity;
                return taken;
            }

            throw std::out_of_range("Стеллаж " + std::to_string(code_) +
                                    ": товар id=" + std::to_string(good_id) + " не найден");
        }

    private:
        int code_;
        int capacity_;
        int used_ = 0;
        std::vector<Box> boxes_;
    };

    // ---------------------------------------------------------------------------
    // Зона склада
    // ---------------------------------------------------------------------------

    enum class ZoneType
    {
        Receiving,
        Storage,
        Shipping
    };

    inline std::string_view toString(ZoneType type) noexcept
    {
        switch (type)
        {
        case ZoneType::Receiving:
            return "зона приёмки";
        case ZoneType::Storage:
            return "зона хранения";
        case ZoneType::Shipping:
            return "зона отгрузки";
        }
        return "неизвестная зона";
    }

    class Zone
    {
    public:
        Zone(ZoneType type, std::string name) : type_(type), name_(std::move(name))
        {
            if (name_.empty())
            {
                throw std::invalid_argument("Zone: название зоны не может быть пустым");
            }
        }

        [[nodiscard]] ZoneType type() const noexcept { return type_; }
        [[nodiscard]] const std::string &name() const noexcept { return name_; }
        [[nodiscard]]чччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччччч std::size_t rackCount() const noexcept { return racks_.size(); }
        [[nodiscard]] const std::vector<Rack> &racks() const noexcept { return racks_; }
        [[nodiscard]] Rack &rackAt(std::size_t index) { return racks_.at(index); }
        [[nodiscard]] const Rack &rackAt(std::size_t index) const { return racks_.at(index); }

        [[nodiscard]] Rack *findRack(int code) noexcept
        {
            for (Rack &rack : racks_)
            {
                if (rack.code() == code)
                {
                    return &rack;
                }
            }
            return nullptr;
        }

        [[nodiscard]] const Rack *findRack(int code) const noexcept
        {
            for (const Rack &rack : racks_)
            {
                if (rack.code() == code)
                {
                    return &rack;
                }
            }
            return nullptr;
        }

        Rack &addRack(int code, int capacity)
        {
            if (findRack(code) != nullptr)
            {
                throw std::invalid_argument("Зона '" + name_ + "': стеллаж " +
                                            std::to_string(code) + " уже существует");
            }
            racks_.emplace_back(code, capacity);
            return racks_.back();
        }

        void removeRack(int code)
        {
            auto it = std::find_if(racks_.begin(), racks_.end(),
                                   [code](const Rack &rack)
                                   { return rack.code() == code; });
            if (it == racks_.end())
            {
                throw std::invalid_argument("Зона '" + name_ + "': стеллаж " +
                                            std::to_string(code) + " не найден");
            }
            if (it->used() != 0)
            {
                throw std::logic_error("Зона '" + name_ + "': нельзя удалить непустой стеллаж " +
                                       std::to_string(code));
            }
            racks_.erase(it);
        }

        [[nodiscard]] int quantityOf(int good_id) const noexcept
        {
            int total = 0;
            for (const Rack &rack : racks_)
            {
                total += rack.quantityOf(good_id);
            }
            return total;
        }

        [[nodiscard]] int totalQuantity() const noexcept
        {
            int total = 0;
            for (const Rack &rack : racks_)
            {
                total += rack.used();
            }
            return total;
        }

        [[nodiscard]] int freeSpace() const noexcept
        {
            int total = 0;
            for (const Rack &rack : racks_)
            {
                total += rack.freeSpace();
            }
            return total;
        }

    private:
        ZoneType type_;
        std::string name_;
        std::vector<Rack> racks_;
    };

    // ---------------------------------------------------------------------------
    // Склад
    // ---------------------------------------------------------------------------

    class Sklad
    {
    public:
        // deque, а не vector: ссылки на зоны не инвалидируются при добавлении новых,
        // а роли (StoreKeeper, Picker) держат эти ссылки всё время работы.
        Zone &addZone(ZoneType type, std::string name)
        {
            if (findZone(name) != nullptr)
            {
                throw std::invalid_argument("Склад: зона '" + name + "' уже существует");
            }
            zones_.emplace_back(type, std::move(name));
            return zones_.back();
        }

        void removeZone(const std::string &name)
        {
            auto it = std::find_if(zones_.begin(), zones_.end(),
                                   [&name](const Zone &zone)
                                   { return zone.name() == name; });
            if (it == zones_.end())
            {
                throw std::invalid_argument("Склад: зона '" + name + "' не найдена");
            }
            if (it->totalQuantity() != 0)
            {
                throw std::logic_error("Склад: нельзя удалить непустую зону '" + name + "'");
            }
            zones_.erase(it);
        }

        [[nodiscard]] Zone *findZone(const std::string &name) noexcept
        {
            for (Zone &zone : zones_)
            {
                if (zone.name() == name)
                {
                    return &zone;
                }
            }
            return nullptr;
        }

        [[nodiscard]] const Zone *findZone(const std::string &name) const noexcept
        {
            for (const Zone &zone : zones_)
            {
                if (zone.name() == name)
                {
                    return &zone;
                }
            }
            return nullptr;
        }

        Zone &zone(const std::string &name)
        {
            Zone *found = findZone(name);
            if (found == nullptr)
            {
                throw std::invalid_argument("Склад: зона '" + name + "' не найдена");
            }
            return *found;
        }

        [[nodiscard]] const std::deque<Zone> &zones() const noexcept { return zones_; }

    private:
        std::deque<Zone> zones_;
    };

    // ---------------------------------------------------------------------------
    // Накладные
    // ---------------------------------------------------------------------------

    struct WaybillItem
    {
        std::shared_ptr<const Good> good;
        int quantity;
    };

    class Waybill
    {
    public:
        explicit Waybill(std::vector<WaybillItem> items) : items_(std::move(items))
        {
            if (items_.empty())
            {
                throw std::invalid_argument("Накладная: нужна хотя бы одна позиция");
            }
            for (const WaybillItem &item : items_)
            {
                if (!item.good)
                {
                    throw std::invalid_argument("Накладная: позиция без товара");
                }
                if (item.quantity <= 0)
                {
                    throw std::invalid_argument("Накладная: количество должно быть положительным");
                }
            }
            for (std::size_t i = 0; i < items_.size(); ++i)
            {
                for (std::size_t j = i + 1; j < items_.size(); ++j)
                {
                    if (items_[i].good->id() == items_[j].good->id())
                    {
                        throw std::invalid_argument("Накладная: товар id=" +
                                                    std::to_string(items_[i].good->id()) +
                                                    " указан дважды");
                    }
                }
            }
        }

        [[nodiscard]] const std::vector<WaybillItem> &items() const noexcept { return items_; }

        [[nodiscard]] int totalQuantity() const noexcept
        {
            int total = 0;
            for (const WaybillItem &item : items_)
            {
                total += item.quantity;
            }
            return total;
        }

    private:
        std::vector<WaybillItem> items_;
    };

    // Разные типы накладных нельзя перепутать на уровне типов.
    class ReceivingWaybill final : public Waybill
    {
    public:
        using Waybill::Waybill;
    };

    class ShippingWaybill final : public Waybill
    {
    public:
        using Waybill::Waybill;
    };

    // ---------------------------------------------------------------------------
    // Планирование размещения
    // ---------------------------------------------------------------------------

    struct PlannedItem
    {
        int good_id;
        int quantity;
    };

    // Подбирает, на какой стеллаж пойдёт каждая партия, не меняя состояние склада.
    // Сначала пробуем стеллаж, где такой товар уже лежит (уплотняем хранение),
    // иначе — первый стеллаж с достаточным свободным местом.
    // std::nullopt — места не хватает хотя бы на одну партию.
    [[nodiscard]] std::optional<std::vector<std::size_t>> planPlacement(
        const std::vector<Rack> &racks,
        const std::vector<PlannedItem> &items)
    {
        std::vector<int> free_space;
        free_space.reserve(racks.size());
        for (const Rack &rack : racks)
        {
            free_space.push_back(rack.freeSpace());
        }

        std::vector<std::size_t> plan;
        plan.reserve(items.size());

        for (const PlannedItem &item : items)
        {
            std::size_t target = racks.size();

            for (std::size_t i = 0; i < racks.size(); ++i)
            {
                if (racks[i].contains(item.good_id) && free_space[i] >= item.quantity)
                {
                    target = i;
                    break;
                }
            }
            if (target == racks.size())
            {
                for (std::size_t i = 0; i < racks.size(); ++i)
                {
                    if (free_space[i] >= item.quantity)
                    {
                        target = i;
                        break;
                    }
                }
            }
            if (target == racks.size())
            {
                return std::nullopt;
            }

            free_space[target] -= item.quantity;
            plan.push_back(target);
        }

        return plan;
    }

    [[nodiscard]] inline std::vector<PlannedItem> toPlanned(const Waybill &waybill)
    {
        std::vector<PlannedItem> planned;
        planned.reserve(waybill.items().size());
        for (const WaybillItem &item : waybill.items())
        {
            planned.push_back({item.good->id(), item.quantity});
        }
        return planned;
    }

    // ---------------------------------------------------------------------------
    // Кладовщик
    // ---------------------------------------------------------------------------

    class StoreKeeper
    {
    public:
        explicit StoreKeeper(Zone &storage) : storage_(storage)
        {
            if (storage_.type() != ZoneType::Storage)
            {
                throw std::invalid_argument("Кладовщик: зона '" + storage_.name() +
                                            "' не является зоной хранения");
            }
        }

        // Раскладывает товар из накладной на приёмку по стеллажам зоны хранения,
        // увеличивая общее количество товара. Атомарно: если хоть одна позиция
        // не помещается — не размещается ничего.
        void receive(const ReceivingWaybill &waybill)
        {
            const std::vector<PlannedItem> planned = toPlanned(waybill);
            const auto plan = planPlacement(storage_.racks(), planned);
            if (!plan)
            {
                throw std::runtime_error("Кладовщик: в зоне хранения '" + storage_.name() +
                                         "' недостаточно места для накладной");
            }

            for (std::size_t i = 0; i < planned.size(); ++i)
            {
                const WaybillItem &item = waybill.items()[i];
                storage_.rackAt((*plan)[i]).place(Box(item.good, item.quantity));
            }
        }

    private:
        Zone &storage_;
    };

    // ---------------------------------------------------------------------------
    // Комплектовщик
    // ---------------------------------------------------------------------------

    class Picker
    {
    public:
        Picker(Zone &storage, Zone &shipping) : storage_(storage), shipping_(shipping)
        {
            if (storage_.type() != ZoneType::Storage)
            {
                throw std::invalid_argument("Комплектовщик: зона '" + storage_.name() +
                                            "' не является зоной хранения");
            }
            if (shipping_.type() != ZoneType::Shipping)
            {
                throw std::invalid_argument("Комплектовщик: зона '" + shipping_.name() +
                                            "' не является зоной отгрузки");
            }
        }

        // Собирает товар по накладной на отгрузку: уменьшает количество на стеллажах
        // и перемещает его в зону отгрузки. Атомарно: сначала все проверки
        // (хватает товара + хватает места), только потом изменения.
        void ship(const ShippingWaybill &waybill)
        {
            const std::vector<PlannedItem> planned = toPlanned(waybill);

            for (const PlannedItem &item : planned)
            {
                const int available = storage_.quantityOf(item.good_id);
                if (available < item.quantity)
                {
                    throw std::runtime_error("Комплектовщик: товара id=" +
                                             std::to_string(item.good_id) + " на складе только " +
                                             std::to_string(available) + ", требуется " +
                                             std::to_string(item.quantity));
                }
            }

            const auto plan = planPlacement(shipping_.racks(), planned);
            if (!plan)
            {
                throw std::runtime_error("Комплектовщик: в зоне отгрузки '" + shipping_.name() +
                                         "' недостаточно места");
            }

            for (std::size_t i = 0; i < planned.size(); ++i)
            {
                int remaining = planned[i].quantity;
                for (std::size_t r = 0; r < storage_.rackCount() && remaining > 0; ++r)
                {
                    Rack &rack = storage_.rackAt(r);
                    const int on_rack = rack.quantityOf(planned[i].good_id);
                    if (on_rack <= 0)
                    {
                        continue;
                    }
                    const int chunk = std::min(on_rack, remaining);
                    Box box = rack.take(planned[i].good_id, chunk);
                    shipping_.rackAt((*plan)[i]).place(box);
                    remaining -= chunk;
                }
                if (remaining > 0)
                {
                    // Сюда попасть нельзя: наличие проверено выше.
                    throw std::logic_error("Комплектовщик: внутренняя ошибка планирования");
                }
            }
        }

    private:
        Zone &storage_;
        Zone &shipping_;
    };

    // ---------------------------------------------------------------------------
    // Начальник склада
    // ---------------------------------------------------------------------------

    class Manager
    {
    public:
        explicit Manager(Sklad &sklad) : sklad_(sklad) {}

        Zone &createZone(ZoneType type, const std::string &name)
        {
            return sklad_.addZone(type, name);
        }

        void deleteZone(const std::string &name)
        {
            sklad_.removeZone(name);
        }

        Rack &createRack(const std::string &zone_name, int code, int capacity)
        {
            return sklad_.zone(zone_name).addRack(code, capacity);
        }

        void deleteRack(const std::string &zone_name, int code)
        {
            sklad_.zone(zone_name).removeRack(code);
        }

        [[nodiscard]] std::string report() const
        {
            std::ostringstream out;
            out << "=== Отчёт об остатках склада ===\n";

            if (sklad_.zones().empty())
            {
                out << "(зон нет)\n";
                return out.str();
            }

            for (const Zone &zone : sklad_.zones())
            {
                out << "\n[" << toString(zone.type()) << "] " << zone.name()
                    << " — всего " << zone.totalQuantity() << " ед., свободно "
                    << zone.freeSpace() << " ед.\n";
                if (zone.rackCount() == 0)
                {
                    out << "  (стеллажей нет)\n";
                    continue;
                }
                for (const Rack &rack : zone.racks())
                {
                    out << "  стеллаж #" << rack.code() << " [" << rack.used() << "/"
                        << rack.capacity() << "]\n";
                    if (rack.boxes().empty())
                    {
                        out << "    (пусто)\n";
                        continue;
                    }
                    for (const Box &box : rack.boxes())
                    {
                        out << "    " << box.good().name() << " (id=" << box.goodId()
                            << "): " << box.quantity() << "\n";
                    }
                }
            }
            return out.str();
        }

    private:
        Sklad &sklad_;
    };

} // namespace warehouse

// ---------------------------------------------------------------------------
// Демонстрация
// ---------------------------------------------------------------------------

int main()
{
    using namespace warehouse;

    try
    {
        Sklad sklad;
        Manager manager(sklad);

        // Начальник склада создаёт зоны и стеллажи.
        manager.createZone(ZoneType::Receiving, "Зона приёмки");
        manager.createZone(ZoneType::Storage, "Зона хранения");
        manager.createZone(ZoneType::Shipping, "Зона отгрузки");

        manager.createRack("Зона приёмки", 1, 100);
        manager.createRack("Зона хранения", 10, 500);
        manager.createRack("Зона хранения", 11, 300);
        manager.createRack("Зона отгрузки", 20, 400);

        const auto laptop = std::make_shared<Good>("Ноутбук");
        const auto mouse = std::make_shared<Good>("Мышь");
        const auto keyboard = std::make_shared<Good>("Клавиатура");

        Zone &storage = sklad.zone("Зона хранения");
        Zone &shipping = sklad.zone("Зона отгрузки");

        // Приёмка: кладовщик раскладывает товар по стеллажам.
        const ReceivingWaybill arrival({
            {laptop, 40},
            {mouse, 150},
            {keyboard, 60},
        });

        StoreKeeper store_keeper(storage);
        store_keeper.receive(arrival);
        std::cout << "\nПосле приёмки:\n"
                  << manager.report();

        // Отгрузка: комплектовщик собирает заказ в зону отгрузки.
        const ShippingWaybill order({
            {laptop, 20},
            {mouse, 100},
        });

        Picker picker(storage, shipping);
        picker.ship(order);
        std::cout << "\nПосле отгрузки:\n"
                  << manager.report();

        // --- Ожидаемые ошибки: состояние склада не должно меняться ---

        // 1. Не хватает места под накладную.
        try
        {
            const ReceivingWaybill too_big({{keyboard, 100000}});
            store_keeper.receive(too_big);
        }
        catch (const std::exception &error)
        {
            std::cout << "\n[ожидаемая ошибка] " << error.what() << "\n";
        }

        // 2. Не хватает товара на складе под накладную на отгрузку.
        try
        {
            const ShippingWaybill impossible({{keyboard, 9999}});
            picker.ship(impossible);
        }
        catch (const std::exception &error)
        {
            std::cout << "[ожидаемая ошибка] " << error.what() << "\n";
        }

        // 3. Атомарность приёмки: первая позиция влезает, вторая — нет,
        //    значит не должна разместиться и первая.
        const int laptops_before = storage.quantityOf(laptop->id());
        try
        {
            const ReceivingWaybill partly_too_big({
                {laptop, 10},
                {mouse, 100000},
            });
            store_keeper.receive(partly_too_big);
        }
        catch (const std::exception &error)
        {
            std::cout << "[ожидаемая ошибка] " << error.what() << "\n";
        }
        std::cout << "Ноутбуков до/после неудачной приёмки: " << laptops_before << " / "
                  << storage.quantityOf(laptop->id()) << "\n";

        // 4. Удаление непустого стеллажа и непустой зоны запрещено.
        try
        {
            manager.deleteRack("Зона хранения", 10);
        }
        catch (const std::exception &error)
        {
            std::cout << "[ожидаемая ошибка] " << error.what() << "\n";
        }
        try
        {
            manager.deleteZone("Зона хранения");
        }
        catch (const std::exception &error)
        {
            std::cout << "[ожидаемая ошибка] " << error.what() << "\n";
        }

        // 5. Пустую зону удалить можно.
        manager.deleteRack("Зона приёмки", 1);
        manager.deleteZone("Зона приёмки");
        std::cout << "\nПосле удаления пустой зоны приёмки:\n"
                  << manager.report();
    }
    catch (const std::exception &error)
    {
        std::cerr << "Критическая ошибка: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
