#include <iostream>
#include <cstring>

const char *DATA_FILENAME = "data.bin";

struct Trip
{
    int number;
    char bus_type[100];
    char destination[100];
    char date_departure[10];
    char time_departure[5];
    char time_arrival[5];
    float ticket_cost;
    int tickets_left;
    int tickets_sold;
};
#pragma region File working Functions
void init_file(FILE *file)
{
    file = fopen(DATA_FILENAME, "rb");
    if (file == nullptr)
    {
        file = fopen(DATA_FILENAME, "wb");
        fclose(file);
        file = fopen(DATA_FILENAME, "rb");
    }
}

int get_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

Trip *get_trip(FILE *file, int ind)
{
    if (sizeof(Trip) * ind >= get_size(file))
        return nullptr;
    fseek(file, sizeof(Trip) * ind, SEEK_SET);
    Trip *tr;
    fread(tr, sizeof(Trip), 1, file);
    return tr;
}

#pragma endregion

int main()
{
    FILE *file;
    init_file(file);

    return 0;
}