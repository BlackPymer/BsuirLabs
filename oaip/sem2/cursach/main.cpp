#include <iostream>
#include <cstring>
#include <iomanip>
#include <filesystem>
#include <cstddef>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif
const char *DATA_FILENAME = "data.bin";
const char *TMP_FILENAME = "tmp.bin";

const int COL_NUMBER = 15;
const int COL_BUS_TYPE = 20;
const int COL_DESTINATION = 35;
const int COL_DATE = 15;
const int COL_TIME = 15;
const int COL_COST = 15;
const int COL_LEFT = 15;
const int COL_SOLD = 15;
const int ROW_SIZE = COL_NUMBER + COL_BUS_TYPE + COL_DESTINATION + COL_DATE + COL_TIME + COL_COST + COL_LEFT + COL_SOLD;

struct Trip
{
    int number;
    char bus_type[50];
    char destination[50];
    char date_departure[20];
    char time_departure[10];
    char time_arrival[10];
    float ticket_cost;
    int tickets_left;
    int tickets_sold;

    Trip() : number(0), ticket_cost(0), tickets_left(0), tickets_sold(0)
    {
        bus_type[0] = '\0';
        destination[0] = '\0';
        date_departure[0] = '\0';
        time_departure[0] = '\0';
        time_arrival[0] = '\0';
    }

    Trip(int number, const char *bus_type, const char *destination,
         const char *date_departure, const char *time_departure,
         const char *time_arrival, float ticket_cost, int tickets_left,
         int tickets_sold) : number(number), ticket_cost(ticket_cost),
                             tickets_left(tickets_left), tickets_sold(tickets_sold)
    {
        strcpy(this->bus_type, bus_type);
        strcpy(this->destination, destination);
        strcpy(this->date_departure, date_departure);
        strcpy(this->time_departure, time_departure);
        strcpy(this->time_arrival, time_arrival);
    }
};

void print_trip(Trip *trip);
void print_trips(const char *filename);

#pragma region STACK
struct StackData
{
    int l;
    int r;
    StackData(int left, int right)
    {
        l = left;
        r = right;
    }
    StackData() {}
};

struct StackNode
{
    StackData data;
    StackNode *next = nullptr;
    StackNode() {}
    StackNode(StackData val) { data = val; }
};
struct Stack
{
    StackNode *root = nullptr;
    Stack() {}
    Stack(StackData val)
    {
        push(val);
    }
    void push(StackData val)
    {
        StackNode *node = new StackNode(val);
        node->next = root;
        root = node;
    }
    StackData pop()
    {
        if (root == nullptr)
        {
            std::cerr << "Stack is null";
            exit(1);
        }
        StackNode *tmp = root;
        root = root->next;
        StackData res = tmp->data;
        delete tmp;
        return res;
    }
    StackData *top()
    {
        return (root) ? &root->data : nullptr;
    }
    ~Stack()
    {
        StackNode *tmp = root;
        while (root != nullptr)
        {
            root = root->next;
            delete tmp;
            tmp = root;
        }
    }
};
#pragma endregion

#pragma region FILES WORKING
long get_struct_num(const char *filename = DATA_FILENAME)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return 0;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size / sizeof(Trip);
}
Trip *get_trip(int ind, const char *filename = DATA_FILENAME)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return nullptr;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);

    if (sizeof(Trip) * ind >= size)
    {
        fclose(file);
        return nullptr;
    }

    Trip *tr = new Trip();
    fseek(file, sizeof(Trip) * ind, SEEK_SET);
    fread(tr, sizeof(Trip), 1, file);
    fclose(file);
    return tr;
}

void set_trip(int ind, Trip *trip, const char *filename = DATA_FILENAME)
{
    FILE *file = fopen(filename, "r+b");
    if (!file)
        file = fopen(filename, "wb");
    if (!file)
        return;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);

    if (sizeof(Trip) * ind > size)
    {
        std::cout << "index was out of the bounds of array\n";
        fclose(file);
        return;
    }

    fseek(file, sizeof(Trip) * ind, SEEK_SET);
    fwrite(trip, sizeof(Trip), 1, file);
    fclose(file);
}
void delete_last_trip()
{
    std::size_t count = get_struct_num();
    if (count == 0)
        return;
    std::size_t new_size = sizeof(Trip) * (count - 1);
#ifdef _WIN32
    int fd = _open(DATA_FILENAME, _O_WRONLY | _O_BINARY);
    if (fd != -1)
    {
        _chsize(fd, new_size);
        _close(fd);
    }
#else
    truncate(DATA_FILENAME, new_size);
#endif
}
#pragma endregion

