#include <iostream>
#include <cstring>
#include <iomanip>
#include <filesystem>
#include <cstddef>
#include "constants.hpp"
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

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
        strncpy(this->bus_type, bus_type, 49);
        this->bus_type[49] = '\0';
        strncpy(this->destination, destination, 49);
        this->destination[49] = '\0';
        strncpy(this->date_departure, date_departure, 19);
        this->date_departure[19] = '\0';
        strncpy(this->time_departure, time_departure, 9);
        this->time_departure[9] = '\0';
        strncpy(this->time_arrival, time_arrival, 9);
        this->time_arrival[9] = '\0';
    }
};

void print_trip(Trip trip);
void print_trips(FILE *file);
void quick_sort_silent(FILE *file);

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

#pragma region FILES_WORKING
bool init_file(const char *filename, FILE *&file)
{
    if (file != nullptr)
    {
        fclose(file);
        file = nullptr;
    }
    file = fopen(filename, "r+b");
    if (!file)
    {
        file = fopen(filename, "wb");
        if (file == nullptr)
            return false;
        fclose(file);
        file = fopen(filename, "r+b");
    }
    return file != nullptr;
}
long get_struct_num(FILE *file)
{
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    return size / sizeof(Trip);
}
Trip get_trip(int ind, FILE *file, bool *success = nullptr)
{
    if (success)
        *success = true;
    long size = get_struct_num(file);
    Trip tr;
    if (ind < 0 || ind >= size)
    {
        if (success)
            *success = false;
        return tr;
    }

    fseek(file, sizeof(Trip) * ind, SEEK_SET);
    fread(&tr, sizeof(Trip), 1, file);
    return tr;
}

void set_trip(int ind, Trip *trip, FILE *file)
{
    long count = get_struct_num(file);
    if (ind < 0 || ind > count)
    {
        std::cout << "index was out of the bounds of array\n";
        return;
    }

    fseek(file, sizeof(Trip) * ind, SEEK_SET);
    fwrite(trip, sizeof(Trip), 1, file);
    fflush(file);
}
void delete_last_trip(FILE *file, const char *filename)
{
    std::size_t count = get_struct_num(file);
    if (count == 0)
        return;
    std::size_t new_size = sizeof(Trip) * (count - 1);
#ifdef _WIN32
    int fd = _open(filename, _O_WRONLY | _O_BINARY);
    if (fd != -1)
    {
        _chsize(fd, new_size);
        _close(fd);
    }
#else
    truncate(filename, new_size);
#endif
}

bool clear_file(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (f == nullptr)
        return 0;
    fclose(f);
    return 1;
}
#pragma endregion

