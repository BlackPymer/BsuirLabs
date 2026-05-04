#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cstdlib>

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

struct StatNode
{
    char name[50];
    int count;
    int total_days;
    StatNode *next;
    StatNode(const char *n, int c, int d) : count(c), total_days(d), next(nullptr)
    {
        std::strcpy(name, n);
    }
};

long g_record_count = 0;

long get_file_count(FILE *file)
{
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    return size / sizeof(RentalRecord);
}

bool init_file(const char *filename, FILE *&file)
{
    if (file)
        fclose(file);
    file = fopen(filename, "r+b");
    if (!file)
    {
        file = fopen(filename, "wb");
        if (!file)
            return false;
        fclose(file);
        file = fopen(filename, "r+b");
    }
    if (file)
        g_record_count = get_file_count(file);
    return file != nullptr;
}

bool clear_file_data(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f)
        return false;
    fclose(f);
    g_record_count = 0;
    return true;
}

RentalRecord read_record_at(int index, FILE *file, bool *ok = nullptr)
{
    RentalRecord rec;
    if (ok)
        *ok = true;
    if (index < 0 || index >= g_record_count)
    {
        if (ok)
            *ok = false;
        return rec;
    }
    fseek(file, sizeof(RentalRecord) * index, SEEK_SET);
    fread(&rec, sizeof(RentalRecord), 1, file);
    return rec;
}

void write_record_at(int index, RentalRecord *rec, FILE *file)
{
    if (index < 0 || index > g_record_count)
        return;
    fseek(file, sizeof(RentalRecord) * index, SEEK_SET);
    fwrite(rec, sizeof(RentalRecord), 1, file);
    fflush(file);
    if (index == g_record_count)
        g_record_count = get_file_count(file);
}

void append_record(RentalRecord *rec, FILE *file)
{
    fseek(file, 0, SEEK_END);
    fwrite(rec, sizeof(RentalRecord), 1, file);
    g_record_count++;
}

void remove_last(FILE *file, const char *filename)
{
    if (g_record_count == 0)
        return;
    long new_size = sizeof(RentalRecord) * (g_record_count - 1);
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
    g_record_count = get_file_count(file);
}

bool init_report_file(const char *filename, FILE *&file, const char *open_type)
{
    if (file)
    {
        fclose(file);
        file = nullptr;
    }
    file = fopen(filename, open_type);
    if (!file)
    {
        file = fopen(filename, "w");
        if (!file)
            return false;
        fclose(file);
        file = fopen(filename, open_type);
    }
    if (!file)
        return false;
    fprintf(file, "\n%-20s%-25s%-15s%-10s%-12s\n",
            "Item Name", "Renter Name", "Rental Date", "Days", "Status");
    return true;
}

void write_report_header(FILE *report)
{
    fprintf(report, "%-20s%-25s%-15s%-10s%-12s\n",
            "Item Name", "Renter Name", "Rental Date", "Days", "Status");
}

void write_record_to_report(FILE *report, const RentalRecord &rec)
{
    fprintf(report, "%-20s%-25s%-15s%-10d%-12s\n",
            rec.item_name, rec.renter_name, rec.rental_date,
            rec.rental_days, (rec.status == 0 ? "Available" : "Rented"));
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
    std::cout << "\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(25) << "Renter Name"
              << std::left << std::setw(15) << "Rental Date"
              << std::left << std::setw(10) << "Days"
              << std::left << std::setw(12) << "Status" << '\n';

    std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
    int ans;
    std::cin >> ans;
    FILE *report = nullptr;
    if (ans == 1)
        init_report_file("report.txt", report, "a");
    else if (ans == 2)
        init_report_file("report.txt", report, "w");

    fseek(file, 0, SEEK_SET);
    for (long i = 0; i < g_record_count; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        print_record(rec);
        if (report)
            write_record_to_report(report, rec);
    }
    std::cout << '\n';
    if (report)
    {
        fclose(report);
        std::cout << "Report saved to report.txt\n";
    }
}

