#include <iostream>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <clocale>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

using namespace std;

const int STR_SIZE = 100;

struct Visit
{
    char fio[STR_SIZE];
    char membershipType[STR_SIZE];
    char visitDate[11];
    char visitTime[6];
    int duration;
};

long g_record_count = 0;

bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isValidDateFormat(const char date[])
{
    if (strlen(date) != 10)
        return false;
    if (date[2] != '.' || date[5] != '.')
        return false;
    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
            continue;
        if (date[i] < '0' || date[i] > '9')
            return false;
    }
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
    if (month < 1 || month > 12)
        return false;
    if (day < 1)
        return false;
    int daysInMonth;
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        daysInMonth = 31;
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        daysInMonth = 30;
        break;
    case 2:
        daysInMonth = isLeapYear(year) ? 29 : 28;
        break;
    default:
        return false;
    }
    return day <= daysInMonth;
}

bool isValidTimeFormat(const char time[])
{
    if (strlen(time) != 5)
        return false;
    if (time[2] != ':')
        return false;
    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
            continue;
        if (time[i] < '0' || time[i] > '9')
            return false;
    }
    int hour = (time[0] - '0') * 10 + (time[1] - '0');
    int minute = (time[3] - '0') * 10 + (time[4] - '0');
    return !(hour < 0 || hour > 23 || minute < 0 || minute > 59);
}

bool isValidPositiveNumberLine(const char line[])
{
    if (strlen(line) == 0)
        return false;
    for (int i = 0; line[i] != '\0'; i++)
        if (line[i] < '0' || line[i] > '9')
            return false;
    return true;
}

int dateToNumber(const char date[])
{
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
    return year * 10000 + month * 100 + day;
}

int timeToNumber(const char time[])
{
    int hour = (time[0] - '0') * 10 + (time[1] - '0');
    int minute = (time[3] - '0') * 10 + (time[4] - '0');
    return hour * 100 + minute;
}

long get_record_count(FILE *file)
{
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    return size / sizeof(Visit);
}

bool read_record(FILE *file, int index, Visit &v)
{
    if (index < 0 || index >= g_record_count)
        return false;
    fseek(file, sizeof(Visit) * index, SEEK_SET);
    return fread(&v, sizeof(Visit), 1, file) == 1;
}

bool write_record(FILE *file, int index, const Visit &v)
{
    if (index < 0 || index > g_record_count)
        return false;
    fseek(file, sizeof(Visit) * index, SEEK_SET);
    if (fwrite(&v, sizeof(Visit), 1, file) != 1)
        return false;
    fflush(file);
    if (index == g_record_count)
        g_record_count = get_record_count(file);
    return true;
}

bool append_record(FILE *file, const Visit &v)
{
    fseek(file, 0, SEEK_END);
    if (fwrite(&v, sizeof(Visit), 1, file) != 1)
        return false;
    fflush(file);
    g_record_count = get_record_count(file);
    return true;
}

void delete_last_record(FILE *file, const char *filename)
{
    if (g_record_count == 0)
        return;
    long new_size = sizeof(Visit) * (g_record_count - 1);
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
    g_record_count = get_record_count(file);
}

void create_empty_file(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (f)
        fclose(f);
    g_record_count = 0;
}

bool file_exists(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f)
    {
        fclose(f);
        return true;
    }
    return false;
}

void swap_records(FILE *file, int i, int j)
{
    if (i == j)
        return;
    Visit a, b;
    read_record(file, i, a);
    read_record(file, j, b);
    write_record(file, i, b);
    write_record(file, j, a);
}

struct StackNode
{
    int l, r;
    StackNode *next;
    StackNode(int left = 0, int right = 0) : l(left), r(right), next(nullptr) {}
};

void pushStack(StackNode *&root, int l, int r)
{
    StackNode *node = new StackNode(l, r);
    node->next = root;
    root = node;
}

bool popStack(StackNode *&root, int &l, int &r)
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

bool isStackEmpty(StackNode *root)
{
    return root == nullptr;
}

void clearStack(StackNode *&root)
{
    int l, r;
    while (popStack(root, l, r))
        ;
}

