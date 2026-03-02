#include <iostream>
#include <cstring>
#include <iomanip>
const char *DATA_FILENAME = "data.bin";
const char *TMP_FILENAME = "tmp.bin";

const int COL_WIDTH = 20;

void print_trips(const char *filename);

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

Trip *find_by_destination(const char *destination)
{
    quick_sort();
    int l = 0;
    int r = get_struct_num() - 1;
    Trip *tmp = nullptr;
    while (l <= r)
    {
        int mid = (l + r) / 2;
        tmp = get_trip(mid);
        int res = strcmp(tmp->destination, destination);
        if (res == 0)
            return tmp;
        else if (res < 0)
            l = mid + 1;
        else
            r = mid - 1;
    }
    delete tmp;
    return nullptr;
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
#pragma endregion

void print_trips(const char *filename = DATA_FILENAME)
{
    std::cout << std::setfill(' ') << std::setw(COL_WIDTH * 9) << "\n";
    std::cout << "\n"
              << std::left << std::setw(COL_WIDTH) << "Trip number"
              << std::left << std::setw(COL_WIDTH) << "Bus type"
              << std::left << std::setw(COL_WIDTH) << "Destination"
              << std::left << std::setw(COL_WIDTH) << "Date departure"
              << std::left << std::setw(COL_WIDTH) << "Time departure"
              << std::left << std::setw(COL_WIDTH) << "Time arrival"
              << std::left << std::setw(COL_WIDTH) << "Ticket cost"
              << std::left << std::setw(COL_WIDTH) << "Tickets left"
              << std::left << std::setw(COL_WIDTH) << "Tickets sold" << '\n';
    for (int i = 0; i < get_struct_num(filename); ++i)
    {
        Trip *trip = get_trip(i, filename);
        std::cout << std::left << std::setw(COL_WIDTH) << trip->number
                  << std::left << std::setw(COL_WIDTH) << trip->bus_type
                  << std::left << std::setw(COL_WIDTH) << trip->destination
                  << std::left << std::setw(COL_WIDTH) << trip->date_departure
                  << std::left << std::setw(COL_WIDTH) << trip->time_departure
                  << std::left << std::setw(COL_WIDTH) << trip->time_arrival
                  << std::left << std::setw(COL_WIDTH) << trip->ticket_cost
                  << std::left << std::setw(COL_WIDTH) << trip->tickets_left
                  << std::left << std::setw(COL_WIDTH) << trip->tickets_sold << '\n';
        delete trip;
    }
    std::cout << std::setfill('-') << std::setw(COL_WIDTH * 9) << "\n";
}

int main()
{
    Trip *trip1 = new Trip(1, "bus", "Moscow", "2026-01-01", "10:00", "18:00", 1000, 50, 10);
    Trip *trip2 = new Trip(2, "bus", "Saint Petersburg", "2026-01-02", "18:00", "21:00", 1500, 20, 7);
    Trip *trip3 = new Trip(3, "bus", "Minsk", "2026-02-01", "19:00", "20:00", 100, 27, 3);
    Trip *trip4 = new Trip(56, "bus", "Moscow", "2026-01-01", "11:00", "23:00", 1000, 50, 10);
    set_trip(0, trip1);
    set_trip(1, trip2);
    set_trip(2, trip3);
    set_trip(3, trip4);
    delete trip1;
    delete trip2;
    delete trip3;
    delete trip4;
    print_trips();
    print_proper_trips("Moscow", "23:00", 7);
    return 0;
}