#pragma region SORTINGS
void insertion_sort(FILE *file)
{
    int n = get_struct_num(file);
    if (n < 2)
        return;
    for (int i = 1; i < n; ++i)
    {
        Trip key = get_trip(i, file);
        int j = i - 1;
        while (j >= 0)
        {
            Trip t = get_trip(j, file);
            if (strcmp(t.time_arrival, key.time_arrival) <= 0)
                break;
            set_trip(j + 1, &t, file);
            --j;
        }
        if (j + 1 != i)
            set_trip(j + 1, &key, file);
    }
    print_trips(file);
}
void selection_sort(FILE *file)
{
    int n = get_struct_num(file);
    if (n < 2)
    {
        print_trips(file);
        return;
    }
    for (int i = 0; i < n - 1; ++i)
    {
        int l = i;
        Trip min = get_trip(i, file);
        for (int j = i + 1; j < n; ++j)
        {
            Trip tmp = get_trip(j, file);
            if (strcmp(tmp.date_departure, min.date_departure) < 0)
            {
                l = j;
                min = tmp;
            }
        }
        if (i != l)
        {
            Trip cur = get_trip(i, file);
            set_trip(i, &min, file);
            set_trip(l, &cur, file);
        }
    }
    print_trips(file);
}
void quick_sort(FILE *file)
{
    int n = get_struct_num(file);
    if (n < 2)
        return;
    Stack st;
    StackData sd(0, n - 1);
    st.push(sd);
    while (st.top() != nullptr)
    {
        StackData qsd = st.pop();
        int l = qsd.l, r = qsd.r;
        int mid = (l + r) >> 1;
        Trip m = get_trip(mid, file);
        char mid_dest[50];
        strncpy(mid_dest, m.destination, 49);
        mid_dest[49] = '\0';
        int i = l, j = r;
        while (i <= j)
        {
            while (i <= r)
            {
                Trip tmp = get_trip(i, file);
                if (strcmp(tmp.destination, mid_dest) >= 0)
                    break;
                ++i;
            }
            while (j >= l)
            {
                Trip tmp = get_trip(j, file);
                if (strcmp(tmp.destination, mid_dest) <= 0)
                    break;
                --j;
            }
            if (i <= j)
            {
                Trip t1 = get_trip(i, file);
                Trip t2 = get_trip(j, file);
                set_trip(i, &t2, file);
                set_trip(j, &t1, file);
                ++i;
                --j;
            }
        }
        if (l < j)
        {
            StackData sd2(l, j);
            st.push(sd2);
        }
        if (i < r)
        {
            StackData sd2(i, r);
            st.push(sd2);
        }
    }
    print_trips(file);
}
void sort_by_time_arrival(FILE *file)
{
    int n = get_struct_num(file);
    if (n < 2)
        return;
    Stack st;
    StackData sd(0, n - 1);
    st.push(sd);
    while (st.top() != nullptr)
    {
        StackData qsd = st.pop();
        int l = qsd.l, r = qsd.r;
        int mid = (l + r) >> 1;
        Trip m = get_trip(mid, file);
        char mid_time[50];
        strncpy(mid_time, m.time_arrival, 49);
        mid_time[49] = '\0';
        int i = l, j = r;
        while (i <= j)
        {
            while (i <= r)
            {
                Trip tmp = get_trip(i, file);
                if (strcmp(tmp.time_arrival, mid_time) >= 0)
                    break;
                ++i;
            }
            while (j >= l)
            {
                Trip tmp = get_trip(j, file);
                if (strcmp(tmp.time_arrival, mid_time) <= 0)
                    break;
                --j;
            }
            if (i <= j)
            {
                Trip t1 = get_trip(i, file);
                Trip t2 = get_trip(j, file);
                set_trip(i, &t2, file);
                set_trip(j, &t1, file);
                ++i;
                --j;
            }
        }
        if (l < j)
        {
            StackData sd2(l, j);
            st.push(sd2);
        }
        if (i < r)
        {
            StackData sd2(i, r);
            st.push(sd2);
        }
    }
}
#pragma endregion

#pragma region SEARCHES
Trip find_by_race_number(int number, FILE *file, bool *success = nullptr)
{
    if (success)
        *success = false;
    long size = get_struct_num(file);
    Trip tmp;
    for (long i = 0; i < size; ++i)
    {
        tmp = get_trip(i, file);
        if (tmp.number == number)
        {
            if (success)
                *success = true;
            return tmp;
        }
    }
    return tmp;
}

