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