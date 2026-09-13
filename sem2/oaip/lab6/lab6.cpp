#include <iostream>
#include <cstring>
#include <ctime>
struct Data
{
    char surname[100];
    int place;
    int points;
};

int random(int max)
{
    return int(double(rand()) / double(RAND_MAX) * max);
}

int M = 10;

int hash1(int place)
{
    return place % M; // i
}
int hash2(int place)
{
    return 1 + (place % (M - 2)); // c
}

void add(Data *hash_table, Data data)
{
    int i = hash1(data.place);
    int c = hash2(data.place);
    while (hash_table[i].place != -1 && hash_table[i].place != -2)
    {
        i -= c;
        if (i < 0)
            i += M;
    }
    hash_table[i] = data;
}

void print(Data *hash_table)
{
    for (int i = 0; i < M; i++)
    {
        std::cout << i << ". " << hash_table[i].surname << ' ' << hash_table[i].place << ' ' << hash_table[i].points << std::endl;
    }
}

bool find(Data *hash_table, int place)
{
    int i = hash1(place);
    int c = hash2(place);
    while (hash_table[i].place != -1)
    {
        if (hash_table[i].place == place)
            return true;
        i -= c;
        if (i < 0)
            i += M;
    }
    return false;
}

void remove(Data *hash_table, int place)
{
    int i = hash1(place);
    int c = hash2(place);
    while (hash_table[i].place != place)
    {
        if (hash_table[i].place == -1)
            return;
        i -= c;
        if (i < 0)
            i += M;
    }
    hash_table[i].place = -2;
}

int main()
{
    srand(time(NULL));
    Data *hash_table = new Data[M];
    for (int i = 0; i < M; i++)
        hash_table[i].place = -1;

    std::cout << "-------------------------------\n";
    std::cout << "       Random test             \n";
    std::cout << "-------------------------------\n";
    std::cout << "Before number adding:\n";
    print(hash_table);

    for (int i = 0; i < M * 0.7; ++i)
    {
        Data data;
        data.place = random(10 * M);
        data.points = M - data.place;
        strcpy(data.surname, "student");
        add(hash_table, data);
    }
    std::cout << "\nAfter:\n";
    print(hash_table);
    int r = random(M);
    int attempts = 0;
    while (hash_table[r].place < 0 && attempts < 100)
    {
        r = random(M);
        attempts++;
    }

    std::cout << "Trying to find " << hash_table[r].place << " in table: ";
    std::cout << find(hash_table, hash_table[r].place) << '\n';

    r = random(10 * M);
    std::cout << "Trying to find " << r << " in table: ";
    std::cout << find(hash_table, r) << '\n';

    r = random(M - 1);
    while (hash_table[r].place == -1)
        r = random(M - 1);
    std::cout << "\nRemoving " << hash_table[r].place << "\n";

    int key_to_remove = hash_table[r].place;
    remove(hash_table, key_to_remove);
    print(hash_table);
    std::cout << "Trying to find key " << key_to_remove << " after removal: ";
    std::cout << find(hash_table, key_to_remove) << '\n';

    delete[] hash_table;
    return 0;
}