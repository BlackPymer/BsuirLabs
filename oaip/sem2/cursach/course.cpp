#include <iostream>
#include <stdio.h>
#include <io.h>
#include <cstring>
#include <conio.h>

int number_of_structures = 0;
char current_filename[256] = "mas.bin";

#pragma region STRUCTS

struct rent_device
{
    char name[100];
    char type[100];
    int rent_cost;
    char rent_start[11];
    char rent_end[11];
    bool is_free;
};

struct pair
{
    int f;
    int s;

    pair(int first_val = 0, int second_val = 0)
    {
        f = first_val;
        s = second_val;
    }
};

struct node
{
    node *next;
    pair val;

    node(pair value)
    {
        val = value;
        next = nullptr;
    }
};

struct stack
{
    node *first;

    stack() : first(nullptr) {}
    stack(pair value)
    {
        node *nd = new node(value);
        first = nd;
    }
    ~stack() { delete_all(); }
    stack(const stack &) = delete;
    stack &operator=(const stack &) = delete;

    void add(pair value)
    {
        node *nt = new node(value);
        nt->next = first;
        first = nt;
    }

    pair peek()
    {
        return first->val;
    }

    pair pop()
    {
        if (first == nullptr)
        {
            std::cerr << "error! you tried popping from an empty stack!" << std::endl;
            return pair();
        }
        pair value = first->val;
        node *old = first;
        first = first->next;
        delete old;
        return value;
    }

    bool is_empty()
    {
        return first == nullptr;
    }

    void delete_all()
    {
        for (; !is_empty(); pop())
            ;
    }
};

