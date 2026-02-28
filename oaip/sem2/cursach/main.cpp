#include <iostream>
#include <cstring>

const char *DATA_FILENAME = "data.bin";

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
struct QuicksortData
{
    int l;
    int r;
    QuicksortData(int left, int right)
    {
        l = left;
        r = right;
    }
};
struct StackData
{
    QuicksortData *qsd = nullptr;
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
    Stack()
    {
    }
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
    StackData top()
    {
        return root->data;
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
long get_struct_num()
{
    FILE *file = fopen(DATA_FILENAME, "rb");
    if (!file)
        return 0;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size / sizeof(Trip);
}
Trip *get_trip(int ind)
{
    FILE *file = fopen(DATA_FILENAME, "rb");
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

void set_trip(int ind, Trip *trip)
{
    FILE *file = fopen(DATA_FILENAME, "r+b");
    if (!file)
        file = fopen(DATA_FILENAME, "wb");
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
    Stack st;
    QuicksortData *q = new QuicksortData(0, get_struct_num() - 1);
    StackData sd;
    sd.qsd = q;
    st.push(sd);
    while (st.top().qsd != nullptr)
    {
        QuicksortData *qsd = st.pop().qsd;
        int l = qsd->l, r = qsd->r;
        delete qsd;
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
            QuicksortData *qsd = new QuicksortData(l, j);
            StackData sd;
            sd.qsd = qsd;
            st.push(sd);
        }
        if (i < r)
        {
            QuicksortData *qsd = new QuicksortData(i, r);
            StackData sd;
            sd.qsd = qsd;
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
    // TODO: check sorted
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
    return nullptr;
}
#pragma endregion

int main()
{
    Trip *trip = new Trip(1, "bus", "Moscow", "2024-01-01", "10:00", "18:00", 1000, 50, 10);
    set_trip(0, trip);
    delete trip;

    trip = get_trip(0);
    if (trip)
    {
        std::cout << "Bus type: " << trip->bus_type << std::endl;
        delete trip;
    }

    return 0;
}