void find_by_destination(const char *destination, FILE *file)
{
    int n = get_struct_num(file);
    if (n == 0)
    {
        std::cout << "No matching records found\n";
        return;
    }
    quick_sort_silent(file);
    int l = 0;
    int r = get_struct_num(file) - 1;
    Trip tmp;
    int first = -1;
    while (l <= r)
    {
        int mid = (l + r) / 2;
        tmp = get_trip(mid, file);
        int res = strcmp(tmp.destination, destination);
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
    if (first == -1)
    {
        std::cout << "No matching records found\n";
        return;
    }
    int idx = first;
    bool found = false;

    FILE *report = fopen("report_destination.txt", "w");
    if (report)
    {
        fprintf(report, "\n");
        fprintf(report, "%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s\n",
                COL_NUMBER, "Trip number",
                COL_BUS_TYPE, "Bus type",
                COL_DESTINATION, "Destination",
                COL_DATE, "Date departure",
                COL_TIME, "Time departure",
                COL_TIME, "Time arrival",
                COL_COST, "Ticket cost",
                COL_LEFT, "Tickets left",
                COL_SOLD, "Tickets sold");
    }

    while (idx < n)
    {
        tmp = get_trip(idx, file);
        if (strcmp(tmp.destination, destination) != 0)
            break;
        print_trip(tmp);
        if (report)
        {
            fprintf(report, "%-*d%-*s%-*s%-*s%-*s%-*s%-*.*f%-*d%-*d\n",
                    COL_NUMBER, tmp.number,
                    COL_BUS_TYPE, tmp.bus_type,
                    COL_DESTINATION, tmp.destination,
                    COL_DATE, tmp.date_departure,
                    COL_TIME, tmp.time_departure,
                    COL_TIME, tmp.time_arrival,
                    COL_COST, 2, tmp.ticket_cost,
                    COL_LEFT, tmp.tickets_left,
                    COL_SOLD, tmp.tickets_sold);
        }
        found = true;
        ++idx;
    }

    if (report)
    {
        for (int i = 0; i < ROW_SIZE; ++i)
            fputc('-', report);
        fputc('\n', report);
        fclose(report);
    }

    if (!found)
        std::cout << "No matching records found\n";
}
void print_proper_trips(const char *dest, const char *max_time, int min_ticks, FILE *main_file)
{
    int n = get_struct_num(main_file);
    if (n == 0)
    {
        std::cout << "\nNo records found matching the criteria\n";
        return;
    }
    Trip *buffer = new Trip[n];
    int j = 0;
    for (int i = 0; i < n; ++i)
    {
        Trip tr = get_trip(i, main_file);
        if (strcmp(tr.destination, dest) == 0 && strcmp(tr.time_arrival, max_time) <= 0 && tr.tickets_left >= min_ticks)
            buffer[j++] = tr;
    }
    if (j > 0)
    {
        FILE *tmp_file = fopen(TMP_FILENAME, "wb");
        for (int k = 0; k < j; ++k)
            fwrite(&buffer[k], sizeof(Trip), 1, tmp_file);
        fclose(tmp_file);
        tmp_file = fopen(TMP_FILENAME, "r+b");
        sort_by_time_arrival(tmp_file);

        FILE *report = fopen("report_filter.txt", "w");
        if (report)
        {
            fprintf(report, "\n");
            fprintf(report, "%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s\n",
                    COL_NUMBER, "Trip number",
                    COL_BUS_TYPE, "Bus type",
                    COL_DESTINATION, "Destination",
                    COL_DATE, "Date departure",
                    COL_TIME, "Time departure",
                    COL_TIME, "Time arrival",
                    COL_COST, "Ticket cost",
                    COL_LEFT, "Tickets left",
                    COL_SOLD, "Tickets sold");
        }

        for (int i = 0; i < get_struct_num(tmp_file); ++i)
        {
            Trip trip = get_trip(i, tmp_file);
            std::cout << std::left << std::setw(COL_NUMBER) << trip.number
                      << std::left << std::setw(COL_BUS_TYPE) << trip.bus_type
                      << std::left << std::setw(COL_DESTINATION) << trip.destination
                      << std::left << std::setw(COL_DATE) << trip.date_departure
                      << std::left << std::setw(COL_TIME) << trip.time_departure
                      << std::left << std::setw(COL_TIME) << trip.time_arrival
                      << std::left << std::setw(COL_COST) << trip.ticket_cost
                      << std::left << std::setw(COL_LEFT) << trip.tickets_left
                      << std::left << std::setw(COL_SOLD) << trip.tickets_sold << '\n';
            if (report)
            {
                fprintf(report, "%-*d%-*s%-*s%-*s%-*s%-*s%-*.*f%-*d%-*d\n",
                        COL_NUMBER, trip.number,
                        COL_BUS_TYPE, trip.bus_type,
                        COL_DESTINATION, trip.destination,
                        COL_DATE, trip.date_departure,
                        COL_TIME, trip.time_departure,
                        COL_TIME, trip.time_arrival,
                        COL_COST, 2, trip.ticket_cost,
                        COL_LEFT, trip.tickets_left,
                        COL_SOLD, trip.tickets_sold);
            }
        }
        for (int i = 0; i < ROW_SIZE; ++i)
            std::cout << "-";
        std::cout << '\n';

        if (report)
        {
            for (int i = 0; i < ROW_SIZE; ++i)
                fputc('-', report);
            fputc('\n', report);
            fclose(report);
        }

        fclose(tmp_file);
        remove(TMP_FILENAME);
    }
    else
        std::cout << "\nNo records found matching the criteria\n";
    delete[] buffer;
}
void print_by_bus_type(const char *bus_type, const char *min_departure_time, FILE *file)
{
    bool found = false;
    FILE *report = nullptr;
    for (int i = 0; i < get_struct_num(file); ++i)
    {
        Trip trip = get_trip(i, file);
        if (strcmp(bus_type, trip.bus_type) == 0 && strcmp(trip.time_departure, min_departure_time) > 0)
        {
            if (!found)
            {
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
                report = fopen("report_bus_type.txt", "w");
                if (report)
                {
                    fprintf(report, "\n");
                    fprintf(report, "%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s\n",
                            COL_NUMBER, "Trip number",
                            COL_BUS_TYPE, "Bus type",
                            COL_DESTINATION, "Destination",
                            COL_DATE, "Date departure",
                            COL_TIME, "Time departure",
                            COL_TIME, "Time arrival",
                            COL_COST, "Ticket cost",
                            COL_LEFT, "Tickets left",
                            COL_SOLD, "Tickets sold");
                }
            }
            std::cout << std::left << std::setw(COL_NUMBER) << trip.number
                      << std::left << std::setw(COL_BUS_TYPE) << trip.bus_type
                      << std::left << std::setw(COL_DESTINATION) << trip.destination
                      << std::left << std::setw(COL_DATE) << trip.date_departure
                      << std::left << std::setw(COL_TIME) << trip.time_departure
                      << std::left << std::setw(COL_TIME) << trip.time_arrival
                      << std::left << std::setw(COL_COST) << trip.ticket_cost
                      << std::left << std::setw(COL_LEFT) << trip.tickets_left
                      << std::left << std::setw(COL_SOLD) << trip.tickets_sold << '\n';
            if (report)
            {
                fprintf(report, "%-*d%-*s%-*s%-*s%-*s%-*s%-*.*f%-*d%-*d\n",
                        COL_NUMBER, trip.number,
                        COL_BUS_TYPE, trip.bus_type,
                        COL_DESTINATION, trip.destination,
                        COL_DATE, trip.date_departure,
                        COL_TIME, trip.time_departure,
                        COL_TIME, trip.time_arrival,
                        COL_COST, 2, trip.ticket_cost,
                        COL_LEFT, trip.tickets_left,
                        COL_SOLD, trip.tickets_sold);
            }
            found = true;
        }
    }
    if (report)
    {
        for (int i = 0; i < ROW_SIZE; ++i)
            fputc('-', report);
        fputc('\n', report);
        fclose(report);
    }
    if (!found)
        std::cout << "\nNo records found matching the criteria\n";
}
#pragma endregion

#pragma region delete_update
void delete_by_num(int number, FILE *file, const char *filename)
{
    bool found = false;
    int j = 0;
    int n = get_struct_num(file);
    for (int i = 0; i < n; ++i)
    {
        Trip tr = get_trip(i, file);
        if (tr.number == number)
        {
            found = true;
            continue;
        }
        set_trip(j, &tr, file);
        ++j;
    }
    if (found)
        delete_last_trip(file, filename);
}
void update_by_num(int number, Trip trip, FILE *file)
{
    bool found = false;
    for (int i = 0; i < get_struct_num(file); ++i)
    {
        Trip t = get_trip(i, file);
        if (t.number == number)
        {
            set_trip(i, &trip, file);
            found = true;
            break;
        }
    }
    if (!found)
        std::cout << "\n[NOT FOUND] Рейс не найден\n";
}
#pragma endregion

void quick_sort_silent(FILE *file)
{
    int n = get_struct_num(file);
    if (n < 2)
        return;
    Stack st;
    StackData sd(0, n - 1);
    st.push(sd);
    while (st.top() != nullptr)
    {
        StackData qsd = st.pop();
        int l = qsd.l, r = qsd.r;
        int mid = (l + r) >> 1;
        Trip m = get_trip(mid, file);
        char mid_dest[50];
        strncpy(mid_dest, m.destination, 49);
        mid_dest[49] = '\0';
        int i = l, j = r;
        while (i <= j)
        {
            while (i <= r)
            {
                Trip tmp = get_trip(i, file);
                if (strcmp(tmp.destination, mid_dest) >= 0)
                    break;
                ++i;
            }
            while (j >= l)
            {
                Trip tmp = get_trip(j, file);
                if (strcmp(tmp.destination, mid_dest) <= 0)
                    break;
                --j;
            }
            if (i <= j)
            {
                Trip t1 = get_trip(i, file);
                Trip t2 = get_trip(j, file);
                set_trip(i, &t2, file);
                set_trip(j, &t1, file);
                ++i;
                --j;
            }
        }
        if (l < j)
        {
            StackData sd2(l, j);
            st.push(sd2);
        }
        if (i < r)
        {
            StackData sd2(i, r);
            st.push(sd2);
        }
    }
}

void print_trips(FILE *file)
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
    for (int i = 0; i < get_struct_num(file); ++i)
    {
        Trip trip = get_trip(i, file);
        std::cout << std::left << std::setw(COL_NUMBER) << trip.number
                  << std::left << std::setw(COL_BUS_TYPE) << trip.bus_type
                  << std::left << std::setw(COL_DESTINATION) << trip.destination
                  << std::left << std::setw(COL_DATE) << trip.date_departure
                  << std::left << std::setw(COL_TIME) << trip.time_departure
                  << std::left << std::setw(COL_TIME) << trip.time_arrival
                  << std::left << std::setw(COL_COST) << trip.ticket_cost
                  << std::left << std::setw(COL_LEFT) << trip.tickets_left
                  << std::left << std::setw(COL_SOLD) << trip.tickets_sold << '\n';
    }
    for (int i = 0; i < ROW_SIZE; ++i)
        std::cout << "-";
    std::cout << '\n';
}

