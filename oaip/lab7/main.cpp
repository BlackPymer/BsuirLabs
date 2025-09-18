#include <iostream>
#include <stdio.h>
#include <cstring>
#define FILE_NAME "students.txt"

using namespace std;

struct Student
{
    char second_name[100];
    int group_number;
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

void init_file()
{
    FILE *f = fopen(FILE_NAME, "r");
    if (f == NULL)
        f = fopen(FILE_NAME, "w");
    fclose(f);
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

void get_student_info(char second_name[], Student *students, int n)
{
    bool found = false;
    for (int i = 0; i < n; ++i)
    {
        if (strcmp(students[i].second_name, second_name) == 0)
        {
            cout << "Info:\n";
            cout << "Group: " << students[i].group_number << '\n';
            cout << "Physics mark: " << students[i].physics_mark << '\n';
            cout << "Math mark: " << students[i].math_mark << '\n';
            cout << "Informatics mark: " << students[i].informatics_mark << '\n';
            cout << "Average mark: " << students[i].avg << '\n';
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

void edit_student(Student *students, int n)
{
    char sname[100];
    cout << "Enter second name of student to edit";
    cin >> sname;
    for (int i = 0; i < n; ++i)
    {
        if (strcmp(students[i].second_name, sname) == 0)
        {
            Student student = students[i];
            cout << "Found student with second name " << student.second_name << "His info:\n";
            get_student_info(student.second_name, students, n);
            cout << "Enter ";
        }
    }
}

int main()
{
    init_file();
    int size = 0;
    Student *students = new Student[0];
    while (true)
    {
        cout << '\n';
        cout << "Choose action:\n"
             << "1. Add student\n";
        cout << "2. Get student info by second name\n";
        cout << "3. Get second names of excellent students\n";
        cout << "4. Edit student\n";
        cout << "5. Delete student\n";
        cout << "6. Sort students\n";
        cout << "7. Exit program\n";
        int input;
        cin >> input;
        if (!check_input() || input < 1 || input > 7)
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
            break;
        }
        }
    }
    delete[] students;
    return 0;
}