#pragma region SORTINGS
void insertion_sort()
{
    for (int i = 1; i < get_struct_num(); ++i)
    {
        for (int j = i; j > 0; --j)
        {
            Trip *t1 = get_trip(j);
            Trip *t2 = get_trip(j - 1);
            if (strcmp(t1->time_arrival, t2->time_arrival) >= 0)
            {
                delete t1;
                delete t2;
                break;
            }
            set_trip(j, t2);
            set_trip(j - 1, t1);
            delete t1;
            delete t2;
        }
    }
}
void selection_sort()
{
    for (int i = 0; i < get_struct_num() - 1; ++i)
    {
        Trip *min = get_trip(i);
        int l = i;
        for (int j = i + 1; j < get_struct_num(); ++j)
        {
            Trip *tmp = get_trip(j);
            if (strcmp(tmp->date_departure, min->date_departure) < 0)
            {
                l = j;
                delete min;
                min = tmp;
            }
            else
                delete tmp;
        }
        if (i != l)
        {
            Trip *cur = get_trip(i);
            set_trip(i, min);
            set_trip(l, cur);
            delete cur;
        }
        delete min;
    }
}
void quick_sort()
{
    if (get_struct_num() < 2)
        return;
    Stack st;
    StackData sd(0, get_struct_num() - 1);
    st.push(sd);
    while (st.top() != nullptr)
    {
        StackData qsd = st.pop();
        int l = qsd.l, r = qsd.r;
        int mid = (l + r) >> 1;
        Trip *m = get_trip(mid);
        char mid_dest[50];
        strcpy(mid_dest, m->destination);
        delete m;
        int i = l, j = r;
        while (i <= j)
        {
            while (true)
            {
                Trip *tmp = get_trip(i);
                if (strcmp(tmp->destination, mid_dest) >= 0)
                {
                    delete tmp;
                    break;
                }
                ++i;
                delete tmp;
            }
            while (true)
            {
                Trip *tmp = get_trip(j);
                if (strcmp(tmp->destination, mid_dest) <= 0)
                {
                    delete tmp;
                    break;
                }
                --j;
                delete tmp;
            }
            if (i <= j)
            {
                Trip *t1 = get_trip(i);
                Trip *t2 = get_trip(j);
                set_trip(i, t2);
                set_trip(j, t1);
                delete t1;
                delete t2;
                ++i;
                --j;
            }
        }
        if (l < j)
        {
            StackData sd(l, j);
            st.push(sd);
        }
        if (i < r)
        {
            StackData sd(i, r);
            st.push(sd);
        }
    }
}
void sort_by_time_arrival(const char *filename = DATA_FILENAME)
{
    Stack st;
    StackData sd(0, get_struct_num(filename) - 1);
    st.push(sd);
    while (st.top() != nullptr)
    {
        StackData qsd = st.pop();
        int l = qsd.l, r = qsd.r;
        int mid = (l + r) >> 1;
        Trip *m = get_trip(mid, filename);
        char mid_time[50];
        strcpy(mid_time, m->time_arrival);
        delete m;
        int i = l, j = r;
        while (i <= j)
        {
            while (true)
            {
                Trip *tmp = get_trip(i, filename);
                if (strcmp(tmp->time_arrival, mid_time) >= 0)
                {
                    delete tmp;
                    break;
                }
                ++i;
                delete tmp;
            }
            while (true)
            {
                Trip *tmp = get_trip(j, filename);
                if (strcmp(tmp->time_arrival, mid_time) <= 0)
                {
                    delete tmp;
                    break;
                }
                --j;
                delete tmp;
            }
            if (i <= j)
            {
                Trip *t1 = get_trip(i, filename);
                Trip *t2 = get_trip(j, filename);
                set_trip(i, t2, filename);
                set_trip(j, t1, filename);
                delete t1;
                delete t2;
                ++i;
                --j;
            }
        }
        if (l < j)
        {
            StackData sd(l, j);
            st.push(sd);
        }
        if (i < r)
        {
            StackData sd(i, r);
            st.push(sd);
        }
    }
}
#pragma endregion

#pragma region SEARCHES
Trip *find_by_race_number(int number)
{
    long size = get_struct_num();
    Trip *tmp = nullptr;
    for (long i = 0; i < size; ++i)
    {
        tmp = get_trip(i);
        if (tmp->number == number)
            return tmp;
    }
    return nullptr;
}

