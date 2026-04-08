#include <iostream>
#include <cstring>
#include <iomanip>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

struct RentalRecord
{
    char item_name[50];
    char renter_name[50];
    char rental_date[20];
    int rental_days;
    int status;

    RentalRecord() : rental_days(0), status(0)
    {
        item_name[0] = '\0';
        renter_name[0] = '\0';
        rental_date[0] = '\0';
    }
};

struct StackNode
{
    int l;
    int r;
    StackNode *next;
    StackNode(int left, int right) : l(left), r(right), next(nullptr) {}
};

struct Stack
{
    StackNode *root;

    Stack() : root(nullptr) {}

    void push(int l, int r)
    {
        StackNode *node = new StackNode(l, r);
        node->next = root;
        root = node;
    }

    bool pop(int &l, int &r)
    {
        if (!root)
            return false;
        StackNode *tmp = root;
        root = root->next;
        l = tmp->l;
        r = tmp->r;
        delete tmp;
        return true;
    }

    bool empty() { return root == nullptr; }

    ~Stack()
    {
        while (root)
        {
            StackNode *tmp = root;
            root = root->next;
            delete tmp;
        }
    }
};

bool open_file(const char *filename, FILE *&file)
{
    if (file)
        fclose(file);
    file = std::fopen(filename, "r+b");
    if (!file)
    {
        file = std::fopen(filename, "wb");
        if (file)
        {
            std::fclose(file);
            file = std::fopen(filename, "r+b");
        }
    }
    return file != nullptr;
}

long count_records(FILE *file)
{
    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    return size / sizeof(RentalRecord);
}

RentalRecord read_record(int index, FILE *file, bool *ok = nullptr)
{
    RentalRecord rec;
    if (ok)
        *ok = true;
    long cnt = count_records(file);
    if (index >= cnt)
    {
        if (ok)
            *ok = false;
        return rec;
    }
    std::fseek(file, sizeof(RentalRecord) * index, SEEK_SET);
    std::fread(&rec, sizeof(RentalRecord), 1, file);
    return rec;
}

void write_record(int index, RentalRecord *rec, FILE *file)
{
    long cnt = count_records(file);
    if (index >= cnt)
        return;
    std::fseek(file, sizeof(RentalRecord) * index, SEEK_SET);
    std::fwrite(rec, sizeof(RentalRecord), 1, file);
}

void append_record(RentalRecord *rec, FILE *file)
{
    std::fseek(file, 0, SEEK_END);
    std::fwrite(rec, sizeof(RentalRecord), 1, file);
}