void insertion_sort(FILE *file)
{
    long n = g_record_count;
    for (int i = 1; i < n; ++i)
    {
        RentalRecord key = read_record_at(i, file);
        int j = i - 1;
        while (j >= 0)
        {
            RentalRecord t = read_record_at(j, file);
            if (std::strcmp(t.item_name, key.item_name) <= 0)
                break;
            write_record_at(j + 1, &t, file);
            --j;
        }
        write_record_at(j + 1, &key, file);
    }
}

void selection_sort(FILE *file)
{
    long n = g_record_count;
    if (n < 2)
        return;
    for (int i = 0; i < n - 1; ++i)
    {
        fseek(file, sizeof(RentalRecord) * i, SEEK_SET);
        RentalRecord cur, min;
        if (fread(&min, sizeof(RentalRecord), 1, file) != 1)
            break;
        cur = min;
        int minIdx = i;
        for (int j = i + 1; j < n; ++j)
        {
            RentalRecord tmp;
            if (fread(&tmp, sizeof(RentalRecord), 1, file) != 1)
                break;
            if (tmp.rental_days < min.rental_days)
            {
                min = tmp;
                minIdx = j;
            }
        }
        if (minIdx != i)
        {
            write_record_at(i, &min, file);
            write_record_at(minIdx, &cur, file);
        }
    }
}