void print_trip(Trip trip)
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
    std::cout << std::left << std::setw(COL_NUMBER) << trip.number
              << std::left << std::setw(COL_BUS_TYPE) << trip.bus_type
              << std::left << std::setw(COL_DESTINATION) << trip.destination
              << std::left << std::setw(COL_DATE) << trip.date_departure
              << std::left << std::setw(COL_TIME) << trip.time_departure
              << std::left << std::setw(COL_TIME) << trip.time_arrival
              << std::left << std::setw(COL_COST) << trip.ticket_cost
              << std::left << std::setw(COL_LEFT) << trip.tickets_left
              << std::left << std::setw(COL_SOLD) << trip.tickets_sold << '\n';
}

void initialize_data(FILE *file)
{
    Trip t0(101, "Mercedes", "Moscow", "2024-06-01", "08:00", "13:30", 1500.50f, 10, 40);
    set_trip(0, &t0, file);
    Trip t1(102, "Volvo", "Moscow", "2024-06-01", "09:30", "14:45", 1600.00f, 5, 45);
    set_trip(1, &t1, file);
    Trip t2(103, "Scania", "Moscow", "2024-06-02", "10:15", "15:20", 1700.00f, 20, 30);
    set_trip(2, &t2, file);
    Trip t3(104, "MAN", "Saint-Petersburg", "2024-06-02", "11:00", "16:30", 1800.00f, 15, 35);
    set_trip(3, &t3, file);
    Trip t4(105, "Setra", "Saint-Petersburg", "2024-06-03", "07:30", "12:15", 1900.00f, 8, 42);
    set_trip(4, &t4, file);
    Trip t5(106, "Mercedes", "Saint-Petersburg", "2024-06-03", "08:45", "14:00", 2000.00f, 12, 38);
    set_trip(5, &t5, file);
    Trip t6(107, "Volvo", "Kazan", "2024-06-04", "09:15", "17:30", 2100.00f, 18, 32);
    set_trip(6, &t6, file);
    Trip t7(108, "Scania", "Kazan", "2024-06-04", "10:30", "18:45", 2200.00f, 22, 28);
    set_trip(7, &t7, file);
    Trip t8(109, "MAN", "Nizhny-Novgorod", "2024-06-05", "11:45", "15:10", 2300.00f, 7, 43);
    set_trip(8, &t8, file);
    Trip t9(110, "Setra", "Nizhny-Novgorod", "2024-06-05", "12:30", "16:20", 2400.00f, 9, 41);
    set_trip(9, &t9, file);
    Trip t10(111, "Mercedes", "Rostov-on-Don", "2024-06-06", "13:15", "20:00", 2500.00f, 4, 46);
    set_trip(10, &t10, file);
    Trip t11(112, "Volvo", "Rostov-on-Don", "2024-06-06", "14:00", "21:15", 2600.00f, 6, 44);
    set_trip(11, &t11, file);
    Trip t12(113, "Scania", "Krasnodar", "2024-06-07", "08:30", "12:45", 2700.00f, 25, 25);
    set_trip(12, &t12, file);
    Trip t13(114, "MAN", "Krasnodar", "2024-06-07", "09:45", "14:30", 2800.00f, 3, 47);
    set_trip(13, &t13, file);
    Trip t14(115, "Setra", "Sochi", "2024-06-08", "10:00", "18:10", 2900.00f, 16, 34);
    set_trip(14, &t14, file);
    Trip t15(116, "Mercedes", "Sochi", "2024-06-08", "11:30", "19:45", 3000.00f, 11, 39);
    set_trip(15, &t15, file);
    Trip t16(117, "Volvo", "Voronezh", "2024-06-09", "12:45", "16:55", 3100.00f, 13, 37);
    set_trip(16, &t16, file);
    Trip t17(118, "Scania", "Voronezh", "2024-06-09", "13:30", "17:40", 3200.00f, 17, 33);
    set_trip(17, &t17, file);
    Trip t18(119, "MAN", "Samara", "2024-06-10", "14:15", "21:30", 3300.00f, 19, 31);
    set_trip(18, &t18, file);
    Trip t19(120, "Setra", "Samara", "2024-06-10", "15:00", "22:20", 3400.00f, 2, 48);
    set_trip(19, &t19, file);
    Trip t20(121, "Mercedes", "Ufa", "2024-06-11", "08:00", "15:30", 3500.00f, 14, 36);
    set_trip(20, &t20, file);
    Trip t21(122, "Volvo", "Yekaterinburg", "2024-06-11", "09:30", "18:00", 3600.00f, 21, 29);
    set_trip(21, &t21, file);
    Trip t22(123, "Scania", "Novosibirsk", "2024-06-12", "10:45", "20:30", 3700.00f, 23, 27);
    set_trip(22, &t22, file);
    Trip t23(124, "MAN", "Vladivostok", "2024-06-12", "11:15", "23:45", 5000.00f, 1, 49);
    set_trip(23, &t23, file);
    Trip t24(125, "Setra", "Chelyabinsk", "2024-06-13", "12:00", "19:15", 3800.00f, 24, 26);
    set_trip(24, &t24, file);
}