void remove_last(FILE *file, const char *filename)
{
    long cnt = count_records(file);
    if (cnt == 0)
        return;
    std::size_t new_size = sizeof(RentalRecord) * (cnt - 1);
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

bool clear_file_data(const char *filename)
{
    FILE *f = std::fopen(filename, "wb");
    if (!f)
        return false;
    std::fclose(f);
    return true;
}

void print_record(const RentalRecord &rec)
{
    std::cout << std::left << std::setw(20) << rec.item_name
              << std::left << std::setw(25) << rec.renter_name
              << std::left << std::setw(15) << rec.rental_date
              << std::left << std::setw(10) << rec.rental_days
              << std::left << std::setw(12) << (rec.status == 0 ? "Available" : "Rented")
              << '\n';
}

void print_all(FILE *file)
{
    long cnt = count_records(file);

    std::cout << "\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(25) << "Renter Name"
              << std::left << std::setw(15) << "Rental Date"
              << std::left << std::setw(10) << "Days"
              << std::left << std::setw(12) << "Status" << '\n';

    for (int i = 0; i < cnt; ++i)
    {
        RentalRecord rec = read_record(i, file);
        print_record(rec);
    }
    std::cout << '\n';
}

void insertion_sort(FILE *file)
{
    long n = count_records(file);
    for (int i = 1; i < n; ++i)
    {
        RentalRecord key = read_record(i, file);
        int j = i - 1;
        while (j >= 0)
        {
            RentalRecord t = read_record(j, file);
            if (std::strcmp(t.item_name, key.item_name) <= 0)
                break;
            write_record(j + 1, &t, file);
            --j;
        }
        write_record(j + 1, &key, file);
    }
}

void selection_sort(FILE *file)
{
    long n = count_records(file);
    for (int i = 0; i < n - 1; ++i)
    {
        RentalRecord minRec = read_record(i, file);
        int minIdx = i;
        for (int j = i + 1; j < n; ++j)
        {
            RentalRecord tmp = read_record(j, file);
            if (tmp.rental_days < minRec.rental_days)
            {
                minRec = tmp;
                minIdx = j;
            }
        }
        if (minIdx != i)
        {
            RentalRecord cur = read_record(i, file);
            write_record(minIdx, &cur, file);
            write_record(i, &minRec, file);
        }
    }
}

void quick_sort(FILE *file)
{
    long n = count_records(file);
    if (n < 2)
        return;

    Stack st;
    st.push(0, n - 1);

    int l, r;
    while (st.pop(l, r))
    {
        int mid = (l + r) / 2;
        RentalRecord pivot = read_record(mid, file);

        int i = l, j = r;
        while (i <= j)
        {
            while (true)
            {
                RentalRecord tmp = read_record(i, file);
                if (std::strcmp(tmp.rental_date, pivot.rental_date) >= 0)
                    break;
                ++i;
            }
            while (true)
            {
                RentalRecord tmp = read_record(j, file);
                if (std::strcmp(tmp.rental_date, pivot.rental_date) <= 0)
                    break;
                --j;
            }
            if (i <= j)
            {
                RentalRecord ti = read_record(i, file);
                RentalRecord tj = read_record(j, file);
                write_record(i, &tj, file);
                write_record(j, &ti, file);
                ++i;
                --j;
            }
        }
        if (l < j)
            st.push(l, j);
        if (i < r)
            st.push(i, r);
    }
}

RentalRecord linear_search(FILE *file, const char *renter)
{
    RentalRecord result;
    long cnt = count_records(file);
    for (int i = 0; i < cnt; ++i)
    {
        RentalRecord rec = read_record(i, file);
        if (std::strcmp(rec.renter_name, renter) == 0)
        {
            return rec;
        }
    }
    return result;
}

RentalRecord binary_search(FILE *file, const char *name)
{
    insertion_sort(file);

    int l = 0;
    int r = count_records(file) - 1;

    while (l <= r)
    {
        int mid = (l + r) / 2;
        RentalRecord rec = read_record(mid, file);
        int cmp = std::strcmp(rec.item_name, name);
        if (cmp == 0)
            return rec;
        if (cmp < 0)
            l = mid + 1;
        else
            r = mid - 1;
    }
    return RentalRecord();
}

void find_by_item_and_date(FILE *file, const char *itemName, const char *date)
{
    long cnt = count_records(file);
    bool found = false;

    std::cout << "\nRenters of '" << itemName << "' after " << date << ":\n\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(25) << "Renter Name"
              << std::left << std::setw(15) << "Rental Date"
              << std::left << std::setw(10) << "Days"
              << std::left << std::setw(12) << "Status" << '\n';

    for (int i = 0; i < cnt; ++i)
    {
        RentalRecord rec = read_record(i, file);
        if (std::strcmp(rec.item_name, itemName) == 0 &&
            std::strcmp(rec.rental_date, date) > 0)
        {
            print_record(rec);
            found = true;
        }
    }

    if (!found)
    {
        std::cout << "No matching records found.\n";
    }
}

void statistics(FILE *file)
{
    const char *tmpName = "temp.bin";
    clear_file_data(tmpName);
    FILE *tmp;
    open_file(tmpName, tmp);

    long cnt = count_records(file);
    for (int i = 0; i < cnt; ++i)
    {
        RentalRecord rec = read_record(i, file);
        std::fwrite(&rec, sizeof(RentalRecord), 1, tmp);
    }

    long n = count_records(tmp);
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            RentalRecord ti = read_record(i, tmp);
            RentalRecord tj = read_record(j, tmp);
            if (tj.rental_days > ti.rental_days)
            {
                write_record(i, &tj, tmp);
                write_record(j, &ti, tmp);
            }
        }
    }

    std::cout << "\n=== Statistics by Item ===\n\n";
    print_all(tmp);

    std::cout << "\n=== Most Popular Item ===\n";

    char mostPopular[50] = "";
    int maxCount = 0;

    for (int i = 0; i < cnt; ++i)
    {
        RentalRecord rec = read_record(i, file);
        int count = 1;
        for (int j = i + 1; j < cnt; ++j)
        {
            RentalRecord other = read_record(j, file);
            if (std::strcmp(other.item_name, rec.item_name) == 0)
            {
                count++;
            }
        }

        bool alreadyCounted = false;
        for (int k = 0; k < i; ++k)
        {
            RentalRecord prev = read_record(k, file);
            if (std::strcmp(prev.item_name, rec.item_name) == 0)
            {
                alreadyCounted = true;
                break;
            }
        }

        if (!alreadyCounted && count > maxCount)
        {
            maxCount = count;
            std::strcpy(mostPopular, rec.item_name);
        }
    }

    if (maxCount > 0)
    {
        std::cout << "Most rented item: " << mostPopular
                  << " (rented " << maxCount << " times)\n";
    }

    std::fclose(tmp);
    std::remove(tmpName);
}