void quick_sort(FILE *file)
{
    long n = g_record_count;
    if (n < 2)
        return;
    Stack st;
    st.push(0, n - 1);
    int l, r;
    while (st.pop(l, r))
    {
        int mid = (l + r) / 2;
        RentalRecord pivot = read_record_at(mid, file);
        int i = l, j = r;
        while (i <= j)
        {
            fseek(file, sizeof(RentalRecord) * i, SEEK_SET);
            RentalRecord t1;
            while (i <= r)
            {
                if (fread(&t1, sizeof(RentalRecord), 1, file) != 1)
                    break;
                if (std::strcmp(t1.rental_date, pivot.rental_date) >= 0)
                    break;
                ++i;
            }
            RentalRecord t2;
            while (j >= l)
            {
                t2 = read_record_at(j, file);
                if (std::strcmp(t2.rental_date, pivot.rental_date) <= 0)
                    break;
                --j;
            }
            if (i <= j)
            {
                write_record_at(i, &t2, file);
                write_record_at(j, &t1, file);
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
    fseek(file, 0, SEEK_SET);
    RentalRecord rec;
    for (long i = 0; i < g_record_count; ++i)
    {
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        if (std::strcmp(rec.renter_name, renter) == 0)
            return rec;
    }
    return RentalRecord();
}

void binary_search_and_print(FILE *file, const char *name)
{
    insertion_sort(file);
    long cnt = g_record_count;
    if (cnt == 0)
    {
        std::cout << "\nNo records.\n";
        return;
    }
    int l = 0, r = cnt - 1, foundIndex = -1;
    while (l <= r)
    {
        int mid = (l + r) / 2;
        RentalRecord rec = read_record_at(mid, file);
        int cmp = std::strcmp(rec.item_name, name);
        if (cmp == 0)
        {
            foundIndex = mid;
            break;
        }
        else if (cmp < 0)
            l = mid + 1;
        else
            r = mid - 1;
    }
    if (foundIndex == -1)
    {
        std::cout << "\nNo records found for item: " << name << "\n";
        std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
        int ans;
        std::cin >> ans;
        FILE *report = nullptr;
        if (ans == 1)
            init_report_file("report.txt", report, "a");
        else if (ans == 2)
            init_report_file("report.txt", report, "w");
        if (report)
        {
            fprintf(report, "No records found for item: %s\n", name);
            fclose(report);
            std::cout << "Report saved to report.txt\n";
        }
        return;
    }
    int left = foundIndex;
    while (left > 0 && std::strcmp(read_record_at(left - 1, file).item_name, name) == 0)
        left--;
    int right = foundIndex;
    while (right < cnt - 1 && std::strcmp(read_record_at(right + 1, file).item_name, name) == 0)
        right++;

    std::cout << "\nFound " << (right - left + 1) << " record(s):\n\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(25) << "Renter Name"
              << std::left << std::setw(15) << "Rental Date"
              << std::left << std::setw(10) << "Days"
              << std::left << std::setw(12) << "Status" << '\n';

    std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
    int ans;
    std::cin >> ans;
    FILE *report = nullptr;
    if (ans == 1)
        init_report_file("report.txt", report, "a");
    else if (ans == 2)
        init_report_file("report.txt", report, "w");

    fseek(file, sizeof(RentalRecord) * left, SEEK_SET);
    for (int i = left; i <= right; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        print_record(rec);
        if (report)
            write_record_to_report(report, rec);
    }
    if (report)
    {
        fclose(report);
        std::cout << "\nReport saved to report.txt\n";
    }
}

void find_by_item_and_date(FILE *file, const char *itemName, const char *date)
{
    bool found = false;
    std::cout << "\nRenters of '" << itemName << "' after " << date << ":\n\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(25) << "Renter Name"
              << std::left << std::setw(15) << "Rental Date"
              << std::left << std::setw(10) << "Days"
              << std::left << std::setw(12) << "Status" << '\n';

    std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
    int ans;
    std::cin >> ans;
    FILE *report = nullptr;
    if (ans == 1)
        init_report_file("report.txt", report, "a");
    else if (ans == 2)
        init_report_file("report.txt", report, "w");
    if (report)
    {
        fprintf(report, "Renters of '%s' after %s:\n\n", itemName, date);
        write_report_header(report);
    }

    fseek(file, 0, SEEK_SET);
    for (long i = 0; i < g_record_count; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        if (std::strcmp(rec.item_name, itemName) == 0 && std::strcmp(rec.rental_date, date) > 0)
        {
            print_record(rec);
            if (report)
                write_record_to_report(report, rec);
            found = true;
        }
    }
    if (!found)
    {
        std::cout << "No matching records found.\n";
        if (report)
            fprintf(report, "No matching records found.\n");
    }
    if (report)
    {
        fclose(report);
        std::cout << "Report saved to report.txt\n";
    }
}

void find_by_date(FILE *file, const char *date)
{
    bool found = false;
    std::cout << "\nRecords with date " << date << ":\n\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(25) << "Renter Name"
              << std::left << std::setw(15) << "Rental Date"
              << std::left << std::setw(10) << "Days"
              << std::left << std::setw(12) << "Status" << '\n';

    FILE *report = fopen("report.txt", "w");
    if (report)
    {
        fprintf(report, "Records with date %s:\n\n", date);
        write_report_header(report);
    }

    fseek(file, 0, SEEK_SET);
    for (long i = 0; i < g_record_count; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        if (std::strcmp(rec.rental_date, date) == 0)
        {
            print_record(rec);
            if (report)
                write_record_to_report(report, rec);
            found = true;
        }
    }
    if (!found)
    {
        std::cout << "No records found.\n";
        if (report)
            fprintf(report, "No records found.\n");
    }
    if (report)
    {
        fclose(report);
        std::cout << "Report saved to report.txt\n";
    }
}

void statistics(FILE *file)
{
    if (g_record_count == 0)
    {
        std::cout << "\nNo records.\n";
        return;
    }
    StatNode *head = nullptr;

    fseek(file, 0, SEEK_SET);
    for (long i = 0; i < g_record_count; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        StatNode *curr = head;
        StatNode *prev = nullptr;
        while (curr)
        {
            if (std::strcmp(curr->name, rec.item_name) == 0)
            {
                curr->count++;
                curr->total_days += rec.rental_days;
                break;
            }
            prev = curr;
            curr = curr->next;
        }
        if (!curr)
        {
            StatNode *newNode = new StatNode(rec.item_name, 1, rec.rental_days);
            if (!head)
                head = newNode;
            else
                prev->next = newNode;
        }
    }

    for (StatNode *p = head; p; p = p->next)
    {
        for (StatNode *q = p->next; q; q = q->next)
        {
            if (q->count > p->count)
            {
                std::swap(p->count, q->count);
                std::swap(p->total_days, q->total_days);
                char tmp[50];
                std::strcpy(tmp, p->name);
                std::strcpy(p->name, q->name);
                std::strcpy(q->name, tmp);
            }
        }
    }

    std::cout << "\n=== Statistics by Item (sorted by popularity) ===\n\n";
    std::cout << std::left << std::setw(20) << "Item Name"
              << std::left << std::setw(15) << "Rentals"
              << std::left << std::setw(15) << "Total Days"
              << std::left << std::setw(10) << "Avg Days" << '\n';

    std::cout << "\nSave result to report.txt? (1 - Yes, append. 2 - Yes, overwrite. 3 - No.)\n-> ";
    int ans;
    std::cin >> ans;
    FILE *report = nullptr;
    if (ans == 1)
        init_report_file("report.txt", report, "a");
    else if (ans == 2)
        init_report_file("report.txt", report, "w");
    if (report)
    {
        fprintf(report, "=== Statistics by Item (sorted by popularity) ===\n\n");
        fprintf(report, "%-20s%-15s%-15s%-10s\n", "Item Name", "Rentals", "Total Days", "Avg Days");
    }

    for (StatNode *p = head; p; p = p->next)
    {
        double avg = p->total_days / (double)p->count;
        std::cout << std::left << std::setw(20) << p->name
                  << std::left << std::setw(15) << p->count
                  << std::left << std::setw(15) << p->total_days
                  << std::left << std::setw(10) << avg << '\n';
        if (report)
            fprintf(report, "%-20s%-15d%-15d%-10.2f\n", p->name, p->count, p->total_days, avg);
    }

    if (head)
    {
        std::cout << "\nMost rented item: " << head->name << " (" << head->count << " times)\n";
        if (report)
            fprintf(report, "\nMost rented item: %s (%d times)\n", head->name, head->count);
    }

    while (head)
    {
        StatNode *tmp = head;
        head = head->next;
        delete tmp;
    }
    if (report)
    {
        fclose(report);
        std::cout << "Report saved to report.txt\n";
    }
}

void delete_record_by_index(FILE *file, const char *filename, int index)
{
    if (index < 0 || index >= g_record_count)
    {
        std::cout << "Invalid index.\n";
        return;
    }
    int writeIdx = 0;
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < g_record_count; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        if (i != index)
            write_record_at(writeIdx++, &rec, file);
    }
    remove_last(file, filename);
    std::cout << "Record deleted.\n";
}

void delete_records_by_renter(FILE *file, const char *filename, const char *renter)
{
    int writeIdx = 0;
    long original_count = g_record_count;
    fseek(file, 0, SEEK_SET);
    for (long i = 0; i < original_count; ++i)
    {
        RentalRecord rec;
        if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
            break;
        if (std::strcmp(rec.renter_name, renter) != 0)
            write_record_at(writeIdx++, &rec, file);
    }
    while (g_record_count > writeIdx)
        remove_last(file, filename);
    std::cout << "\nRecord(s) deleted.\n";
}

void init_data(FILE *file)
{
    RentalRecord r0;
    std::strcpy(r0.item_name, "Tennis Racket");
    std::strcpy(r0.renter_name, "Ivanov");
    std::strcpy(r0.rental_date, "2024-06-01");
    r0.rental_days = 5;
    r0.status = 1;
    append_record(&r0, file);
    RentalRecord r1;
    std::strcpy(r1.item_name, "Football");
    std::strcpy(r1.renter_name, "Petrov");
    std::strcpy(r1.rental_date, "2024-06-02");
    r1.rental_days = 3;
    r1.status = 1;
    append_record(&r1, file);
    RentalRecord r2;
    std::strcpy(r2.item_name, "Basketball");
    std::strcpy(r2.renter_name, "Sidorov");
    std::strcpy(r2.rental_date, "2024-06-03");
    r2.rental_days = 7;
    r2.status = 1;
    append_record(&r2, file);
    RentalRecord r3;
    std::strcpy(r3.item_name, "Tennis Racket");
    std::strcpy(r3.renter_name, "Kuznetsov");
    std::strcpy(r3.rental_date, "2024-06-04");
    r3.rental_days = 2;
    r3.status = 1;
    append_record(&r3, file);
    RentalRecord r4;
    std::strcpy(r4.item_name, "Volleyball");
    std::strcpy(r4.renter_name, "Ivanov");
    std::strcpy(r4.rental_date, "2024-06-05");
    r4.rental_days = 4;
    r4.status = 1;
    append_record(&r4, file);
    RentalRecord r5;
    std::strcpy(r5.item_name, "Football");
    std::strcpy(r5.renter_name, "Smirnov");
    std::strcpy(r5.rental_date, "2024-06-06");
    r5.rental_days = 6;
    r5.status = 1;
    append_record(&r5, file);
    RentalRecord r6;
    std::strcpy(r6.item_name, "Ski Set");
    std::strcpy(r6.renter_name, "Petrov");
    std::strcpy(r6.rental_date, "2024-06-07");
    r6.rental_days = 10;
    r6.status = 1;
    append_record(&r6, file);
    RentalRecord r7;
    std::strcpy(r7.item_name, "Basketball");
    std::strcpy(r7.renter_name, "Kozlov");
    std::strcpy(r7.rental_date, "2024-06-08");
    r7.rental_days = 3;
    r7.status = 1;
    append_record(&r7, file);
    RentalRecord r8;
    std::strcpy(r8.item_name, "Tennis Racket");
    std::strcpy(r8.renter_name, "Morozov");
    std::strcpy(r8.rental_date, "2024-06-09");
    r8.rental_days = 5;
    r8.status = 1;
    append_record(&r8, file);
    RentalRecord r9;
    std::strcpy(r9.item_name, "Football");
    std::strcpy(r9.renter_name, "Ivanov");
    std::strcpy(r9.rental_date, "2024-06-10");
    r9.rental_days = 4;
    r9.status = 1;
    append_record(&r9, file);
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
        std::cout << "  6. Edit file\n";
        std::cout << "  7. Exit\n";
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
            std::cout << "\nEnter new filename: ";
            std::cin >> currentFile;
            if (init_file(currentFile, file))
            {
                std::cout << "\nFile created successfully.\n";
                std::cout << "Fill with default data? (1 - Yes, 0 - No): ";
                int fill;
                std::cin >> fill;
                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    fill = 0;
                }
                if (fill == 1)
                    init_data(file);
                fileOpen = true;
            }
            else
                std::cout << "\nFailed to create file.\n";
            break;
        case 2:
            std::cout << "\nEnter filename to delete: ";
            std::cin >> currentFile;
            if (std::remove(currentFile) == 0)
                std::cout << "\nFile deleted.\n";
            else
                std::cout << "\nCannot delete file.\n";
            break;
        case 3:
            std::cout << "\nEnter filename to clear: ";
            std::cin >> currentFile;
            if (clear_file_data(currentFile))
                std::cout << "\nFile cleared.\n";
            else
                std::cout << "\nCannot clear file.\n";
            break;
        case 4:
            std::strcpy(currentFile, "rentals.bin");
            std::cout << "\nUsing default file: " << currentFile << "\n";
            if (init_file(currentFile, file))
            {
                if (g_record_count == 0)
                    init_data(file);
                fileOpen = true;
            }
            break;
        case 5:
            std::cout << "\nEnter filename: ";
            std::cin >> currentFile;
            if (init_file(currentFile, file))
            {
                std::cout << "\nFile opened: " << currentFile << "\n";
                fileOpen = true;
            }
            else
                std::cout << "\nCannot open file.\n";
            break;
        case 6:
        {
            bool editing = true;
            while (editing)
            {
                std::cout << "\n+--------------------------+\n";
                std::cout << "|      FILE OPERATIONS     |\n";
                std::cout << "+--------------------------+\n";
                std::cout << "  1. Rename file\n";
                std::cout << "  2. Copy file\n";
                std::cout << "  3. Back\n";
                std::cout << "\nSelect option: ";
                int subChoice;
                std::cin >> subChoice;
                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    continue;
                }
                if (subChoice == 1)
                {
                    char oldName[100], newName[100];
                    std::cout << "\nEnter current filename: ";
                    std::cin >> oldName;
                    std::cout << "\nEnter new filename: ";
                    std::cin >> newName;
                    if (std::rename(oldName, newName) == 0)
                    {
                        std::cout << "\nFile renamed successfully.\n";
                        std::strcpy(currentFile, newName);
                    }
                    else
                        std::cout << "\nCannot rename file.\n";
                }
                else if (subChoice == 2)
                {
                    char srcName[100], dstName[100];
                    std::cout << "\nEnter source filename: ";
                    std::cin >> srcName;
                    std::cout << "\nEnter destination filename: ";
                    std::cin >> dstName;
                    FILE *src = fopen(srcName, "rb");
                    if (!src)
                    {
                        std::cout << "\nCannot open source file.\n";
                        continue;
                    }
                    FILE *dst = fopen(dstName, "wb");
                    if (!dst)
                    {
                        fclose(src);
                        std::cout << "\nCannot create destination file.\n";
                        continue;
                    }
                    RentalRecord rec;
                    while (fread(&rec, sizeof(RentalRecord), 1, src) == 1)
                        fwrite(&rec, sizeof(RentalRecord), 1, dst);
                    fclose(src);
                    fclose(dst);
                    std::cout << "\nFile copied successfully.\n";
                }
                else if (subChoice == 3)
                    editing = false;
                else
                    std::cout << "\nInvalid option.\n";
            }
            break;
        }
        case 7:
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
        std::cout << "  8. Find by date\n";
        std::cout << "  9. Statistics by item type\n";
        std::cout << "  10. Add rental record\n";
        std::cout << "  11. Delete rental record by renter\n";
        std::cout << "  12. Delete rental record by index\n";
        std::cout << "  13. Update rental record\n";
        std::cout << "  14. Change file\n";
        std::cout << "  15. Exit\n";
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
            print_all(file);
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
            char name[50];
            std::cout << "\nEnter renter name: ";
            std::cin >> name;
            RentalRecord found = linear_search(file, name);
            if (found.renter_name[0] != '\0')
            {
                std::cout << "\nFound:\n";
                print_record(found);
                FILE *report = fopen("report.txt", "w");
                if (report)
                {
                    write_report_header(report);
                    write_record_to_report(report, found);
                    fclose(report);
                    std::cout << "Report saved to report.txt\n";
                }
            }
            else
                std::cout << "\nNot found.\n";
        }
        else if (opt == 6)
        {
            char name[50];
            std::cout << "\nEnter item name: ";
            std::cin >> name;
            binary_search_and_print(file, name);
        }
        else if (opt == 7)
        {
            char itemName[50], date[20];
            std::cout << "\nEnter item name: ";
            std::cin >> itemName;
            std::cout << "\nEnter date (YYYY-MM-DD): ";
            std::cin >> date;
            find_by_item_and_date(file, itemName, date);
        }
        else if (opt == 8)
        {
            char date[20];
            std::cout << "\nEnter date (YYYY-MM-DD): ";
            std::cin >> date;
            find_by_date(file, date);
        }
        else if (opt == 9)
            statistics(file);
        else if (opt == 10)
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
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }
            append_record(&newRec, file);
            std::cout << "\nRecord added.\n";
        }
        else if (opt == 11)
        {
            char name[50];
            std::cout << "\nEnter renter name to delete: ";
            std::cin >> name;
            delete_records_by_renter(file, currentFile, name);
        }
        else if (opt == 12)
        {
            int idx;
            std::cout << "\nEnter index to delete (0-based): ";
            std::cin >> idx;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }
            delete_record_by_index(file, currentFile, idx);
        }
        else if (opt == 13)
        {
            char name[50];
            std::cout << "\nEnter renter name to update: ";
            std::cin >> name;
            bool found = false;
            fseek(file, 0, SEEK_SET);
            for (long i = 0; i < g_record_count; ++i)
            {
                RentalRecord rec;
                if (fread(&rec, sizeof(RentalRecord), 1, file) != 1)
                    break;
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
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        continue;
                    }
                    write_record_at(i, &rec, file);
                    found = true;
                    std::cout << "\nRecord updated.\n";
                    break;
                }
            }
            if (!found)
                std::cout << "\nNot found.\n";
        }
        else if (opt == 14)
        {
            fclose(file);
            fileOpen = false;
            break;
        }
        else if (opt == 15)
            running = false;
        else
            std::cout << "\nInvalid option.\n";
        std::cout << "\n======================================================================\n";
    }
    if (file)
        fclose(file);
    return 0;
}