int main()
{
    std::cout << "*************************************************************************************************************************************\n\n";
    std::cout << "                              СИСТЕМА ОТСЛЕЖИВАНИЯ РЕАЛИЗАЦИИ БИЛЕТОВ НА АВТОБУСНЫЕ ПОЕЗДКИ\n\n";
    std::cout << "*************************************************************************************************************************************\n\n";

    char current_file[100] = "";
    bool file_selected = false;
    FILE *file = nullptr;

    bool running = true;
    while (running)
    {
        while (!file_selected)
        {
            std::cout << "\n+-------------------------------------+\n";
            std::cout << "|       МЕНЮ ВЫБОРА ФАЙЛА             |\n";
            std::cout << "+-------------------------------------+\n";
            std::cout << "  1 -> Создать новый файл\n";
            std::cout << "  2 -> Удалить файл\n";
            std::cout << "  3 -> Очистить файл\n";
            std::cout << "  4 -> Задать файл по умолчанию\n";
            std::cout << "  5 -> Открыть файл\n";
            std::cout << "  6 -> Выйти из программы\n";
            std::cout << "\n-> Выбор: ";
            int choice = 0;
            std::cin >> choice;
            if (std::cin.fail())
            {
                std::cout << "\n[ERROR] Неверный ввод\n";
                std::cin.clear();
                std::cin.ignore(9999, '\n');
                continue;
            }

            switch (choice)
            {
            case 1:
            {
                std::cout << "\n-> Введите имя нового файла: ";
                std::cin >> std::setw(99) >> current_file;
                if (init_file(current_file, file))
                {
                    initialize_data(file);
                    std::cout << "\n[OK] Файл создан успешно\n";
                    file_selected = true;
                }
                else
                    std::cout << "\n[ERROR] Не удалось создать файл\n";
                break;
            }
            case 2:
            {
                std::cout << "\n-> Введите имя файла для удаления: ";
                std::cin >> std::setw(99) >> current_file;
                if (remove(current_file) == 0)
                    std::cout << "\n[OK] Файл удалён успешно\n";
                else
                    std::cout << "\n[ERROR] Не удалось удалить файл\n";
                break;
            }
            case 3:
            {
                std::cout << "\n-> Введите имя файла для очистки: ";
                std::cin >> std::setw(99) >> current_file;
                if (clear_file(current_file))
                    std::cout << "\n[OK] Файл очищен успешно\n";
                else
                    std::cout << "\n[ERROR] Не удалось очистить файл\n";
                break;
            }
            case 4:
            {
                std::strcpy(current_file, "data.bin");
                std::cout << "\n[OK] Файл по умолчанию установлен: " << current_file << "\n";
                if (init_file(current_file, file))
                {
                    if (get_struct_num(file) == 0)
                        initialize_data(file);
                    file_selected = true;
                }
                break;
            }
            case 5:
            {
                std::cout << "\n-> Введите имя файла для открытия: ";
                std::cin >> std::setw(99) >> current_file;
                if (init_file(current_file, file))
                {
                    std::cout << "\n[OK] Файл открыт: " << current_file << "\n";
                    file_selected = true;
                }
                else
                    std::cout << "\n[ERROR] Не удалось открыть файл\n";
                break;
            }
            case 6:
                fclose(file);
                running = false;
                file_selected = true;
                break;
            default:
                std::cout << "\n[ERROR] Неверный ввод\n";
            }
        }

        bool working_with_file = true;
        while (working_with_file)
        {
            std::cout << "\n+---------------------------------------------------------+\n";
            std::cout << "|  РАБОТА С ФАЙЛОМ: " << std::left << std::setw(38) << current_file << " |\n";
            std::cout << "+---------------------------------------------------------+\n";
            std::cout << "\n  1 -> Вывести все рейсы\n";
            std::cout << "  2 -> Сортировать по пункту назначения\n";
            std::cout << "  3 -> Сортировать по дате отправления\n";
            std::cout << "  4 -> Сортировать по времени прибытия\n";
            std::cout << "  5 -> Поиск рейса по номеру\n";
            std::cout << "  6 -> Поиск по пункту назначения\n";
            std::cout << "  7 -> Фильтр по времени прибытия и билетам\n";
            std::cout << "  8 -> Фильтр по типу автобуса и времени\n";
            std::cout << "  9 -> Добавить рейс\n";
            std::cout << "  10 -> Удалить рейс\n";
            std::cout << "  11 -> Редактировать рейс\n";
            std::cout << "  12 -> Вернуться в меню выбора файла\n";
            std::cout << "  13 -> Выйти из программы\n";
            std::cout << "\n-> Выбор: ";
            int choise = 0;
            std::cin >> choise;
            if (std::cin.fail())
            {
                std::cout << "\n[ERROR] Неверный ввод\n";
                std::cin.clear();
                std::cin.ignore(9999, '\n');
                continue;
            }

            if (choise == 1)
                print_trips(file);
            else if (choise == 2)
                quick_sort(file);
            else if (choise == 3)
                selection_sort(file);
            else if (choise == 4)
                insertion_sort(file);
            else if (choise == 5)
            {
                std::cout << "\n-> Vvedite nomer reysa: ";
                int race_number = 0;
                std::cin >> race_number;
                if (std::cin.fail())
                {
                    std::cout << "\n[ERROR] Неверный ввод\n";
                    continue;
                }
                bool success = false;
                Trip tr = find_by_race_number(race_number, file, &success);
                if (success)
                    print_trip(tr);
                else
                    std::cout << "\n[NOT FOUND] Рейс не найден\n";
            }
            else if (choise == 6)
            {
                std::cout << "\n-> Введите пункт назначения: ";
                char dest[50];
                std::cin >> std::setw(49) >> dest;
                find_by_destination(dest, file);
            }
            else if (choise == 7)
            {
                std::cout << "\n-> Введите пункт назначения: ";
                char dest[50];
                std::cin >> std::setw(49) >> dest;
                std::cout << "\n-> Введите максимальное время прибытия (ЧЧ:ММ): ";
                char max_time[10];
                std::cin >> std::setw(9) >> max_time;
                std::cout << "\n-> Введите минимальное число некупленных билетов: ";
                int min_ticks_left = 0;
                std::cin >> min_ticks_left;
                if (std::cin.fail())
                {
                    std::cout << "\n[ERROR] Неверный ввод\n";
                    continue;
                }
                print_proper_trips(dest, max_time, min_ticks_left, file);
            }
            else if (choise == 8)
            {
                std::cout << "\n-> Введите тип автобусов: ";
                char bus_type[50];
                std::cin >> std::setw(49) >> bus_type;
                std::cout << "\n-> Введите минимальное время отправления (ЧЧ:ММ): ";
                char min_time[10];
                std::cin >> std::setw(9) >> min_time;
                print_by_bus_type(bus_type, min_time, file);
            }
            else if (choise == 9)
            {
                Trip newTrip;
                std::cout << "\n-> Введите номер рейса: ";
                std::cin >> newTrip.number;
                if (std::cin.fail())
                {
                    std::cout << "\n[ERROR] Неверный ввод\n";
                    continue;
                }
                bool exists = false;
                for (int i = 0; i < get_struct_num(file); ++i)
                {
                    Trip t = get_trip(i, file);
                    if (t.number == newTrip.number)
                    {
                        exists = true;
                        break;
                    }
                }
                if (exists)
                {
                    std::cout << "\n[ERROR] Рейс с таким номером уже существует\n";
                }
                else
                {
                    std::cout << "\n-> Введите тип автобуса: ";
                    std::cin >> std::setw(49) >> newTrip.bus_type;
                    std::cout << "\n-> Введите пункт назначения: ";
                    std::cin >> std::setw(49) >> newTrip.destination;
                    std::cout << "\n-> Введите дату отправления (ГГГГ-ММ-ДД): ";
                    std::cin >> std::setw(19) >> newTrip.date_departure;
                    std::cout << "\n-> Введите время отправления (ЧЧ:ММ): ";
                    std::cin >> std::setw(9) >> newTrip.time_departure;
                    std::cout << "\n-> Введите время прибытия (ЧЧ:ММ): ";
                    std::cin >> std::setw(9) >> newTrip.time_arrival;
                    std::cout << "\n-> Введите стоимость билета: ";
                    std::cin >> newTrip.ticket_cost;
                    std::cout << "\n-> Введите количество оставшихся билетов: ";
                    std::cin >> newTrip.tickets_left;
                    std::cout << "\n-> Введите количество проданных билетов: ";
                    std::cin >> newTrip.tickets_sold;
                    if (std::cin.fail())
                    {
                        std::cout << "\n[ERROR] Неверный ввод\n";
                        continue;
                    }
                    set_trip(get_struct_num(file), &newTrip, file);
                    std::cout << "\n[OK] Рейс добавлен\n";
                }
            }
            else if (choise == 10)
            {
                std::cout << "\n-> Введите номер рейса для удаления: ";
                int number;
                std::cin >> number;
                if (std::cin.fail())
                {
                    std::cout << "\n[ERROR] Неверный ввод\n";
                    continue;
                }
                delete_by_num(number, file, current_file);
            }
            else if (choise == 11)
            {
                std::cout << "\n-> Введите номер рейса для редактирования: ";
                int number;
                std::cin >> number;
                if (std::cin.fail())
                {
                    std::cout << "\n[ERROR] Неверный ввод\n";
                    continue;
                }
                bool found = false;
                for (int i = 0; i < get_struct_num(file); ++i)
                {
                    Trip t = get_trip(i, file);
                    if (t.number == number)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "\n[NOT FOUND] Рейс не найден\n";
                    continue;
                }
                Trip updatedTrip;
                updatedTrip.number = number;
                std::cout << "\n-> Введите новый тип автобуса: ";
                std::cin >> std::setw(49) >> updatedTrip.bus_type;
                std::cout << "\n-> Введите новый пункт назначения: ";
                std::cin >> std::setw(49) >> updatedTrip.destination;
                std::cout << "\n-> Введите новую дату отправления (ГГГГ-ММ-ДД): ";
                std::cin >> std::setw(19) >> updatedTrip.date_departure;
                std::cout << "\n-> Введите новое время отправления (ЧЧ:ММ): ";
                std::cin >> std::setw(9) >> updatedTrip.time_departure;
                std::cout << "\n-> Введите новое время прибытия (ЧЧ:ММ): ";
                std::cin >> std::setw(9) >> updatedTrip.time_arrival;
                std::cout << "\n-> Введите новую стоимость билета: ";
                std::cin >> updatedTrip.ticket_cost;
                std::cout << "\n-> Введите новое количество оставшихся билетов: ";
                std::cin >> updatedTrip.tickets_left;
                std::cout << "\n-> Введите новое количество проданных билетов: ";
                std::cin >> updatedTrip.tickets_sold;
                if (std::cin.fail())
                {
                    std::cout << "\n[ERROR] Неверный ввод\n";
                    continue;
                }
                update_by_num(number, updatedTrip, file);
                std::cout << "\n[OK] Рейс обновлён\n";
            }
            else if (choise == 12)
            {
                if (file != nullptr)
                {
                    fclose(file);
                    file = nullptr;
                }
                file_selected = false;
                break;
            }
            else if (choise == 13)
                working_with_file = false;
            else
                std::cout << "\n[ERROR] Неверный ввод\n";

            std::cout << "\n=========================================================================\n";
        }
        if (file != nullptr)
        {
            fclose(file);
            file = nullptr;
        }
    }
    return 0;
}
