#include <iostream>
#include <cstring>
#include <iomanip>
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
void print_table();
long get_struct_num(FILE *file);

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
long file_struct_num = 0;
bool init_report_file(const char *filename, FILE *&file, const char *open_type)
{
    if (file != nullptr)
    {
        fclose(file);
        file = nullptr;
    }
    file = fopen(filename, open_type);
    if (!file)
    {
        file = fopen(filename, "w");
        if (file == nullptr)
            return false;
        fclose(file);
        file = fopen(filename, open_type);
    }
    if (!file)
        return false;
    fprintf(file, "\n");
    fprintf(file, "%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s\n",
            COL_NUMBER, "Trip number",
            COL_BUS_TYPE, "Bus type",
            COL_DESTINATION, "Destination",
            COL_DATE, "Date departure",
            COL_TIME, "Time departure",
            COL_TIME, "Time arrival",
            COL_COST, "Ticket cost",
            COL_LEFT, "Tickets left",
            COL_SOLD, "Tickets sold");
    return true;
}
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
    file_struct_num = get_struct_num(file);
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
    Trip tr;
    if (ind < 0 || ind >= file_struct_num)
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
    if (ind < 0 || ind > file_struct_num)
    {
        std::cout << "index was out of the bounds of array\n";
        return;
    }

    fseek(file, sizeof(Trip) * ind, SEEK_SET);
    fwrite(trip, sizeof(Trip), 1, file);
    fflush(file);
    if (ind == file_struct_num)
        file_struct_num = get_struct_num(file);
}
void delete_last_trip(FILE *file, const char *filename)
{
    if (file_struct_num == 0)
        return;
    int new_size = sizeof(Trip) * (file_struct_num - 1);
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
    file_struct_num = get_struct_num(file);
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
    int n = file_struct_num;
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
        set_trip(j + 1, &key, file);
    }
    print_trips(file);
}
void selection_sort(FILE *file)
{
    int n = file_struct_num;
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
    int n = file_struct_num;
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
void sort_by_time_arrival(FILE *file)
{
    int n = file_struct_num;
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
    long size = file_struct_num;
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
    int n = file_struct_num;
    if (n == 0)
    {
        std::cout << "No matching records found\n";
        return;
    }
    quick_sort(file);
    int l = 0;
    int r = file_struct_num - 1;
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
    print_table();
    while (idx < n)
    {
        tmp = get_trip(idx, file);
        if (strcmp(tmp.destination, destination) != 0)
            break;
        print_trip(tmp);
        found = true;
        ++idx;
    }

    if (!found)
        std::cout << "No matching records found\n";
}
void print_proper_trips(const char *dest, const char *max_time, int min_ticks, FILE *main_file, FILE *&report)
{
    int n = file_struct_num;
    if (n == 0)
    {
        std::cout << "\nNo records found matching the criteria\n";
        return;
    }
    int j = 0;
    bool found = 0;

    for (int i = 0; i < n; ++i)
    {
        Trip tr = get_trip(i, main_file);
        if (strcmp(tr.destination, dest) == 0 && strcmp(tr.time_arrival, max_time) <= 0 && tr.tickets_left >= min_ticks)
        {
            if (!found)
            {
                print_table();
                found = 1;
            }
            ++j;
            print_trip(tr);
            if (report)
            {
                fprintf(report, "%-*d%-*s%-*s%-*s%-*s%-*s%-*.*f%-*d%-*d\n",
                        COL_NUMBER, tr.number,
                        COL_BUS_TYPE, tr.bus_type,
                        COL_DESTINATION, tr.destination,
                        COL_DATE, tr.date_departure,
                        COL_TIME, tr.time_departure,
                        COL_TIME, tr.time_arrival,
                        COL_COST, 2, tr.ticket_cost,
                        COL_LEFT, tr.tickets_left,
                        COL_SOLD, tr.tickets_sold);
            }
        }
    }
    if (j == 0)
        std::cout << "\nNo records found matching the criteria\n";
    if (report)
        fclose(report);
}
void print_by_bus_type(const char *bus_type, const char *min_departure_time, FILE *file, FILE *&report)
{
    bool found = false;
    for (int i = 0; i < file_struct_num; ++i)
    {
        Trip trip = get_trip(i, file);
        if (strcmp(bus_type, trip.bus_type) == 0 && strcmp(trip.time_departure, min_departure_time) > 0)
        {
            if (!found)
                print_table();

            print_trip(trip);
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
    int n = file_struct_num;
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
    for (int i = 0; i < file_struct_num; ++i)
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
        std::cout << "\n[NOT FOUND] Trip not found\n";
}
#pragma endregion

void print_trips(FILE *file)
{
    print_table();
    for (int i = 0; i < file_struct_num; ++i)
    {
        Trip trip = get_trip(i, file);
        print_trip(trip);
    }
    for (int i = 0; i < ROW_SIZE; ++i)
        std::cout << "-";
    std::cout << '\n';
}

void print_trip(Trip trip)
{
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
void print_table()
{
    std::cout << '\n';
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
    std::cout << "                              BUS TICKET SALES TRACKING SYSTEM\n\n";
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
            std::cout << "|       FILE SELECTION MENU           |\n";
            std::cout << "+-------------------------------------+\n";
            std::cout << "  1 -> Create new file\n";
            std::cout << "  2 -> Delete file\n";
            std::cout << "  3 -> Clear file\n";
            std::cout << "  4 -> Set default file\n";
            std::cout << "  5 -> Open file\n";
            std::cout << "  6 -> Exit program\n";
            std::cout << "\n-> Choice: ";
            int choice = 0;
            std::cin >> choice;
            if (std::cin.fail())
            {
                std::cout << "\nError: Invalid input\n";
                std::cin.clear();
                std::cin.ignore(9999, '\n');
                continue;
            }

            switch (choice)
            {
            case 1:
            {
                std::cout << "\n-> Enter new file name: ";
                std::cin >> std::setw(99) >> current_file;
                if (init_file(current_file, file))
                {
                    initialize_data(file);
                    std::cout << "\nSuccess: File created successfully\n";
                    file_selected = true;
                }
                else
                    std::cout << "\nError: Failed to create file\n";
                break;
            }
            case 2:
            {
                std::cout << "\n-> Enter file name to delete: ";
                std::cin >> std::setw(99) >> current_file;
                if (remove(current_file) == 0)
                    std::cout << "\nSuccess: File deleted successfully\n";
                else
                    std::cout << "\nError: Failed to delete file\n";
                break;
            }
            case 3:
            {
                std::cout << "\n-> Enter file name to clear: ";
                std::cin >> std::setw(99) >> current_file;
                if (clear_file(current_file))
                    std::cout << "\nSuccess: File cleared successfully\n";
                else
                    std::cout << "\nError: Failed to clear file\n";
                break;
            }
            case 4:
            {
                std::strcpy(current_file, "data.bin");
                std::cout << "\nSuccess: Default file set: " << current_file << "\n";
                if (init_file(current_file, file))
                {
                    if (file_struct_num == 0)
                        initialize_data(file);
                    file_selected = true;
                }
                break;
            }
            case 5:
            {
                std::cout << "\n-> Enter file name to open: ";
                std::cin >> std::setw(99) >> current_file;
                if (init_file(current_file, file))
                {
                    std::cout << "\nSuccess: File opened: " << current_file << "\n";
                    file_selected = true;
                }
                else
                    std::cout << "\nError: Failed to open file\n";
                break;
            }
            case 6:
                fclose(file);
                running = false;
                file_selected = true;
                break;
            default:
                std::cout << "\nError: Invalid choice\n";
            }
        }

        bool working_with_file = true;
        while (working_with_file)
        {
            std::cout << "\n+---------------------------------------------------------+\n";
            std::cout << "|  WORKING WITH FILE: " << std::left << std::setw(38) << current_file << " |\n";
            std::cout << "+---------------------------------------------------------+\n";
            std::cout << "\n  1 -> Display all trips\n";
            std::cout << "  2 -> Sort by destination\n";
            std::cout << "  3 -> Sort by departure date\n";
            std::cout << "  4 -> Sort by arrival time\n";
            std::cout << "  5 -> Search trip by number\n";
            std::cout << "  6 -> Search by destination\n";
            std::cout << "  7 -> Filter by arrival time and tickets\n";
            std::cout << "  8 -> Filter by bus type and time\n";
            std::cout << "  9 -> Add trip\n";
            std::cout << "  10 -> Delete trip\n";
            std::cout << "  11 -> Edit trip\n";
            std::cout << "  12 -> Return to file selection menu\n";
            std::cout << "  13 -> Exit program\n";
            std::cout << "\n-> Choice: ";
            int choise = 0;
            std::cin >> choise;
            if (std::cin.fail())
            {
                std::cout << "\nError: Invalid input\n";
                std::cin.clear();
                std::cin.ignore(9999, '\n');
                continue;
            }

            if (choise == 1)
                print_trips(file);
            else if (choise == 2)
            {
                quick_sort(file);
                print_trips(file);
            }
            else if (choise == 3)
                selection_sort(file);
            else if (choise == 4)
                insertion_sort(file);
            else if (choise == 5)
            {
                std::cout << "\n-> Enter trip number: ";
                int race_number = 0;
                std::cin >> race_number;
                if (std::cin.fail())
                {
                    std::cout << "\nError: Invalid input\n";
                    continue;
                }
                bool success = false;
                Trip tr = find_by_race_number(race_number, file, &success);
                if (success)
                {
                    print_table();
                    print_trip(tr);
                }
                else
                    std::cout << "\nNot found: Trip not found\n";
            }
            else if (choise == 6)
            {
                std::cout << "\n-> Enter destination: ";
                char dest[50];
                std::cin >> std::setw(49) >> dest;
                find_by_destination(dest, file);
            }
            else if (choise == 7)
            {
                std::cout << "\n-> Enter destination: ";
                char dest[50];
                std::cin >> std::setw(49) >> dest;
                std::cout << "\n-> Enter maximum arrival time (HH:MM): ";
                char max_time[10];
                std::cin >> std::setw(9) >> max_time;
                std::cout << "\n-> Enter minimum number of unsold tickets: ";
                int min_ticks_left = 0;
                std::cin >> min_ticks_left;
                if (std::cin.fail())
                {
                    std::cout << "\nError: Invalid input\n";
                    continue;
                }
                std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
                int ans = 0;
                std::cin >> ans;
                FILE *report = NULL;
                if (ans == 1)
                    init_report_file("report.txt", report, "a");
                else if (ans == 2)
                    init_report_file("report.txt", report, "w");
                print_proper_trips(dest, max_time, min_ticks_left, file, report);
            }
            else if (choise == 8)
            {
                std::cout << "\n-> Enter bus type: ";
                char bus_type[50];
                std::cin >> std::setw(49) >> bus_type;
                std::cout << "\n-> Enter minimum departure time (HH:MM): ";
                char min_time[10];
                std::cin >> std::setw(9) >> min_time;
                std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
                int ans = 0;
                std::cin >> ans;
                FILE *report = NULL;
                if (ans == 1)
                    init_report_file("report.txt", report, "a");
                else if (ans == 2)
                    init_report_file("report.txt", report, "w");
                print_by_bus_type(bus_type, min_time, file, report);
            }
            else if (choise == 9)
            {
                Trip newTrip;
                std::cout << "\n-> Enter trip number: ";
                std::cin >> newTrip.number;
                if (std::cin.fail())
                {
                    std::cout << "\nError: Invalid input\n";
                    continue;
                }
                bool exists = false;
                for (int i = 0; i < file_struct_num; ++i)
                {
                    Trip t = get_trip(i, file);
                    if (t.number == newTrip.number)
                    {
                        exists = true;
                        break;
                    }
                }
                if (exists)
                    std::cout << "\nError: Trip with this number already exists\n";
                else
                {
                    std::cout << "\n-> Enter bus type: ";
                    std::cin >> std::setw(49) >> newTrip.bus_type;
                    std::cout << "\n-> Enter destination: ";
                    std::cin >> std::setw(49) >> newTrip.destination;
                    std::cout << "\n-> Enter departure date (YYYY-MM-DD): ";
                    std::cin >> std::setw(19) >> newTrip.date_departure;
                    std::cout << "\n-> Enter departure time (HH:MM): ";
                    std::cin >> std::setw(9) >> newTrip.time_departure;
                    std::cout << "\n-> Enter arrival time (HH:MM): ";
                    std::cin >> std::setw(9) >> newTrip.time_arrival;
                    std::cout << "\n-> Enter ticket cost: ";
                    std::cin >> newTrip.ticket_cost;
                    std::cout << "\n-> Enter number of tickets left: ";
                    std::cin >> newTrip.tickets_left;
                    std::cout << "\n-> Enter number of tickets sold: ";
                    std::cin >> newTrip.tickets_sold;
                    if (std::cin.fail())
                    {
                        std::cout << "\nError: Invalid input\n";
                        continue;
                    }
                    set_trip(file_struct_num, &newTrip, file);
                    std::cout << "\nSuccess: Trip added\n";
                }
            }
            else if (choise == 10)
            {
                std::cout << "\n-> Enter trip number to delete: ";
                int number;
                std::cin >> number;
                if (std::cin.fail())
                {
                    std::cout << "\nError: Invalid input\n";
                    continue;
                }
                delete_by_num(number, file, current_file);
            }
            else if (choise == 11)
            {
                std::cout << "\n-> Enter trip number to edit: ";
                int number;
                std::cin >> number;
                if (std::cin.fail())
                {
                    std::cout << "\nError: Invalid input\n";
                    continue;
                }
                Trip updatedTrip;
                bool found = false;
                for (int i = 0; i < file_struct_num; ++i)
                {
                    updatedTrip = get_trip(i, file);
                    if (updatedTrip.number == number)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "\nNot found: Trip not found\n";
                    continue;   
                }
                std::cout << "\n-> What do you want to update? (1 - bus type, 2 - destination, 3 - departure date, 4 - departure time, 5 - arrival time, 6 - ticket cost, 7 - tickets left, 8 - tickets sold\n-> ");
                int c = 0;
std::cin>>c;
if(c<1||c>8){std::cout << "\nError: Invalid input\n";
                    continue;}
if(c==1){
                std::cout << "\n-> Enter new bus type: ";
                std::cin >> std::setw(49) >> updatedTrip.bus_type;}if(c==2){
                std::cout << "\n-> Enter new destination: ";

                std::cin >> std::setw(49) >> updatedTrip.destination;}if(c==3){
                std::cout << "\n-> Enter new departure date (YYYY-MM-DD): ";
                std::cin >> std::setw(19) >> updatedTrip.date_departure;}if(c==4){
                std::cout << "\n-> Enter new departure time (HH:MM): ";
                std::cin >> std::setw(9) >> updatedTrip.time_departure;}if(c==5){
                std::cout << "\n-> Enter new arrival time (HH:MM): ";
                std::cin >> std::setw(9) >> updatedTrip.time_arrival;}if(c==6){
                std::cout << "\n-> Enter new ticket cost: ";
                std::cin >> updatedTrip.ticket_cost;}if(c==7){
                std::cout << "\n-> Enter new number of tickets left: ";
                std::cin >> updatedTrip.tickets_left;}if(c==8){
                std::cout << "\n-> Enter new number of tickets sold: ";
                std::cin >> updatedTrip.tickets_sold;}
                if (std::cin.fail())
                {
                    std::cout << "\nError: Invalid input\n";
                    continue;
                }
                update_by_num(number, updatedTrip, file);
                std::cout << "\nSuccess: Trip updated\n";
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
            {
                working_with_file = false;
                return 0;
            }
            else
                std::cout << "\nError: Invalid choice\n";

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