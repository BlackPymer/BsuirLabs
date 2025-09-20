#include <iostream>
#include <stdio.h>
#include <cstring>
#define FILE_NAME "students.txt"

using namespace std;

struct Student
{
    char second_name[100];
    long int group_number;
    int physics_mark;
    int math_mark;
    int informatics_mark;
    double avg;
};

bool check_input()
{
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(9999, '\n');
        return 0;
    }
    return 1;
}

void recalc_avg(Student *student);

Student *load_file(int &size)
{
    FILE *f = fopen(FILE_NAME, "r");
    if (f == NULL)
    {
        cout << "No data to load, created file.\n";
        f = fopen(FILE_NAME, "w");
        fclose(f);
        return new Student[0];
    }

    char buffer[100];
    fgets(buffer, sizeof(buffer), f);
    size = strtol(buffer, NULL, 10);
    Student *students = new Student[size];

    int current_line = 2;
    while (fgets(buffer, sizeof(buffer), f))
    {
        int i = (current_line - 1) / 6;
        switch ((current_line++ - 1) % 6)
        {
        case 1:
        {
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(students[i].second_name, buffer);
            break;
        }
        case 2:
        {
            students[i].group_number = strtol(buffer, NULL, 10);
            break;
        }
        case 3:
        {
            students[i].math_mark = strtol(buffer, NULL, 10);
            break;
        }
        case 4:
        {
            students[i].physics_mark = strtol(buffer, NULL, 10);
            break;
        }
        case 5:
        {
            students[i].informatics_mark = strtol(buffer, NULL, 10);
            recalc_avg(&students[i]);
            break;
        }
        default:
            break;
        }
    }
    cout << "Loaded file with " << size << " students\n";
    fclose(f);
    return students;
}

void recalc_avg(Student *student)
{
    student->avg = (student->math_mark + student->informatics_mark + student->physics_mark) / 3.;
}

Student create_student(bool &good)
{
    Student newStudent;
    cout << "Enter second name (less 100 symbols):\n";
    cin >> newStudent.second_name;
    cout << "Enter group number:\n";
    cin >> newStudent.group_number;
    cout << "Enter physics mark:\n";
    cin >> newStudent.physics_mark;
    cout << "Enter math mark:\n";
    cin >> newStudent.math_mark;
    cout << "Enter informatics mark:\n";
    cin >> newStudent.informatics_mark;

    if (!check_input() || newStudent.informatics_mark < 0 || newStudent.math_mark < 0 || newStudent.physics_mark < 0)
        good = false;
    else
        recalc_avg(&newStudent);
    return newStudent;
}

void print_student(Student student)
{
    cout << "Info:\n";
    cout << "Second name: " << student.second_name << '\n';
    cout << "Group: " << student.group_number << '\n';
    cout << "Physics mark: " << student.physics_mark << '\n';
    cout << "Math mark: " << student.math_mark << '\n';
    cout << "Informatics mark: " << student.informatics_mark << '\n';
    cout << "Average mark: " << student.avg << '\n';
}

void get_student_info(char second_name[], Student *students, int n)
{
    bool found = false;
    for (int i = 0; i < n; ++i)
    {
        if (strcmp(students[i].second_name, second_name) == 0)
        {
            print_student(students[i]);
            found = true;
        }
    }
    if (!found)
        cout << "There is no students found with this second name\n";
}

void get_excellent_students(Student *students, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (students[i].informatics_mark >= 9 && students[i].math_mark >= 9 && students[i].physics_mark >= 9)
        {
            cout << "Excellent student " << students[i].second_name << " with average mark " << students[i].avg << '\n';
        }
    }
}

Student edit_student(Student student)
{
    char sname[100];
    cout << "Enter new second name (Just press . if you dont want to change it)\n";
    cin >> sname;
    if (strcmp(sname, "."))
        strcpy(student.second_name, sname);

    cout << "Enter new group (Just press . if you dont want to change it)\n";
    cin >> sname;
    if (strcmp(sname, "."))
        student.group_number = strtol(sname, NULL, 10);

    cout << "Enter new math mark (Just press . if you dont want to change it)\n";
    cin >> sname;
    if (strcmp(sname, "."))
        student.math_mark = strtol(sname, NULL, 10);

    cout << "Enter new physics mark (Just press . if you dont want to change it)\n";
    cin >> sname;
    if (strcmp(sname, "."))
        student.group_number = strtol(sname, NULL, 10);

    cout << "Enter new informatics mark (Just press . if you dont want to change it)\n";
    cin >> sname;
    if (strcmp(sname, "."))
        student.group_number = strtol(sname, NULL, 10);

    recalc_avg(&student);
    return student;
}

bool compare(Student student1, Student student2, int option)
{
    switch (option)
    {
    case 1:
        return student1.second_name < student2.second_name;
        break;
    case 2:
        return student1.avg < student2.avg;
        break;
    case 3:
        return student1.group_number < student2.group_number;
        break;
    case 4:
        return student1.math_mark < student2.math_mark;
        break;
    case 5:
        return student1.physics_mark < student2.physics_mark;
        break;
    case 6:
        return student1.informatics_mark < student2.informatics_mark;
        break;
    default:
        break;
    }
    return 0;
}