void find_by_destination(const char *destination)
{
    quick_sort();
    int l = 0;
    int r = get_struct_num() - 1;
    Trip *tmp = nullptr;
    int first = -1;
    while (l <= r)
    {
        int mid = (l + r) / 2;
        tmp = get_trip(mid);
        int res = strcmp(tmp->destination, destination);
        if (res == 0)
        {
            first = mid;
            r = mid - 1;
        }
        else if (res < 0)
            l = mid + 1;
        else
            r = mid - 1;
    }

    delete tmp;
    if (first == -1)
    {
        std::cout << "Не найдено подходящих записей\n";
        return;
    }
    tmp = get_trip(first++);
    while (strcmp(tmp->destination, destination) == 0)
    {
        print_trip(tmp);
        delete tmp;
        tmp = get_trip(first++);
    }
    delete tmp;
}
void print_proper_trips(const char *dest, const char *max_time, int min_ticks)
{
    remove(TMP_FILENAME);
    int j = 0;
    for (int i = 0; i < get_struct_num(); ++i)
    {
        Trip *tr = get_trip(i);
        if (strcmp(tr->destination, dest) == 0 && strcmp(tr->time_arrival, max_time) <= 0 && tr->tickets_left >= min_ticks)
            set_trip(j++, tr, TMP_FILENAME);
        delete tr;
    }
    if (j > 0)
    {
        sort_by_time_arrival(TMP_FILENAME);
        print_trips(TMP_FILENAME);
    }
    else
        std::cout << "\nНе найдено записей, удовлетворяющих условиям\n";
}
void print_by_bus_type(const char *bus_type, const char *min_departure_time)
{
    for (int i = 0; i < ROW_SIZE; ++i)
        std::cout << std::setfill(' ');
    std::cout << "\n";
    std::cout << "\n"
              << std::left << std::setw(COL_NUMBER) << "Trip number"
              << std::left << std::setw(COL_BUS_TYPE) << "Bus type"
              << std::left << std::setw(COL_DESTINATION) << "Destination"
              << std::left << std::setw(COL_DATE) << "Date departure"
              << std::left << std::setw(COL_TIME) << "Time departure"
              << std::left << std::setw(COL_TIME) << "Time arrival"
              << std::left << std::setw(COL_COST) << "Ticket cost"
              << std::left << std::setw(COL_LEFT) << "Tickets left"
              << std::left << std::setw(COL_SOLD) << "Tickets sold" << '\n';
    for (int i = 0; i < get_struct_num(); ++i)
    {
        Trip *trip = get_trip(i);
        if (strcmp(bus_type, trip->bus_type) == 0 && strcmp(trip->time_departure, min_departure_time) > 0)
        {
            std::cout << std::left << std::setw(COL_NUMBER) << trip->number
                      << std::left << std::setw(COL_BUS_TYPE) << trip->bus_type
                      << std::left << std::setw(COL_DESTINATION) << trip->destination
                      << std::left << std::setw(COL_DATE) << trip->date_departure
                      << std::left << std::setw(COL_TIME) << trip->time_departure
                      << std::left << std::setw(COL_TIME) << trip->time_arrival
                      << std::left << std::setw(COL_COST) << trip->ticket_cost
                      << std::left << std::setw(COL_LEFT) << trip->tickets_left
                      << std::left << std::setw(COL_SOLD) << trip->tickets_sold << '\n';
        }
        delete trip;
    }
}
#pragma endregion