void init_data(FILE *file)
{
    RentalRecord r0;
    std::strcpy(r0.item_name, "Tennis Racket");
    std::strcpy(r0.renter_name, "Ivanov");
    std::strcpy(r0.rental_date, "2024-06-01");
    r0.rental_days = 5;
    r0.status = 1;
    std::fwrite(&r0, sizeof(RentalRecord), 1, file);

    RentalRecord r1;
    std::strcpy(r1.item_name, "Football");
    std::strcpy(r1.renter_name, "Petrov");
    std::strcpy(r1.rental_date, "2024-06-02");
    r1.rental_days = 3;
    r1.status = 1;
    std::fwrite(&r1, sizeof(RentalRecord), 1, file);

    RentalRecord r2;
    std::strcpy(r2.item_name, "Basketball");
    std::strcpy(r2.renter_name, "Sidorov");
    std::strcpy(r2.rental_date, "2024-06-03");
    r2.rental_days = 7;
    r2.status = 1;
    std::fwrite(&r2, sizeof(RentalRecord), 1, file);

    RentalRecord r3;
    std::strcpy(r3.item_name, "Tennis Racket");
    std::strcpy(r3.renter_name, "Kuznetsov");
    std::strcpy(r3.rental_date, "2024-06-04");
    r3.rental_days = 2;
    r3.status = 1;
    std::fwrite(&r3, sizeof(RentalRecord), 1, file);

    RentalRecord r4;
    std::strcpy(r4.item_name, "Volleyball");
    std::strcpy(r4.renter_name, "Ivanov");
    std::strcpy(r4.rental_date, "2024-06-05");
    r4.rental_days = 4;
    r4.status = 1;
    std::fwrite(&r4, sizeof(RentalRecord), 1, file);

    RentalRecord r5;
    std::strcpy(r5.item_name, "Football");
    std::strcpy(r5.renter_name, "Smirnov");
    std::strcpy(r5.rental_date, "2024-06-06");
    r5.rental_days = 6;
    r5.status = 1;
    std::fwrite(&r5, sizeof(RentalRecord), 1, file);

    RentalRecord r6;
    std::strcpy(r6.item_name, "Ski Set");
    std::strcpy(r6.renter_name, "Petrov");
    std::strcpy(r6.rental_date, "2024-06-07");
    r6.rental_days = 10;
    r6.status = 1;
    std::fwrite(&r6, sizeof(RentalRecord), 1, file);

    RentalRecord r7;
    std::strcpy(r7.item_name, "Basketball");
    std::strcpy(r7.renter_name, "Kozlov");
    std::strcpy(r7.rental_date, "2024-06-08");
    r7.rental_days = 3;
    r7.status = 1;
    std::fwrite(&r7, sizeof(RentalRecord), 1, file);

    RentalRecord r8;
    std::strcpy(r8.item_name, "Tennis Racket");
    std::strcpy(r8.renter_name, "Morozov");
    std::strcpy(r8.rental_date, "2024-06-09");
    r8.rental_days = 5;
    r8.status = 1;
    std::fwrite(&r8, sizeof(RentalRecord), 1, file);

    RentalRecord r9;
    std::strcpy(r9.item_name, "Football");
    std::strcpy(r9.renter_name, "Ivanov");
    std::strcpy(r9.rental_date, "2024-06-10");
    r9.rental_days = 4;
    r9.status = 1;
    std::fwrite(&r9, sizeof(RentalRecord), 1, file);
}