void sort(Student *start, int n, int option)
{
    if (n == 2)
    {
        if (compare(start[1], start[0], option))
        {
            Student tmp = start[0];
            start[0] = start[1];
            start[1] = tmp;
        }
        return;
    }
    else if (n == 1)
        return;

    int middle = n / 2;
    Student *buffer = new Student[n];
    for (int i = 0; i < n; ++i)
        buffer[i] = start[i];

    sort(buffer, middle, option);
    sort(buffer + middle, n - middle, option);
    int l = 0, r = 0;
    while (l != middle && middle + r != n)
    {
        if (compare(buffer[l], buffer[middle + r], option))
            start[l + r] = buffer[l++];
        else
            start[l + r] = buffer[middle + r++];
    }
    while (l != middle)
        start[l + r] = buffer[l++];
    while (r + middle != n)
        start[l + r] = buffer[middle + r++];
    delete[] buffer;
}

void save_students(Student *students, int n)
{
    FILE *f = fopen(FILE_NAME, "w");
    fprintf(f, "%d\n", n);
    for (int i = 0; i < n; ++i)
    {
        fprintf(f, "%s\n", students[i].second_name);
        fprintf(f, "%ld\n", students[i].group_number);
        fprintf(f, "%d\n", students[i].math_mark);
        fprintf(f, "%d\n", students[i].physics_mark);
        fprintf(f, "%d\n", students[i].informatics_mark);
        fprintf(f, "%c", '\n');
    }
    fclose(f);
    cout << "Saved successfully!\n";
}

int main()
{
    int size = 0;
    Student *students = load_file(size);
    bool program_runs = true;
    bool saved = true;
    while (program_runs)
    {
        cout << '\n';
        cout << "Choose action:\n"
             << "1. Add student\n";
        cout << "2. Get student info by second name\n";
        cout << "3. Get second names of excellent students\n";
        cout << "4. Edit student\n";
        cout << "5. Delete student\n";
        cout << "6. Sort students\n";
        cout << "7. Save to file\n";
        cout << "8. Print all students\n";
        cout << "9. Exit program\n";
        int input;
        cin >> input;
        if (!check_input() || input < 1 || input > 9)
        {
            cout << "Wrong input\n";
            continue;
        }
        switch (input)
        {
        case 1:
        {
            Student *tmp = new Student[size + 1];
            bool good = true;
            tmp[size] = create_student(good);
            if (!good)
            {
                delete[] tmp;
                cout << "Failed to create student: wrong data\n";
                break;
            }
            for (int i = 0; i < size; ++i)
                tmp[i] = students[i];
            delete[] students;
            size++;
            students = tmp;
            cout << "Student created successfull\n";
            saved = false;
            break;
        }
        case 2:
        {
            char second_name[100];
            cout << "Enter student second name:\n";
            cin >> second_name;
            get_student_info(second_name, students, size);
            break;
        }
        case 3:
        {
            get_excellent_students(students, size);
            break;
        }
        case 4:
        {
            char sname[100];
            cout << "Enter second name of student to edit\n";
            cin >> sname;
            for (int i = 0; i < size; ++i)
            {
                if (strcmp(students[i].second_name, sname) == 0)
                {
                    cout << "Found student with second name " << students[i].second_name << "His info:\n";
                    get_student_info(students[i].second_name, students, size);
                    Student student = edit_student(students[i]);
                    if (check_input())
                    {
                        students[i] = student;
                        cout << "The student edited successfully\n";
                        saved = false;
                    }
                    else
                        cout << "Failed to edit this student: input error\n";
                }
            }
            break;
        }
        case 5:
        {
            char sname[100];
            cout << "Enter second name of student to delete\n";
            cin >> sname;
            bool found = false;
            Student *tmp = new Student[size - 1];
            for (int i = 0; i < size; ++i, ++students)
            {
                if (strcmp(students->second_name, sname) != 0)
                {
                    *tmp = *students;
                    tmp++;
                }
                else
                    found = true;
            }
            students -= size;
            if (found)
            {
                tmp -= size - 1;
                --size;
                delete[] students;
                students = tmp;
                cout << "The student " << sname << " was successfully deleted\n";
                saved = false;
            }
            else
            {
                tmp -= size;
                delete[] tmp;
                cout << "The student with second name " << sname << " wasn't found\n";
            }
            break;
        }
        case 6:
        {
            cout << "Choose the way to sort by: 1 - by second name; 2 - by average mark; 3 - by group;";
            cout << " 4 - by math mark; 5 - by physics mark; 6 - by informatics mark\n";
            int option = 0;
            cin >> option;
            if (check_input() && option > 0 && option < 7)
            {
                sort(students, size, option);
                cout << "Students sorted successfully\n";
                saved = false;
            }
            else
                cout << "wrong choise\n";
            break;
        }
        case 7:
        {
            save_students(students, size);
            saved = true;
            break;
        }
        case 8:
        {
            for (int i = 0; i < size; ++i)
            {
                print_student(students[i]);
                cout << '\n';
            }
            break;
        }
        case 9:
        {
            if (!saved)
            {
                cout << "There are some unsaved changes detected. Do you want to save them before closing?(y/n)\n";
                char inp;
                cin >> inp;
                if (!check_input() || inp == 'y')
                    save_students(students, size);
            }
            program_runs = false;
        }
        }
    }
    delete[] students;
    return 0;
}