#pragma region delete update
void delete_by_num(int number)
{
    int j = 0, i = 0;
    for (; i < get_struct_num(); ++i)
    {
        Trip *tr = get_trip(i);
        set_trip(j, tr);
        if (tr->number != number)
            ++j;
        delete tr;
    }
    if (i != j)
        delete_last_trip();
}
void update_by_num(int number, Trip *trip)
{
    for (int i = 0; i < get_struct_num(); ++i)
    {
        Trip *t = get_trip(i);
        if (t->number == number)
        {
            set_trip(i, trip);
            delete t;
            break;
        }
        delete t;
    }
}
#pragma endregion
void print_trips(const char *filename = DATA_FILENAME)
{
    std::cout << "\n";
    std::cout << "\n"
              << std::left << std::setw(COL_NUMBER) << "Trip number"
              << std::left << std::setw(COL_BUS_TYPE) << "Bus type"
              << std::left << std::setw(COL_DESTINATION) << "Destination"
              << std::left << std::setw(COL_DATE) << "Date departure"
              << std::left << std::setw(COL_TIME) << "Time departure"
              << std::left << std::setw(COL_TIME) << "Time arrival"
              << std::left << std::setw(COL_COST) << "Ticket cost"
              << std::left << std::setw(COL_LEFT) << "Tickets left"
              << std::left << std::setw(COL_SOLD) << "Tickets sold" << '\n';
    for (int i = 0; i < get_struct_num(filename); ++i)
    {
        Trip *trip = get_trip(i, filename);
        std::cout << std::left << std::setw(COL_NUMBER) << trip->number
                  << std::left << std::setw(COL_BUS_TYPE) << trip->bus_type
                  << std::left << std::setw(COL_DESTINATION) << trip->destination
                  << std::left << std::setw(COL_DATE) << trip->date_departure
                  << std::left << std::setw(COL_TIME) << trip->time_departure
                  << std::left << std::setw(COL_TIME) << trip->time_arrival
                  << std::left << std::setw(COL_COST) << trip->ticket_cost
                  << std::left << std::setw(COL_LEFT) << trip->tickets_left
                  << std::left << std::setw(COL_SOLD) << trip->tickets_sold << '\n';
        delete trip;
    }
    for (int i = 0; i < ROW_SIZE; ++i)
        std::cout << "-";
    std::cout << '\n';
}
void print_trip(Trip *trip)
{
    std::cout << "\n";
    std::cout << "\n"
              << std::left << std::setw(COL_NUMBER) << "Trip number"
              << std::left << std::setw(COL_BUS_TYPE) << "Bus type"
              << std::left << std::setw(COL_DESTINATION) << "Destination"
              << std::left << std::setw(COL_DATE) << "Date departure"
              << std::left << std::setw(COL_TIME) << "Time departure"
              << std::left << std::setw(COL_TIME) << "Time arrival"
              << std::left << std::setw(COL_COST) << "Ticket cost"
              << std::left << std::setw(COL_LEFT) << "Tickets left"
              << std::left << std::setw(COL_SOLD) << "Tickets sold" << '\n';
    std::cout << std::left << std::setw(COL_NUMBER) << trip->number
              << std::left << std::setw(COL_BUS_TYPE) << trip->bus_type
              << std::left << std::setw(COL_DESTINATION) << trip->destination
              << std::left << std::setw(COL_DATE) << trip->date_departure
              << std::left << std::setw(COL_TIME) << trip->time_departure
              << std::left << std::setw(COL_TIME) << trip->time_arrival
              << std::left << std::setw(COL_COST) << trip->ticket_cost
              << std::left << std::setw(COL_LEFT) << trip->tickets_left
              << std::left << std::setw(COL_SOLD) << trip->tickets_sold << '\n';
}