rent_device init_device()
{
    rent_device rt;
    std::cout << "Enter name: ";
    std::cin >> rt.name;
    std::cout << "Enter type: ";
    std::cin >> rt.type;

    std::cout << "Enter daily rent cost: ";
    while (!(std::cin >> rt.rent_cost) || rt.rent_cost <= 0)
    {
        std::cout << "Invalid input. Enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    std::cout << "Enter rent start date (YYYY.MM.DD): ";
    std::cin >> rt.rent_start;
    std::cout << "Enter rent end date (YYYY.MM.DD): ";
    std::cin >> rt.rent_end;

    std::cout << "Enter is_free (0 - free, 1 - rented): ";
    while (!(std::cin >> rt.is_free) || (rt.is_free != 0 && rt.is_free != 1))
    {
        std::cout << "Invalid input. Enter 0 or 1: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    return rt;
}

#pragma endregion

#pragma region BASIC

void add_to_list(rent_device d, FILE *f)
{
    if (f == NULL)
    {
        perror("error opening file");
        return;
    }
    fwrite(&d, sizeof(rent_device), 1, f);
}

void add_new_to_list(FILE *f)
{
    if (f == NULL)
    {
        perror("error opening file");
        return;
    }
    rent_device new_dev;
    std::cout << "\nAdding new equipment\n";
    std::cout << "Enter equipment name: ";
    std::cin.ignore(1000, '\n');
    std::cin.getline(new_dev.name, 100);
    std::cout << "Enter equipment type: ";
    std::cin.getline(new_dev.type, 100);

    std::cout << "Enter daily rent cost: ";
    while (!(std::cin >> new_dev.rent_cost) || new_dev.rent_cost <= 0)
    {
        std::cout << "Invalid input. Enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    std::cout << "Enter rent start date (YYYY.MM.DD): ";
    std::cin >> new_dev.rent_start;
    std::cout << "Enter rent end date (YYYY.MM.DD): ";
    std::cin >> new_dev.rent_end;

    std::cout << "Status (0 - free, 1 - rented): ";
    while (!(std::cin >> new_dev.is_free) || (new_dev.is_free != 0 && new_dev.is_free != 1))
    {
        std::cout << "Invalid input. Enter 0 or 1: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    add_to_list(new_dev, f);
    number_of_structures++;
    std::cout << "Equipment added successfully!\n";
}

void print_list(FILE *f)
{
    rewind(f);
    if (f == NULL)
    {
        perror("error opening file");
        return;
    }
    rent_device tmp;
    int count = 0;
    std::cout << "\nEquipment list:\n";
    printf("%-25s %-20s %8s %11s %11s %10s\n", "Name", "Type", "Cost/day", "Start", "End", "Status");
    while (fread(&tmp, sizeof(rent_device), 1, f) == 1)
    {
        printf("%-25s %-20s %8d %11s %11s %10s\n",
               tmp.name, tmp.type, tmp.rent_cost,
               tmp.rent_start, tmp.rent_end,
               tmp.is_free ? "Rented" : "Free");
        count++;
    }
    std::cout << "Total records: " << count << "\n";
}

rent_device create_new_from_console()
{
    rent_device res;
    std::cout << "Enter name: ";
    std::cin.ignore(1000, '\n');
    std::cin.getline(res.name, 100);

    std::cout << "Enter status (0/1): ";
    while (!(std::cin >> res.is_free) || (res.is_free != 0 && res.is_free != 1))
    {
        std::cout << "Invalid input. Enter 0 or 1: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    std::cout << "Enter rent cost: ";
    while (!(std::cin >> res.rent_cost) || res.rent_cost <= 0)
    {
        std::cout << "Invalid input. Enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    std::cout << "Enter start date (YYYY.MM.DD): ";
    std::cin >> res.rent_start;
    std::cout << "Enter end date (YYYY.MM.DD): ";
    std::cin >> res.rent_end;

    std::cout << "Enter type: ";
    std::cin.ignore(1000, '\n');
    std::cin.getline(res.type, 100);
    return res;
}

void delete_element(FILE *f, char *name)
{
    int n = number_of_structures;
    if (n == 0)
    {
        std::cout << "File is empty!\n";
        return;
    }
    rent_device tmp, s1;
    bool found = false;
    for (int i = 0; i < n; i++)
    {
        fseek(f, sizeof(rent_device) * i, SEEK_SET);
        fread(&tmp, sizeof(rent_device), 1, f);
        if (strcmp(tmp.name, name) == 0)
        {
            found = true;
            for (int j = i; j < n - 1; j++)
            {
                fseek(f, sizeof(rent_device) * (j + 1), SEEK_SET);
                fread(&s1, sizeof(rent_device), 1, f);
                fseek(f, sizeof(rent_device) * j, SEEK_SET);
                fwrite(&s1, sizeof(rent_device), 1, f);
            }
            fflush(f);
            n--;
            number_of_structures--;
            i--;
        }
    }
    if (found)
    {
        chsize(fileno(f), (n) * sizeof(rent_device));
        std::cout << "Record deleted!\n";
    }
    else
    {
        std::cout << "Equipment with this name not found!\n";
    }
}

void edit_element(FILE *f, char *name)
{
    if (f == NULL)
    {
        perror("error opening file");
        return;
    }
    int n = number_of_structures;
    rent_device current;
    bool found = false;
    fseek(f, 0, SEEK_SET);
    for (int i = 0; i < n; i++)
    {
        fread(&current, sizeof(rent_device), 1, f);
        if (strcmp(current.name, name) == 0)
        {
            found = true;
            std::cout << "\nEditing: " << current.name << "\n";
            std::cout << "Fields available for editing:\n";
            std::cout << "1. Name\n2. Type\n3. Rent Cost\n4. Start Date (YYYY.MM.DD)\n5. End Date (YYYY.MM.DD)\n6. Status (0 - free, 1 - rented)\n";
            std::cout << "Enter field numbers consecutively (e.g., 135): ";

            char choices[32];
            std::cin >> choices;
            std::cin.ignore(1000, '\n');

            for (int k = 0; choices[k] != '\0'; ++k)
            {
                char c = choices[k];
                if (c < '1' || c > '6')
                    continue;
                int field = c - '0';

                switch (field)
                {
                case 1:
                    std::cout << "New Name: ";
                    std::cin.getline(current.name, 100);
                    break;
                case 2:
                    std::cout << "New Type: ";
                    std::cin.getline(current.type, 100);
                    break;
                case 3:
                    std::cout << "New Rent Cost: ";
                    while (!(std::cin >> current.rent_cost) || current.rent_cost <= 0)
                    {
                        std::cout << "Invalid. Enter a positive number: ";
                        std::cin.clear();
                        std::cin.ignore(1000, '\n');
                    }
                    std::cin.ignore(1000, '\n');
                    break;
                case 4:
                    std::cout << "New Start Date (YYYY.MM.DD): ";
                    std::cin >> current.rent_start;
                    std::cin.ignore(1000, '\n');
                    break;
                case 5:
                    std::cout << "New End Date (YYYY.MM.DD): ";
                    std::cin >> current.rent_end;
                    std::cin.ignore(1000, '\n');
                    break;
                case 6:
                {
                    std::cout << "New Status (0/1): ";
                    int temp;
                    while (!(std::cin >> temp) || (temp != 0 && temp != 1))
                    {
                        std::cout << "Invalid. Enter 0 or 1: ";
                        std::cin.clear();
                        std::cin.ignore(1000, '\n');
                    }
                    current.is_free = (temp == 1);
                    std::cin.ignore(1000, '\n');
                    break;
                }
                }
            }

            fseek(f, sizeof(rent_device) * i, SEEK_SET);
            fwrite(&current, sizeof(rent_device), 1, f);
            std::cout << "Record updated successfully!\n";
            return;
        }
    }
    if (!found)
    {
        std::cout << "Equipment with this name not found!\n";
    }
}

void type_sort(FILE *f)
{
    int n = number_of_structures;
    rent_device s1, s2;

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            long pos1 = j * sizeof(rent_device);
            long pos2 = (j + 1) * sizeof(rent_device);

            fseek(f, pos1, SEEK_SET);
            fread(&s1, sizeof(rent_device), 1, f);
            fread(&s2, sizeof(rent_device), 1, f);
            if (strcmp(s1.type, s2.type) > 0)
            {
                fseek(f, pos1, SEEK_SET);
                fwrite(&s2, sizeof(rent_device), 1, f);
                fwrite(&s1, sizeof(rent_device), 1, f);
            }
        }
    }
    std::cout << "Sort by type completed!\n";
}

void date_quick_sort(FILE *f)
{
    if (f == NULL)
    {
        perror("error opening file");
        return;
    }

    stack st;
    int n = number_of_structures;
    int len = n;
    int i, j;
    rent_device a, b, mid;
    st.add(pair(0, len));
    while (!st.is_empty())
    {
        pair pr = st.pop();
        if (pr.f == pr.s)
            continue;
        if (pr.f < (pr.s - 1))
        {
            fseek(f, (pr.f + pr.s) / 2 * sizeof(rent_device), SEEK_SET);
            fread(&mid, sizeof(rent_device), 1, f);
            i = pr.f;
            j = pr.s - 1;
            while (i <= j)
            {
                fseek(f, i * sizeof(rent_device), SEEK_SET);
                fread(&a, sizeof(rent_device), 1, f);
                while (strcmp(a.rent_start, mid.rent_start) < 0 && i < pr.s - 1)
                {
                    i++;
                    fseek(f, i * sizeof(rent_device), SEEK_SET);
                    fread(&a, sizeof(rent_device), 1, f);
                }
                fseek(f, j * sizeof(rent_device), SEEK_SET);
                fread(&b, sizeof(rent_device), 1, f);
                while (strcmp(b.rent_start, mid.rent_start) > 0 && j > pr.f)
                {
                    j--;
                    fseek(f, j * sizeof(rent_device), SEEK_SET);
                    fread(&b, sizeof(rent_device), 1, f);
                }
                if (i <= j)
                {
                    fseek(f, j * sizeof(rent_device), SEEK_SET);
                    fwrite(&a, sizeof(rent_device), 1, f);
                    fseek(f, i * sizeof(rent_device), SEEK_SET);
                    fwrite(&b, sizeof(rent_device), 1, f);
                    j--;
                    i++;
                }
            }
            if (pr.f < j)
                st.add(pair(pr.f, j + 1));
            if (i < pr.s - 1)
                st.add(pair(i, pr.s));
        }
    }
    std::cout << "Sort by date completed!\n";
}

void cost_insert_sort(FILE *f)
{
    fseek(f, 0, SEEK_END);
    int n = ftell(f) / sizeof(rent_device);
    rent_device s1, s2, tmp;

    for (int i = 1; i < n; i++)
    {
        fseek(f, i * sizeof(rent_device), SEEK_SET);
        fread(&s1, sizeof(rent_device), 1, f);

        int j = i - 1;
        fseek(f, j * sizeof(rent_device), SEEK_SET);
        fread(&tmp, sizeof(rent_device), 1, f);

        while (j >= 0 && tmp.rent_cost > s1.rent_cost)
        {
            fseek(f, (j + 1) * sizeof(rent_device), SEEK_SET);
            fwrite(&tmp, sizeof(rent_device), 1, f);
            j--;

            if (j >= 0)
            {
                fseek(f, j * sizeof(rent_device), SEEK_SET);
                fread(&tmp, sizeof(rent_device), 1, f);
            }
        }
        fseek(f, (j + 1) * sizeof(rent_device), SEEK_SET);
        fwrite(&s1, sizeof(rent_device), 1, f);
    }
    std::cout << "Sort by cost completed!\n";
}

bool name_search(char *nam, FILE *f)
{
    if (f == NULL)
    {
        perror("error opening file");
        return 0;
    }
    rewind(f);
    rent_device tmp;
    bool found = false;
    std::cout << "\nSearch results by name:\n";
    while (fread(&tmp, sizeof(rent_device), 1, f) == 1)
    {
        if (strstr(tmp.name, nam) != NULL || strcmp(nam, tmp.name) == 0)
        {
            printf("Name: %s\nType: %s\nCost: %d\nStart date: %s\nEnd date: %s\nStatus: %s\n\n",
                   tmp.name, tmp.type, tmp.rent_cost,
                   tmp.rent_start, tmp.rent_end,
                   tmp.is_free ? "Rented" : "Free");
            found = true;
        }
    }
    if (!found)
    {
        std::cout << "Not found!\n";
    }
    return found;
}

void print_adjacent(FILE *f, int i, int n)
{
    rent_device tmp;
    fseek(f, i * sizeof(rent_device), SEEK_SET);
    fread(&tmp, sizeof(rent_device), 1, f);
    int target_cost = tmp.rent_cost;
    int start = i;
    while (start > 0)
    {
        fseek(f, (start - 1) * sizeof(rent_device), SEEK_SET);
        fread(&tmp, sizeof(rent_device), 1, f);
        if (tmp.rent_cost != target_cost)
            break;
        start--;
    }

    fseek(f, start * sizeof(rent_device), SEEK_SET);
    std::cout << "\nEquipment with cost: " << target_cost << "\n";
    for (int j = start; j <= n - 1; j++)
    {
        if (fread(&tmp, sizeof(rent_device), 1, f) != 1)
            break;
        if (tmp.rent_cost != target_cost)
            break;
        printf("Name: %s\nType: %s\nCost: %d\nStatus: %s\n\n",
               tmp.name, tmp.type, tmp.rent_cost,
               tmp.is_free ? "Rented" : "Free");
    }
}

bool cost_interpolation_search(int cost, FILE *f)
{
    if (f == NULL)
    {
        perror("error opening file");
        return false;
    }

    cost_insert_sort(f);
    std::cout << "(File sorted by cost for interpolation search)\n";

    rewind(f);
    int low = 0;
    int high = number_of_structures - 1;
    if (high < 0)
        return false;
    int n = high;
    rent_device s1, s2, s3;

    fseek(f, low * sizeof(rent_device), SEEK_SET);
    fread(&s1, sizeof(rent_device), 1, f);
    fseek(f, high * sizeof(rent_device), SEEK_SET);
    fread(&s2, sizeof(rent_device), 1, f);

    while (low <= high && s1.rent_cost <= cost && s2.rent_cost >= cost)
    {
        if (s1.rent_cost == s2.rent_cost)
            break;

        int mid = low + ((cost - s1.rent_cost) * (high - low) / (s2.rent_cost - s1.rent_cost));
        if (mid < low)
            mid = low;
        if (mid > high)
            mid = high;

        fseek(f, mid * sizeof(rent_device), SEEK_SET);
        fread(&s3, sizeof(rent_device), 1, f);

        if (s3.rent_cost < cost)
        {
            low = mid + 1;
        }
        else if (s3.rent_cost > cost)
        {
            high = mid - 1;
        }
        else
        {
            print_adjacent(f, mid, n);
            return true;
        }

        fseek(f, low * sizeof(rent_device), SEEK_SET);
        fread(&s1, sizeof(rent_device), 1, f);
        fseek(f, high * sizeof(rent_device), SEEK_SET);
        fread(&s2, sizeof(rent_device), 1, f);
    }

    if (s1.rent_cost == cost)
    {
        print_adjacent(f, low, n);
        return true;
    }
    if (s2.rent_cost == cost)
    {
        print_adjacent(f, high, n);
        return true;
    }
    std::cout << "Not found!\n";
    return false;
}

void print_list_free(FILE *f)
{
    FILE *tmpf = fopen("tmp.bin", "wb+");
    rewind(f);
    if (f == NULL || tmpf == NULL)
    {
        perror("error opening file");
        return;
    }

    char start[11];
    char end[11];
    rent_device tmp;

    std::cout << "\nSearch for available equipment\n";
    std::cout << "Enter period start (YYYY.MM.DD): ";
    std::cin >> start;
    std::cout << "Enter period end (YYYY.MM.DD): ";
    std::cin >> end;

    int count = 0;
    while (fread(&tmp, sizeof(rent_device), 1, f) == 1)
    {
        if (tmp.is_free || strcmp(tmp.rent_end, start) < 0 || strcmp(tmp.rent_start, end) > 0)
        {
            fwrite(&tmp, sizeof(rent_device), 1, tmpf);
            count++;
        }
    }

    if (count == 0)
    {
        std::cout << "No equipment available for the specified period.\n";
        fclose(tmpf);
        remove("tmp.bin");
        return;
    }

    fflush(tmpf);
    cost_insert_sort(tmpf);

    std::cout << "\nAvailable equipment (sorted by cost):\n";
    print_list(tmpf);
    fclose(tmpf);
    remove("tmp.bin");
}

void print_type_statistics(FILE *f, bool save_to_file)
{
    if (f == NULL)
        return;

    FILE *tmpf = fopen("stat_tmp.bin", "wb+");
    rewind(f);
    rent_device rd;
    while (fread(&rd, sizeof(rent_device), 1, f) == 1)
    {
        fwrite(&rd, sizeof(rent_device), 1, tmpf);
    }

    fseek(tmpf, 0, SEEK_END);
    int n = ftell(tmpf) / sizeof(rent_device);

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            rent_device s1, s2;
            fseek(tmpf, j * sizeof(rent_device), SEEK_SET);
            fread(&s1, sizeof(rent_device), 1, tmpf);
            fread(&s2, sizeof(rent_device), 1, tmpf);

            bool swap_needed = false;
            int type_cmp = strcmp(s1.type, s2.type);
            if (type_cmp > 0)
            {
                swap_needed = true;
            }
            else if (type_cmp == 0 && strcmp(s1.rent_start, s2.rent_start) < 0)
            {
                swap_needed = true;
            }

            if (swap_needed)
            {
                fseek(tmpf, j * sizeof(rent_device), SEEK_SET);
                fwrite(&s2, sizeof(rent_device), 1, tmpf);
                fwrite(&s1, sizeof(rent_device), 1, tmpf);
            }
        }
    }

    FILE *stat_txt = NULL;
    if (save_to_file)
    {
        stat_txt = fopen("stat.txt", "w");
        if (stat_txt == NULL)
        {
            std::cout << "Error creating stat.txt\n";
            save_to_file = false;
        }
    }

    rewind(tmpf);

    char current_type[100] = "";
    long long type_total = 0;
    bool first = true;

    std::cout << "\nEQUIPMENT TYPE STATISTICS\n";
    if (save_to_file)
        fprintf(stat_txt, "EQUIPMENT TYPE STATISTICS\n");

    while (fread(&rd, sizeof(rent_device), 1, tmpf) == 1)
    {
        if (strcmp(rd.type, current_type) != 0)
        {
            if (!first)
            {
                std::cout << "\nTotal rent payments for type '" << current_type << "': " << type_total << " rub.\n";
                if (save_to_file)
                    fprintf(stat_txt, "\nTotal rent payments for type '%s': %lld rub.\n", current_type, type_total);
            }
            strcpy(current_type, rd.type);
            type_total = 0;
            std::cout << "\nTYPE: " << current_type << "\n";
            if (save_to_file)
                fprintf(stat_txt, "\nTYPE: %s\n", current_type);
            first = false;
        }
        std::cout << "  - " << rd.name << " | Start: " << rd.rent_start << " | Cost: " << rd.rent_cost << " rub./day\n";
        if (save_to_file)
            fprintf(stat_txt, "  - %s | Start: %s | Cost: %d rub./day\n", rd.name, rd.rent_start, rd.rent_cost);
        type_total += rd.rent_cost;
    }
    if (!first)
    {
        std::cout << "\nTotal rent payments for type '" << current_type << "': " << type_total << " rub.\n";
        if (save_to_file)
            fprintf(stat_txt, "\nTotal rent payments for type '%s': %lld rub.\n", current_type, type_total);
    }

    if (save_to_file)
    {
        fclose(stat_txt);
        std::cout << "\nStatistics saved to stat.txt\n";
    }

    fclose(tmpf);
    remove("stat_tmp.bin");
}

void show_menu()
{
    std::cout << "\nMENU\n";
    std::cout << "1. Show all equipment\n";
    std::cout << "2. Add equipment\n";
    std::cout << "3. Delete equipment by name\n";
    std::cout << "4. Edit equipment\n";
    std::cout << "5. Search by name\n";
    std::cout << "6. Search by cost (interpolation)\n";
    std::cout << "7. Sort by type (bubble)\n";
    std::cout << "8. Sort by start date (quick)\n";
    std::cout << "9. Sort by cost (insertion)\n";
    std::cout << "10. Search available equipment for period\n";
    std::cout << "11. Statistics by equipment type\n";
    std::cout << "12. Change file\n";
    std::cout << "0. Exit\n";
    std::cout << "> ";
}

void clear_file(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (f)
        fclose(f);
}

bool open_file(FILE *&f, const char *filename, bool create_new)
{
    if (create_new)
    {
        clear_file(filename);
        f = fopen(filename, "rb+");
        if (f)
        {
            std::cout << "Created new file: " << filename << "\n";
            number_of_structures = 0;
            return true;
        }
    }
    else
    {
        f = fopen(filename, "rb+");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            number_of_structures = ftell(f) / sizeof(rent_device);
            rewind(f);
            std::cout << "Opened file: " << filename << " (records: " << number_of_structures << ")\n";
            return true;
        }
    }
    std::cout << "Error opening file!\n";
    return false;
}

int main()
{
    FILE *f = NULL;
    int choice;
    char search_name[100];
    int search_cost;
    char new_filename[256];

    std::cout << "EQUIPMENT RENTAL ACCOUNTING SYSTEM\n\n";

    int file_choice;
    std::cout << "Select action:\n";
    std::cout << "1. Create new file\n";
    std::cout << "2. Open existing file\n";
    std::cout << "> ";
    std::cin >> file_choice;

    if (file_choice == 1)
    {
        std::cout << "Enter new filename (or press Enter for mas.bin): ";
        std::cin.ignore();
        std::cin.getline(new_filename, 256);
        if (strlen(new_filename) == 0)
        {
            strcpy(new_filename, "mas.bin");
        }
        strcpy(current_filename, new_filename);
        if (!open_file(f, current_filename, true))
        {
            return 1;
        }
    }
    else
    {
        std::cout << "Enter filename (or press Enter for mas.bin): ";
        std::cin.ignore();
        std::cin.getline(new_filename, 256);
        if (strlen(new_filename) == 0)
        {
            strcpy(new_filename, "mas.bin");
        }
        strcpy(current_filename, new_filename);
        if (!open_file(f, current_filename, false))
        {
            std::cout << "File not found. Create new? (1 - yes, 0 - no): ";
            std::cin >> file_choice;
            if (file_choice)
            {
                if (!open_file(f, current_filename, true))
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }
    }

    do
    {
        show_menu();
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            print_list(f);
            break;
        case 2:
            add_new_to_list(f);
            break;
        case 3:
            std::cout << "Enter equipment name to delete: ";
            std::cin.ignore();
            std::cin.getline(search_name, 100);
            delete_element(f, search_name);
            break;
        case 4:
            std::cout << "Enter equipment name to edit: ";
            std::cin.ignore();
            std::cin.getline(search_name, 100);
            edit_element(f, search_name);
            break;
        case 5:
            std::cout << "Enter name to search: ";
            std::cin.ignore();
            std::cin.getline(search_name, 100);
            name_search(search_name, f);
            break;
        case 6:
            std::cout << "Enter cost to search: ";
            std::cin >> search_cost;
            cost_interpolation_search(search_cost, f);
            break;
        case 7:
            type_sort(f);
            break;
        case 8:
            date_quick_sort(f);
            break;
        case 9:
            cost_insert_sort(f);
            break;
        case 10:
            print_list_free(f);
            break;
        case 11:
            int save_choice;
            std::cout << "Save statistics to stat.txt? (1 - yes, 0 - no): ";
            std::cin >> save_choice;
            print_type_statistics(f, save_choice == 1);
            break;
        case 12:
            fclose(f);
            std::cout << "Enter filename: ";
            std::cin.ignore();
            std::cin.getline(new_filename, 256);
            strcpy(current_filename, new_filename);
            if (!open_file(f, current_filename, false))
            {
                std::cout << "File not found. Create new? (1 - yes, 0 - no): ";
                std::cin >> file_choice;
                if (file_choice)
                {
                    if (!open_file(f, current_filename, true))
                    {
                        return 1;
                    }
                }
                else
                {
                    if (!open_file(f, current_filename, false))
                    {
                        return 1;
                    }
                }
            }
            break;
        case 0:
            std::cout << "Exiting program...\n";
            break;
        default:
            std::cout << "Invalid choice! Try again.\n";
        }

        if (choice != 0)
        {
            std::cout << "\nPress any key to continue...";
            _getch();
        }

    } while (choice != 0);

    fclose(f);
    std::cout << "Work completed. Goodbye!\n";
    return 0;
}