int main()
{
    std::cout << "**********************************************************************\n";
    std::cout << "           SPORTS EQUIPMENT RENTAL MANAGEMENT SYSTEM\n";
    std::cout << "**********************************************************************\n\n";

    char currentFile[100] = "";
    bool fileOpen = false;
    FILE *file = nullptr;

    while (!fileOpen)
    {
        std::cout << "\n+--------------------------+\n";
        std::cout << "|      FILE SELECTION      |\n";
        std::cout << "+--------------------------+\n";
        std::cout << "  1. Create new file\n";
        std::cout << "  2. Delete file\n";
        std::cout << "  3. Clear file\n";
        std::cout << "  4. Use default file\n";
        std::cout << "  5. Open existing file\n";
        std::cout << "  6. Exit\n";
        std::cout << "\nSelect option: ";

        int choice;
        std::cin >> choice;
        if (std::cin.fail())
        {
            std::cout << "\nInvalid input!\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
        {
            std::cout << "\nEnter new filename: ";
            std::cin >> currentFile;
            if (open_file(currentFile, file))
            {
                std::cout << "\nFile created successfully.\n";
                init_data(file);
                fileOpen = true;
            }
            else
            {
                std::cout << "\nFailed to create file.\n";
            }
            break;
        }
        case 2:
        {
            std::cout << "\nEnter filename to delete: ";
            std::cin >> currentFile;
            if (std::remove(currentFile) == 0)
            {
                std::cout << "\nFile deleted.\n";
            }
            else
            {
                std::cout << "\nCannot delete file.\n";
            }
            break;
        }
        case 3:
        {
            std::cout << "\nEnter filename to clear: ";
            std::cin >> currentFile;
            if (clear_file_data(currentFile))
            {
                std::cout << "\nFile cleared.\n";
            }
            else
            {
                std::cout << "\nCannot clear file.\n";
            }
            break;
        }
        case 4:
        {
            std::strcpy(currentFile, "rentals.bin");
            std::cout << "\nUsing default file: " << currentFile << "\n";
            if (open_file(currentFile, file))
            {
                init_data(file);
                fileOpen = true;
            }
            break;
        }
        case 5:
        {
            std::cout << "\nEnter filename: ";
            std::cin >> currentFile;
            if (open_file(currentFile, file))
            {
                std::cout << "\nFile opened: " << currentFile << "\n";
                fileOpen = true;
            }
            else
            {
                std::cout << "\nCannot open file.\n";
            }
            break;
        }
        case 6:
            return 0;
        default:
            std::cout << "\nInvalid option.\n";
        }
    }

    bool running = true;
    while (running)
    {
        std::cout << "\n+--------------------------------------------------+\n";
        std::cout << "| Working with: " << std::left << std::setw(35) << currentFile << " |\n";
        std::cout << "+--------------------------------------------------+\n";
        std::cout << "\n  1. Display all rentals\n";
        std::cout << "  2. Sort by rental date (Quick Sort)\n";
        std::cout << "  3. Sort by rental days (Selection Sort)\n";
        std::cout << "  4. Sort by item name (Insertion Sort)\n";
        std::cout << "  5. Search by renter name (Linear)\n";
        std::cout << "  6. Search by item name (Binary)\n";
        std::cout << "  7. Find renters by item and date\n";
        std::cout << "  8. Statistics by item type\n";
        std::cout << "  9. Add rental record\n";
        std::cout << "  10. Delete rental record\n";
        std::cout << "  11. Update rental record\n";
        std::cout << "  12. Change file\n";
        std::cout << "  13. Exit\n";
        std::cout << "\nSelect option: ";

        int opt;
        std::cin >> opt;
        if (std::cin.fail())
        {
            std::cout << "\nInvalid input!\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (opt == 1)
        {
            print_all(file);
        }
        else if (opt == 2)
        {
            quick_sort(file);
            std::cout << "\nSorted by rental date.\n";
        }
        else if (opt == 3)
        {
            selection_sort(file);
            std::cout << "\nSorted by rental days.\n";
        }
        else if (opt == 4)
        {
            insertion_sort(file);
            std::cout << "\nSorted by item name.\n";
        }
        else if (opt == 5)
        {
            std::cout << "\nEnter renter name: ";
            char name[50];
            std::cin >> name;
            RentalRecord found = linear_search(file, name);
            if (found.renter_name[0] != '\0')
            {
                std::cout << "\nFound:\n";
                print_record(found);
            }
            else
            {
                std::cout << "\nNot found.\n";
            }
        }
        else if (opt == 6)
        {
            std::cout << "\nEnter item name: ";
            char name[50];
            std::cin >> name;
            RentalRecord found = binary_search(file, name);
            if (found.item_name[0] != '\0')
            {
                std::cout << "\nFound:\n";
                print_record(found);
            }
            else
            {
                std::cout << "\nNot found.\n";
            }
        }
        else if (opt == 7)
        {
            std::cout << "\nEnter item name: ";
            char itemName[50];
            std::cin >> itemName;
            std::cout << "\nEnter date (YYYY-MM-DD): ";
            char date[20];
            std::cin >> date;
            find_by_item_and_date(file, itemName, date);
        }
        else if (opt == 8)
        {
            statistics(file);
        }
        else if (opt == 9)
        {
            RentalRecord newRec;
            std::cout << "\nEnter item name: ";
            std::cin >> newRec.item_name;
            std::cout << "\nEnter renter name: ";
            std::cin >> newRec.renter_name;
            std::cout << "\nEnter rental date (YYYY-MM-DD): ";
            std::cin >> newRec.rental_date;
            std::cout << "\nEnter rental days: ";
            std::cin >> newRec.rental_days;
            std::cout << "\nEnter status (0=available, 1=rented): ";
            std::cin >> newRec.status;

            if (std::cin.fail())
            {
                std::cout << "\nInvalid input!\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            append_record(&newRec, file);
            std::cout << "\nRecord added.\n";
        }
        else if (opt == 10)
        {
            std::cout << "\nEnter renter name to delete: ";
            char name[50];
            std::cin >> name;

            long cnt = count_records(file);
            int writeIdx = 0;
            for (int i = 0; i < cnt; ++i)
            {
                RentalRecord rec = read_record(i, file);
                if (std::strcmp(rec.renter_name, name) != 0)
                {
                    write_record(writeIdx, &rec, file);
                    writeIdx++;
                }
            }
            while (writeIdx < cnt)
            {
                remove_last(file, currentFile);
                cnt--;
            }
            std::cout << "\nRecord(s) deleted.\n";
        }
        else if (opt == 11)
        {
            std::cout << "\nEnter renter name to update: ";
            char name[50];
            std::cin >> name;

            long cnt = count_records(file);
            bool found = false;
            for (int i = 0; i < cnt; ++i)
            {
                RentalRecord rec = read_record(i, file);
                if (std::strcmp(rec.renter_name, name) == 0)
                {
                    std::cout << "\nEnter new item name: ";
                    std::cin >> rec.item_name;
                    std::cout << "\nEnter new rental date (YYYY-MM-DD): ";
                    std::cin >> rec.rental_date;
                    std::cout << "\nEnter new rental days: ";
                    std::cin >> rec.rental_days;
                    std::cout << "\nEnter new status (0=available, 1=rented): ";
                    std::cin >> rec.status;

                    if (std::cin.fail())
                    {
                        std::cout << "\nInvalid input!\n";
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        continue;
                    }

                    write_record(i, &rec, file);
                    found = true;
                    std::cout << "\nRecord updated.\n";
                    break;
                }
            }
            if (!found)
            {
                std::cout << "\nNot found.\n";
            }
        }
        else if (opt == 12)
        {
            std::fclose(file);
            fileOpen = false;
            continue;
        }
        else if (opt == 13)
        {
            running = false;
        }
        else
        {
            std::cout << "\nInvalid option.\n";
        }

        std::cout << "\n======================================================================\n";
    }

    std::fclose(file);
    return 0;
}