void initialize_data()
{
    if (get_struct_num() <= 25)
        remove(DATA_FILENAME);
    else
        return;

    Trip t0(101, "Mercedes", "Moscow", "2024-06-01", "08:00", "13:30", 1500.50f, 10, 40);
    set_trip(0, &t0);
    Trip t1(102, "Volvo", "Moscow", "2024-06-01", "09:30", "14:45", 1600.00f, 5, 45);
    set_trip(1, &t1);
    Trip t2(103, "Scania", "Moscow", "2024-06-02", "10:15", "15:20", 1700.00f, 20, 30);
    set_trip(2, &t2);
    Trip t3(104, "MAN", "Saint-Petersburg", "2024-06-02", "11:00", "16:30", 1800.00f, 15, 35);
    set_trip(3, &t3);
    Trip t4(105, "Setra", "Saint-Petersburg", "2024-06-03", "07:30", "12:15", 1900.00f, 8, 42);
    set_trip(4, &t4);
    Trip t5(106, "Mercedes", "Saint-Petersburg", "2024-06-03", "08:45", "14:00", 2000.00f, 12, 38);
    set_trip(5, &t5);
    Trip t6(107, "Volvo", "Kazan", "2024-06-04", "09:15", "17:30", 2100.00f, 18, 32);
    set_trip(6, &t6);
    Trip t7(108, "Scania", "Kazan", "2024-06-04", "10:30", "18:45", 2200.00f, 22, 28);
    set_trip(7, &t7);
    Trip t8(109, "MAN", "Nizhny-Novgorod", "2024-06-05", "11:45", "15:10", 2300.00f, 7, 43);
    set_trip(8, &t8);
    Trip t9(110, "Setra", "Nizhny-Novgorod", "2024-06-05", "12:30", "16:20", 2400.00f, 9, 41);
    set_trip(9, &t9);
    Trip t10(111, "Mercedes", "Rostov-on-Don", "2024-06-06", "13:15", "20:00", 2500.00f, 4, 46);
    set_trip(10, &t10);
    Trip t11(112, "Volvo", "Rostov-on-Don", "2024-06-06", "14:00", "21:15", 2600.00f, 6, 44);
    set_trip(11, &t11);
    Trip t12(113, "Scania", "Krasnodar", "2024-06-07", "08:30", "12:45", 2700.00f, 25, 25);
    set_trip(12, &t12);
    Trip t13(114, "MAN", "Krasnodar", "2024-06-07", "09:45", "14:30", 2800.00f, 3, 47);
    set_trip(13, &t13);
    Trip t14(115, "Setra", "Sochi", "2024-06-08", "10:00", "18:10", 2900.00f, 16, 34);
    set_trip(14, &t14);
    Trip t15(116, "Mercedes", "Sochi", "2024-06-08", "11:30", "19:45", 3000.00f, 11, 39);
    set_trip(15, &t15);
    Trip t16(117, "Volvo", "Voronezh", "2024-06-09", "12:45", "16:55", 3100.00f, 13, 37);
    set_trip(16, &t16);
    Trip t17(118, "Scania", "Voronezh", "2024-06-09", "13:30", "17:40", 3200.00f, 17, 33);
    set_trip(17, &t17);
    Trip t18(119, "MAN", "Samara", "2024-06-10", "14:15", "21:30", 3300.00f, 19, 31);
    set_trip(18, &t18);
    Trip t19(120, "Setra", "Samara", "2024-06-10", "15:00", "22:20", 3400.00f, 2, 48);
    set_trip(19, &t19);
    Trip t20(121, "Mercedes", "Ufa", "2024-06-11", "08:00", "15:30", 3500.00f, 14, 36);
    set_trip(20, &t20);
    Trip t21(122, "Volvo", "Yekaterinburg", "2024-06-11", "09:30", "18:00", 3600.00f, 21, 29);
    set_trip(21, &t21);
    Trip t22(123, "Scania", "Novosibirsk", "2024-06-12", "10:45", "20:30", 3700.00f, 23, 27);
    set_trip(22, &t22);
    Trip t23(124, "MAN", "Vladivostok", "2024-06-12", "11:15", "23:45", 5000.00f, 1, 49);
    set_trip(23, &t23);
    Trip t24(125, "Setra", "Chelyabinsk", "2024-06-13", "12:00", "19:15", 3800.00f, 24, 26);
    set_trip(24, &t24);
}