void printVisit(const Visit &v, int index)
{
    cout << "\nЗапись #" << index + 1 << endl;
    cout << "ФИО: " << v.fio << endl;
    cout << "Тип абонемента: " << v.membershipType << endl;
    cout << "Дата посещения: " << v.visitDate << endl;
    cout << "Время посещения: " << v.visitTime << endl;
    cout << "Длительность тренировки: " << v.duration << " мин" << endl;
}

void printVisitToFile(FILE *out, const Visit &v, int index)
{
    fprintf(out, "\nЗапись #%d\n", index + 1);
    fprintf(out, "ФИО: %s\n", v.fio);
    fprintf(out, "Тип абонемента: %s\n", v.membershipType);
    fprintf(out, "Дата посещения: %s\n", v.visitDate);
    fprintf(out, "Время посещения: %s\n", v.visitTime);
    fprintf(out, "Длительность тренировки: %d мин\n", v.duration);
}

void inputVisit(Visit &v)
{
    do
    {
        cout << "Введите ФИО: ";
        cin.getline(v.fio, STR_SIZE);
        if (strlen(v.fio) == 0)
            cout << "Ошибка: ФИО не может быть пустым.\n";
    } while (strlen(v.fio) == 0);

    do
    {
        cout << "Введите тип абонемента: ";
        cin.getline(v.membershipType, STR_SIZE);
        if (strlen(v.membershipType) == 0)
            cout << "Ошибка: тип абонемента не может быть пустым.\n";
    } while (strlen(v.membershipType) == 0);

    do
    {
        cout << "Введите дату посещения (dd.mm.yyyy): ";
        cin.getline(v.visitDate, 11);
        if (!isValidDateFormat(v.visitDate))
            cout << "Ошибка: неверная дата.\n";
    } while (!isValidDateFormat(v.visitDate));

    do
    {
        cout << "Введите время посещения (hh:mm): ";
        cin.getline(v.visitTime, 6);
        if (!isValidTimeFormat(v.visitTime))
            cout << "Ошибка: неверный формат времени.\n";
    } while (!isValidTimeFormat(v.visitTime));

    while (true)
    {
        cout << "Введите длительность тренировки (в минутах): ";
        cin >> v.duration;
        if (cin.fail() || v.duration <= 0)
        {
            cout << "Ошибка: длительность должна быть положительным числом.\n";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        else
        {
            cin.ignore(1000, '\n');
            break;
        }
    }
}

int inputRecordIndex(FILE *file, const char *actionName)
{
    int n = g_record_count;
    if (n == 0)
    {
        cout << "\nФайл пуст.\n";
        return -1;
    }
    int recordNumber;
    while (true)
    {
        cout << "Введите номер записи для " << actionName << " (от 1 до " << n << "): ";
        cin >> recordNumber;
        if (cin.fail() || recordNumber < 1 || recordNumber > n)
        {
            cout << "Ошибка: введите номер от 1 до " << n << ".\n";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        else
        {
            cin.ignore(1000, '\n');
            return recordNumber - 1;
        }
    }
}

void showAllRecords(FILE *file)
{
    rewind(file);
    for (int i = 0; i < g_record_count; i++)
    {
        Visit v;
        if (fread(&v, sizeof(Visit), 1, file) == 1)
            printVisit(v, i);
    }
}

void showAllRecordsToFile(FILE *file, FILE *out)
{
    rewind(file);
    for (int i = 0; i < g_record_count; i++)
    {
        Visit v;
        if (fread(&v, sizeof(Visit), 1, file) == 1)
            printVisitToFile(out, v, i);
    }
}

void linearSearchByFio(FILE *file)
{
    if (g_record_count == 0)
    {
        cout << "\nФайл пуст.\n";
        return;
    }
    char target[STR_SIZE];
    cout << "Введите ФИО для поиска: ";
    cin.getline(target, STR_SIZE);

    rewind(file);
    bool found = false;
    for (int i = 0; i < g_record_count; i++)
    {
        Visit v;
        fread(&v, sizeof(Visit), 1, file);
        if (strcmp(v.fio, target) == 0)
        {
            printVisit(v, i);
            found = true;
        }
    }
    if (!found)
        cout << "\nПосетитель с таким ФИО не найден.\n";
}

void editRecordByIndex(FILE *file)
{
    int index = inputRecordIndex(file, "редактирования");
    if (index == -1)
        return;

    Visit record;
    if (!read_record(file, index, record))
        return;

    cout << "\nТекущие данные записи:\n";
    printVisit(record, index);

    cout << "\nКакое поле изменить?\n";
    cout << "1 - ФИО\n2 - Тип абонемента\n3 - Дату посещения\n4 - Время посещения\n5 - Длительность тренировки\nВыберите пункт: ";
    int field;
    cin >> field;
    cin.ignore(1000, '\n');

    switch (field)
    {
    case 1:
    {
        do
        {
            cout << "Введите новое ФИО: ";
            cin.getline(record.fio, STR_SIZE);
            if (strlen(record.fio) == 0)
                cout << "Ошибка: ФИО не может быть пустым.\n";
        } while (strlen(record.fio) == 0);
        break;
    }
    case 2:
    {
        do
        {
            cout << "Введите новый тип абонемента: ";
            cin.getline(record.membershipType, STR_SIZE);
            if (strlen(record.membershipType) == 0)
                cout << "Ошибка: тип абонемента не может быть пустым.\n";
        } while (strlen(record.membershipType) == 0);
        break;
    }
    case 3:
    {
        do
        {
            cout << "Введите новую дату посещения (dd.mm.yyyy): ";
            cin.getline(record.visitDate, 11);
            if (!isValidDateFormat(record.visitDate))
                cout << "Ошибка: неверная дата.\n";
        } while (!isValidDateFormat(record.visitDate));
        break;
    }
    case 4:
    {
        do
        {
            cout << "Введите новое время посещения (hh:mm): ";
            cin.getline(record.visitTime, 6);
            if (!isValidTimeFormat(record.visitTime))
                cout << "Ошибка: неверный формат времени.\n";
        } while (!isValidTimeFormat(record.visitTime));
        break;
    }
    case 5:
    {
        while (true)
        {
            cout << "Введите новую длительность тренировки (в минутах): ";
            cin >> record.duration;
            if (cin.fail() || record.duration <= 0)
            {
                cout << "Ошибка: длительность должна быть положительным числом.\n";
                cin.clear();
                cin.ignore(1000, '\n');
            }
            else
            {
                cin.ignore(1000, '\n');
                break;
            }
        }
        break;
    }
    default:
        cout << "\nНеверный пункт, редактирование отменено.\n";
        return;
    }

    if (write_record(file, index, record))
        cout << "\nЗапись успешно отредактирована.\n";
    else
        cout << "\nОшибка редактирования записи.\n";
}

void deleteRecordByIndex(FILE *&file, const char *filename)
{
    int index = inputRecordIndex(file, "удаления");
    if (index == -1)
        return;

    int n = g_record_count;
    Visit deleted;
    read_record(file, index, deleted);
    cout << "\nБудет удалена запись:\n";
    printVisit(deleted, index);

    for (int i = index; i < n - 1; i++)
    {
        Visit next;
        read_record(file, i + 1, next);
        write_record(file, i, next);
    }

    fclose(file);
    long new_size = sizeof(Visit) * (g_record_count - 1);
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
    file = fopen(filename, "r+b");
    if (file)
        g_record_count = get_record_count(file);
    cout << "\nЗапись успешно удалена.\n";
}

void sortByDurationAscending(FILE *file)
{
    int n = g_record_count;
    if (n <= 1)
        return;
    for (int i = 0; i < n - 1; i++)
    {
        int minIdx = i;
        Visit minRec;
        read_record(file, minIdx, minRec);
        fseek(file, sizeof(Visit) * (i + 1), SEEK_SET);
        for (int j = i + 1; j < n; j++)
        {
            Visit cur;
            fread(&cur, sizeof(Visit), 1, file);
            if (cur.duration < minRec.duration)
            {
                minIdx = j;
                minRec = cur;
            }
        }
        if (minIdx != i)
            swap_records(file, i, minIdx);
    }
}

void sortByDurationDescending(FILE *file)
{
    int n = g_record_count;
    if (n <= 1)
        return;
    for (int i = 0; i < n - 1; i++)
    {
        int maxIdx = i;
        Visit maxRec;
        read_record(file, maxIdx, maxRec);
        fseek(file, sizeof(Visit) * (i + 1), SEEK_SET);
        for (int j = i + 1; j < n; j++)
        {
            Visit cur;
            fread(&cur, sizeof(Visit), 1, file);
            if (cur.duration > maxRec.duration)
            {
                maxIdx = j;
                maxRec = cur;
            }
        }
        if (maxIdx != i)
            swap_records(file, i, maxIdx);
    }
}

int binarySearchByDuration(FILE *file, int targetDuration)
{
    sortByDurationAscending(file);
    int left = 0, right = g_record_count - 1;
    while (left <= right)
    {
        int mid = (left + right) / 2;
        Visit v;
        read_record(file, mid, v);
        if (v.duration == targetDuration)
            return mid;
        else if (v.duration < targetDuration)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

void printAllSameDuration(FILE *file, int index)
{
    if (index == -1)
    {
        cout << "\nЗаписи с такой длительностью не найдены.\n";
        return;
    }
    int n = g_record_count;
    Visit center;
    read_record(file, index, center);
    int target = center.duration;

    int left = index;
    while (left - 1 >= 0)
    {
        Visit prev;
        read_record(file, left - 1, prev);
        if (prev.duration == target)
            left--;
        else
            break;
    }
    int right = index;
    fseek(file, right * sizeof(Visit), SEEK_SET);
    while (right + 1 < n)
    {
        Visit next;
        fread(&next, sizeof(Visit), 1, file);
        if (next.duration == target)
            right++;
        else
            break;
    }
    cout << "\nНайдены записи с длительностью " << target << " мин:\n";
    fseek(file, left * sizeof(Visit), SEEK_SET);
    for (int i = left; i <= right; i++)
    {
        Visit v;
        fread(&v, sizeof(Visit), 0, file);
        printVisit(v, i);
    }
}

void performBinarySearchByDuration(FILE *file)
{
    int target;
    cout << "Введите длительность тренировки для поиска: ";
    cin >> target;
    cin.ignore(1000, '\n');
    int idx = binarySearchByDuration(file, target);
    printAllSameDuration(file, idx);
}

void quickSortMembership(FILE *file)
{
    int n = g_record_count;
    if (n <= 1)
        return;
    StackNode *stackRoot = nullptr;
    pushStack(stackRoot, 0, n - 1);
    while (!isStackEmpty(stackRoot))
    {
        int left, right;
        popStack(stackRoot, left, right);
        int mid = (left + right) / 2;
        Visit pivot;
        read_record(file, mid, pivot);
        char pivotStr[STR_SIZE];
        strcpy(pivotStr, pivot.membershipType);

        int i = left, j = right;
        while (i <= j)
        {
            fseek(file, 0, SEEK_SET);
            while (i <= right)
            {
                Visit cur;
                fread(&cur, sizeof(Visit), 1, file);
                if (strcmp(cur.membershipType, pivotStr) < 0)
                    i++;
                else
                    break;
            }
            while (j >= left)
            {
                Visit cur;
                read_record(file, j, cur);
                if (strcmp(cur.membershipType, pivotStr) > 0)
                    j--;
                else
                    break;
            }
            if (i <= j)
            {
                swap_records(file, i, j);
                i++;
                j--;
            }
        }
        if (left < j)
            pushStack(stackRoot, left, j);
        if (i < right)
            pushStack(stackRoot, i, right);
    }
    clearStack(stackRoot);
    cout << "\nФайл отсортирован по типу абонемента.\n";
}

void selectionSortByDate(FILE *file)
{
    int n = g_record_count;
    if (n <= 1)
        return;
    for (int i = 0; i < n - 1; i++)
    {
        int minIdx = i;
        Visit minRec;
        read_record(file, minIdx, minRec);
        fseek(file, (i + 1) * sizeof(Visit), SEEK_SET);
        for (int j = i + 1; j < n; j++)
        {
            Visit cur;
            fread(&cur, sizeof(Visit), 1, file);
            if (dateToNumber(cur.visitDate) < dateToNumber(minRec.visitDate))
            {
                minIdx = j;
                minRec = cur;
            }
        }
        if (minIdx != i)
            swap_records(file, i, minIdx);
    }
}

void insertionSortByTime(FILE *file)
{
    int n = g_record_count;
    if (n <= 1)
        return;
    for (int i = 1; i < n; i++)
    {
        Visit key;
        read_record(file, i, key);
        int j = i - 1;
        while (j >= 0)
        {
            Visit cur;
            read_record(file, j, cur);
            if (timeToNumber(cur.visitTime) > timeToNumber(key.visitTime))
            {
                write_record(file, j + 1, cur);
                j--;
            }
            else
                break;
        }
        write_record(file, j + 1, key);
    }
}

void analyzeVisitsByPeriod(FILE *file)
{
    int n = g_record_count;
    if (n == 0)
    {
        cout << "\nФайл пуст.\n";
        return;
    }
    char startDate[11], endDate[11];
    do
    {
        cout << "Введите начальную дату периода (dd.mm.yyyy): ";
        cin.getline(startDate, 11);
        if (!isValidDateFormat(startDate))
            cout << "Ошибка: неверный формат начальной даты.\n";
    } while (!isValidDateFormat(startDate));
    do
    {
        cout << "Введите конечную дату периода (dd.mm.yyyy): ";
        cin.getline(endDate, 11);
        if (!isValidDateFormat(endDate))
            cout << "Ошибка: неверный формат конечной даты.\n";
    } while (!isValidDateFormat(endDate));

    int startNum = dateToNumber(startDate);
    int endNum = dateToNumber(endDate);
    if (startNum > endNum)
    {
        cout << "\nОшибка: начальная дата больше конечной.\n";
        return;
    }

    sortByDurationDescending(file);
    cout << "\nСписок посетителей, которые посещали клуб в период с "
         << startDate << " по " << endDate
         << ", в порядке убывания длительности тренировки:\n";
    rewind(file);
    bool found = false;
    for (int i = 0; i < n; i++)
    {
        Visit v;
        fread(&v, sizeof(Visit), 1, file);
        int curDate = dateToNumber(v.visitDate);
        if (curDate >= startNum && curDate <= endNum)
        {
            printVisit(v, i);
            found = true;
        }
    }
    if (!found)
        cout << "\nВ заданный период посетители не найдены.\n";
}

bool isFirstOccurrenceOfMembership(FILE *file, int index, const char *membershipType)
{
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < index; i++)
    {
        Visit cur;
        fread(&cur, sizeof(Visit), 1, file);
        if (strcmp(cur.membershipType, membershipType) == 0)
            return false;
    }
    return true;
}

void statisticsByMembership(FILE *file)
{
    int n = g_record_count;
    if (n == 0)
    {
        cout << "\nФайл пуст.\n";
        return;
    }

    char statFileName[260];
    cout << "Введите имя нового txt-файла для статистики: ";
    cin.getline(statFileName, 260);
    FILE *statFile = fopen(statFileName, "w");
    if (!statFile)
    {
        cout << "\nОшибка открытия файла статистики.\n";
        return;
    }

    for (int i = 0; i < n; i++)
    {
        Visit curType;
        read_record(file, i, curType);
        if (!isFirstOccurrenceOfMembership(file, i, curType.membershipType))
            continue;

        FILE *tempBin = fopen("stats_temp.dat", "w+b");
        if (!tempBin)
        {
            cout << "\nОшибка создания временного файла.\n";
            fclose(statFile);
            return;
        }
        int sumDuration = 0, count = 0;
        rewind(file);
        for (int j = 0; j < n; j++)
        {
            Visit v;
            fread(&v, sizeof(Visit), 1, file);
            if (strcmp(v.membershipType, curType.membershipType) == 0)
            {
                fwrite(&v, sizeof(Visit), 1, tempBin);
                sumDuration += v.duration;
                count++;
            }
        }
        fclose(tempBin);
        tempBin = fopen("stats_temp.dat", "r+b");
        long old_count = g_record_count;
        g_record_count = get_record_count(tempBin);
        selectionSortByDate(tempBin);
        fclose(tempBin);

        cout << "\n========================================\n";
        cout << "Тип абонемента: " << curType.membershipType << endl;
        cout << "Список посещений в порядке возрастания даты:\n";
        tempBin = fopen("stats_temp.dat", "rb");
        g_record_count = get_record_count(tempBin);
        showAllRecords(tempBin);
        fclose(tempBin);

        fprintf(statFile, "\n========================================\n");
        fprintf(statFile, "Тип абонемента: %s\n", curType.membershipType);
        fprintf(statFile, "Список посещений в порядке возрастания даты:\n");
        tempBin = fopen("stats_temp.dat", "rb");
        g_record_count = get_record_count(tempBin);
        showAllRecordsToFile(tempBin, statFile);
        fclose(tempBin);

        double average = (count > 0) ? static_cast<double>(sumDuration) / count : 0.0;
        cout << "Средняя длительность тренировки: " << average << " мин" << endl;
        fprintf(statFile, "Средняя длительность тренировки: %.2f мин\n", average);
        remove("stats_temp.dat");
        g_record_count = old_count;
    }
    fclose(statFile);
    cout << "\nСтатистика сохранена в файл " << statFileName << "\n";
}

void importVisitsFromTextFile(FILE *file, const char *filename, bool clearBefore)
{
    char textName[260];
    cout << "\nФормат текстового файла:\n";
    cout << "1 строка - ФИО\n2 строка - тип абонемента\n3 строка - дата (dd.mm.yyyy)\n4 строка - время (hh:mm)\n5 строка - длительность\n\n";
    cout << "Введите имя текстового файла: ";
    cin.getline(textName, 260);

    FILE *txt = fopen(textName, "r");
    if (!txt)
    {
        cout << "\nОшибка открытия текстового файла.\n";
        return;
    }

    if (clearBefore)
    {
        fclose(file);
        create_empty_file(filename);
        file = fopen(filename, "r+b");
    }

    int imported = 0, skipped = 0, recordNum = 0;
    char line[STR_SIZE];
    while (fgets(line, STR_SIZE, txt))
    {
        if (line[0] == '\n' || line[0] == '\0')
            continue;
        line[strcspn(line, "\n")] = '\0';

        Visit v;
        strncpy(v.fio, line, STR_SIZE - 1);
        v.fio[STR_SIZE - 1] = '\0';

        if (!fgets(v.membershipType, STR_SIZE, txt))
            break;
        v.membershipType[strcspn(v.membershipType, "\n")] = '\0';
        if (!fgets(v.visitDate, 11, txt))
            break;
        v.visitDate[strcspn(v.visitDate, "\n")] = '\0';
        if (!fgets(v.visitTime, 6, txt))
            break;
        v.visitTime[strcspn(v.visitTime, "\n")] = '\0';
        if (!fgets(line, STR_SIZE, txt))
            break;
        line[strcspn(line, "\n")] = '\0';

        recordNum++;
        if (strlen(v.membershipType) == 0 || !isValidDateFormat(v.visitDate) ||
            !isValidTimeFormat(v.visitTime) || !isValidPositiveNumberLine(line))
        {
            cout << "\nЗапись #" << recordNum << " пропущена из-за неверного формата.\n";
            skipped++;
            continue;
        }
        v.duration = atoi(line);
        if (v.duration <= 0)
        {
            cout << "\nЗапись #" << recordNum << " пропущена: длительность <= 0.\n";
            skipped++;
            continue;
        }
        if (!append_record(file, v))
        {
            cout << "\nОшибка записи в бинарный файл.\n";
            fclose(txt);
            return;
        }
        imported++;
    }
    fclose(txt);
    cout << "\nИмпорт завершён. Загружено: " << imported << ", пропущено: " << skipped << "\n";
}
void createSampleFile(FILE *&file, const char *filename)
{
    create_empty_file(filename);
    file = fopen(filename, "r+b");
    if (!file)
    {
        cout << "Ошибка открытия файла.\n";
        return;
    }
    g_record_count = 0;

    const int SAMPLE_COUNT = 22;
    Visit samples[SAMPLE_COUNT] = {
        {"Иванов Иван Иванович", "Безлимитный", "01.02.2025", "08:15", 60},
        {"Петрова Мария Сергеевна", "Стандарт", "02.02.2025", "09:00", 45},
        {"Сидоров Алексей Петрович", "Премиум", "03.02.2025", "07:45", 90},
        {"Кузнецова Ольга Владимировна", "Безлимитный", "04.02.2025", "10:30", 55},
        {"Смирнов Дмитрий Андреевич", "Стандарт", "05.02.2025", "11:00", 40},
        {"Морозова Анна Игоревна", "Премиум", "06.02.2025", "12:20", 75},
        {"Васильев Никита Романович", "Безлимитный", "07.02.2025", "13:45", 50},
        {"Павлова Екатерина Денисовна", "Стандарт", "08.02.2025", "14:10", 35},
        {"Ковалёв Артём Олегович", "Премиум", "09.02.2025", "15:30", 80},
        {"Фёдорова Ксения Павловна", "Безлимитный", "10.02.2025", "16:00", 65},
        {"Белов Сергей Михайлович", "Стандарт", "11.02.2025", "17:20", 55},
        {"Григорьева Дарья Александровна", "Премиум", "12.02.2025", "18:00", 70},
        {"Козлов Максим Викторович", "Безлимитный", "13.02.2025", "08:40", 90},
        {"Новикова Алина Станиславовна", "Стандарт", "14.02.2025", "09:15", 45},
        {"Яковлев Илья Вячеславович", "Премиум", "15.02.2025", "10:05", 60},
        {"Соколова Полина Артёмовна", "Безлимитный", "16.02.2025", "11:45", 50},
        {"Михайлов Даниил Кириллович", "Стандарт", "17.02.2025", "12:30", 40},
        {"Воробьёва Елизавета Андреевна", "Премиум", "18.02.2025", "13:15", 85},
        {"Лебедев Владислав Дмитриевич", "Безлимитный", "19.02.2025", "14:50", 55},
        {"Семёнова Варвара Львовна", "Стандарт", "20.02.2025", "15:25", 35},
        {"Егоров Тимур Русланович", "Премиум", "21.02.2025", "16:40", 95},
        {"Андреева Милана Эмильевна", "Безлимитный", "22.02.2025", "17:55", 70}};

    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        if (!append_record(file, samples[i]))
        {
            cout << "Ошибка при добавлении тестовой записи #" << i + 1 << endl;
        }
    }
    cout << "Файл \"" << filename << "\" успешно создан и заполнен 22 тестовыми записями.\n";
}
void createNewMainFile(FILE *&file, const char *filename)
{
    int mode;
    while (true)
    {
        cout << "Как заполнить основной файл?\n1. Ввести записи вручную\n2. Загрузить из текстового файла\nВыберите пункт: ";
        cin >> mode;
        if (cin.fail() || (mode != 1 && mode != 2))
        {
            cout << "Ошибка: выберите 1 или 2.\n";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        else
        {
            cin.ignore(1000, '\n');
            break;
        }
    }

    create_empty_file(filename);
    file = fopen(filename, "r+b");
    if (!file)
    {
        cout << "Ошибка открытия файла.\n";
        return;
    }
    g_record_count = 0;

    if (mode == 1)
    {
        int n;
        while (true)
        {
            cout << "Введите количество записей: ";
            cin >> n;
            if (cin.fail() || n <= 0)
            {
                cout << "Ошибка: введите положительное число.\n";
                cin.clear();
                cin.ignore(1000, '\n');
            }
            else
            {
                cin.ignore(1000, '\n');
                break;
            }
        }
        for (int i = 0; i < n; i++)
        {
            Visit v;
            cout << "\n===== Запись #" << i + 1 << " =====\n";
            inputVisit(v);
            append_record(file, v);
        }
    }
    else
    {
        importVisitsFromTextFile(file, filename, false);
    }
}

int main()
{
    setlocale(LC_ALL, ".UTF8");

    char current_file[260] = "";
    bool file_selected = false;
    FILE *file = nullptr;

    while (!file_selected)
    {
        cout << "\n========== ВЫБОР ФАЙЛА ==========\n";
        cout << "1. Создать новый файл\n";
        cout << "2. Удалить файл\n";
        cout << "3. Очистить файл\n";
        cout << "4. Использовать файл по умолчанию (visits.dat)\n";
        cout << "5. Открыть существующий файл\n";
        cout << "6. Создать файл с тестовыми данными (22 записи)\n";
        cout << "0. Выход из программы\n";
        cout << "Выберите пункт: ";

        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice)
        {
        case 1:
        {
            cout << "Введите имя нового файла: ";
            cin.getline(current_file, 260);
            createNewMainFile(file, current_file);
            if (file)
                file_selected = true;
            break;
        }
        case 2:
        {
            cout << "Введите имя файла для удаления: ";
            cin.getline(current_file, 260);
            if (remove(current_file) == 0)
                cout << "Файл успешно удалён.\n";
            else
                cout << "Ошибка удаления файла.\n";
            break;
        }
        case 3:
        {
            cout << "Введите имя файла для очистки: ";
            cin.getline(current_file, 260);
            create_empty_file(current_file);
            cout << "Файл очищен.\n";
            break;
        }
        case 4:
        {
            strcpy(current_file, "visits.dat");
            if (!file_exists(current_file))
            {
                cout << "Файл по умолчанию не найден, будет создан новый.\n";
                createNewMainFile(file, current_file);
            }
            else
            {
                file = fopen(current_file, "r+b");
                if (file)
                    g_record_count = get_record_count(file);
            }
            if (file)
                file_selected = true;
            break;
        }
        case 5:
        {
            cout << "Введите имя файла для открытия: ";
            cin.getline(current_file, 260);
            if (!file_exists(current_file))
            {
                cout << "Файл не существует. Создать новый? (1 - да, 0 - нет): ";
                int ans;
                cin >> ans;
                cin.ignore(1000, '\n');
                if (ans == 1)
                    createNewMainFile(file, current_file);
            }
            else
            {
                file = fopen(current_file, "r+b");
                if (file)
                    g_record_count = get_record_count(file);
            }
            if (file)
                file_selected = true;
            break;
        }
        case 6:
        {
            strcpy(current_file, "sample_visits.dat");
            createSampleFile(file, current_file);
            if (file)
                file_selected = true;
            break;
        }
        case 0:
            return 0;
        default:
            cout << "Неверный пункт меню.\n";
        }
    }

    int choice;
    do
    {
        cout << "\n========== МЕНЮ ==========\n";
        cout << "1. Показать все записи\n";
        cout << "2. Линейный поиск по ФИО посетителя\n";
        cout << "3. Бинарный поиск по длительности тренировки\n";
        cout << "4. Быстрая сортировка по типу абонемента\n";
        cout << "5. Сортировка выбором по дате посещения\n";
        cout << "6. Сортировка вставками по времени посещения\n";
        cout << "7. Анализ посещений за период\n";
        cout << "8. Статистика по абонементам\n";
        cout << "9. Редактирование записи\n";
        cout << "10. Удаление записи\n";
        cout << "11. Вернуться к выбору файла\n";
        cout << "0. Выход\n";
        cout << "Выберите пункт: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice)
        {
        case 1:
            showAllRecords(file);
            break;
        case 2:
            linearSearchByFio(file);
            break;
        case 3:
            performBinarySearchByDuration(file);
            break;
        case 4:
            quickSortMembership(file);
            break;
        case 5:
            selectionSortByDate(file);
            cout << "\nФайл отсортирован по дате.\n";
            break;
        case 6:
            insertionSortByTime(file);
            cout << "\nФайл отсортирован по времени.\n";
            break;
        case 7:
            analyzeVisitsByPeriod(file);
            break;
        case 8:
            statisticsByMembership(file);
            break;
        case 9:
            editRecordByIndex(file);
            break;
        case 10:
            deleteRecordByIndex(file, current_file);
            break;
        case 11:
            fclose(file);
            file_selected = false;
            break;
        case 0:
            break;
        default:
            cout << "\nНеверный пункт меню.\n";
            break;
        }
    } while (choice != 0 && file_selected);

    if (file)
        fclose(file);
    return 0;
}