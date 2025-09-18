#include <iostream>
#include <math.h>
#include <cfloat>
int main()
{
    int size = 0;
    std::cout << "Enter size of the array:\n";
    std::cin >> size;
    if (std::cin.fail())
    {
        std::cout << "You should enter a number\n";
        return 0;
    }
    else if (size < 0 || size > 20)
    {
        std::cout << "size should be from 1 to 20\n";
        return 0;
    }

    double *arr = new double[size];
    double min_abs = DBL_MAX;
    int min_ind = 0;
    for (int i = 0; i < size; ++i, ++arr)
    {
        std::cin >> *arr;
        if (abs(*arr) <= min_abs)
        {
            min_abs = abs(*arr);
            min_ind = i;
        }
    }
    if (std::cin.fail())
    {
        std::cout << "You should enter numbers\n";
        return 0;
    }
    arr -= size;
    double sum = min_abs;
    for (int i = min_ind + 1; i < size; i++)
    {
        sum += abs(arr[i]);
    }
    std::cout << "sum: " << sum << '\n';
    delete[] arr;
    return 0;
}