int main()
{
    initialize_data();
    std::cout << "*************************************************************************************************************************************\n\n";
    std::cout << "                              СИСТЕМА ОТСЛЕЖИВАНИЯ РЕАЛИЗАЦИИ БИЛЕТОВ НА АВТОБУСНЫЕ ПОЕЗДКИ\n\n";
    std::cout << "*************************************************************************************************************************************\n\n";
    bool is_app = true;
    while (is_app)
    {
        std::cout << "Выберите операцию:\n"
                  << "1 - Вывести все рейсы\n"
                  << "2 - Сортировать по пункту назначения\n"
                  << "3 - Сортировать по дате отправления\n"
                  << "4 - Сортировать по времени прибытия\n"
                  << "5 - Поиск рейса по номеру\n"
                  << "6 - Поиск по пункту назначения\n"
                  << "7 - Фильтр по времени прибытия в пункт и оставшимся билетам\n"
                  << "8 - Фильтр по типу автобуса и времени отправления\n"
                  << "9 - Добавить рейс\n"
                  << "10 - Удалить рейс\n"
                  << "11 - Редактировать рейс\n"
                  << "12 - Выйти из программы\n"
                  << "-> ";
        int choise = 0;
        std::cin >> choise;
        if (choise == 1)
            print_trips();
        else if (choise == 2)
            quick_sort();
        else if (choise == 3)
            selection_sort();
        else if (choise == 4)
            insertion_sort();
        else if (choise == 5)
        {
            std::cout << "Введите номер рейса:\n-> ";
            int race_number = 0;
            std::cin >> race_number;
            Trip *tr = find_by_race_number(race_number);
            if (tr)
            {
                print_trip(tr);
                delete tr;
            }
            else
            {
                std::cout << "Рейс не найден.\n";
            }
        }
        else if (choise == 6)
        {
            std::cout << "Введите пункт назначения:\n-> ";
            char dest[50];
            std::cin >> dest;
            find_by_destination(dest);
        }
        else if (choise == 7)
        {
            std::cout << "Введите пункт назначения:\n-> ";
            char dest[50];
            std::cin >> dest;
            std::cout << "Введите максимальное время прибытия\n-> ";
            char max_time[10];
            std::cin >> max_time;
            std::cout << "Введите минимальное число некупленных билетов (XX:XX)\n-> ";
            int min_ticks_left = 0;
            std::cin >> min_ticks_left;
            print_proper_trips(dest, max_time, min_ticks_left);
        }
        else if (choise == 8)
        {
            std::cout << "Введите тип автобусов\n-> ";
            char bus_type[50];
            std::cin >> bus_type;
            std::cout << "Введите минимальное время отправления (XX:XX)\n-> ";
            char min_time[10];
            std::cin >> min_time;
            print_by_bus_type(bus_type, min_time);
        }
        else if (choise == 9)
        {
            Trip newTrip;
            std::cout << "Введите номер рейса:\n-> ";
            std::cin >> newTrip.number;
            bool exists = false;
            for (int i = 0; i < get_struct_num(); ++i)
            {
                Trip *t = get_trip(i);
                if (t->number == newTrip.number)
                {
                    exists = true;
                    delete t;
                    break;
                }
                delete t;
            }
            if (exists)
            {
                std::cout << "Рейс с таким номером уже существует.\n";
            }
            else
            {
                std::cout << "Введите тип автобуса:\n-> ";
                std::cin >> newTrip.bus_type;
                std::cout << "Введите пункт назначения:\n-> ";
                std::cin >> newTrip.destination;
                std::cout << "Введите дату отправления (ГГГГ-ММ-ДД):\n-> ";
                std::cin >> newTrip.date_departure;
                std::cout << "Введите время отправления (ЧЧ:ММ):\n-> ";
                std::cin >> newTrip.time_departure;
                std::cout << "Введите время прибытия (ЧЧ:ММ):\n-> ";
                std::cin >> newTrip.time_arrival;
                std::cout << "Введите стоимость билета:\n-> ";
                std::cin >> newTrip.ticket_cost;
                std::cout << "Введите количество оставшихся билетов:\n-> ";
                std::cin >> newTrip.tickets_left;
                std::cout << "Введите количество проданных билетов:\n-> ";
                std::cin >> newTrip.tickets_sold;
                set_trip(get_struct_num(), &newTrip);
                std::cout << "Рейс добавлен.\n";
            }
        }
        else if (choise == 10)
        {
            std::cout << "Введите номер рейса для удаления:\n-> ";
            int number;
            std::cin >> number;
            delete_by_num(number);
        }
        else if (choise == 11)
        {
            std::cout << "Введите номер рейса для редактирования:\n-> ";
            int number;
            std::cin >> number;
            Trip updatedTrip;
            updatedTrip.number = number;
            std::cout << "Введите новый тип автобуса:\n-> ";
            std::cin >> updatedTrip.bus_type;
            std::cout << "Введите новый пункт назначения:\n-> ";
            std::cin >> updatedTrip.destination;
            std::cout << "Введите новую дату отправления (ГГГГ-ММ-ДД):\n-> ";
            std::cin >> updatedTrip.date_departure;
            std::cout << "Введите новое время отправления (ЧЧ:ММ):\n-> ";
            std::cin >> updatedTrip.time_departure;
            std::cout << "Введите новое время прибытия (ЧЧ:ММ):\n-> ";
            std::cin >> updatedTrip.time_arrival;
            std::cout << "Введите новую стоимость билета:\n-> ";
            std::cin >> updatedTrip.ticket_cost;
            std::cout << "Введите новое количество оставшихся билетов:\n-> ";
            std::cin >> updatedTrip.tickets_left;
            std::cout << "Введите новое количество проданных билетов:\n-> ";
            std::cin >> updatedTrip.tickets_sold;
            update_by_num(number, &updatedTrip);
            std::cout << "Рейс обновлён.\n";
        }
        else if (choise == 12)
            is_app = 0;
        else
            std::cout << "Неверный ввод\n";
        if (std::cin.fail())
        {
            std::cout << "Неверный ввод\n";
            return 1;
        }
        std::cout << "\n\n-------------------------------------------------------------------------------------------------------------------------------\n\n";
    }
    return 0;
}