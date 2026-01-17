#include <iostream>

bool in_array(int *arr, int value, int size)
{
    for (int i = 0; i < size; ++i)
    {
        if (arr[i] == value)
            return true;
    }
    return false;
}
int main()
{
    int **arr, n, *once, *twice, *third, once_size = 0, twice_size = 0, third_size = 0;
    std::cin >> n;
    arr = new int *[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new int[n];
        for (int j = 0; j < n; j++)
        {
            std::cin >> arr[i][j];
        }
    }
    once = new int[n * n];
    twice = new int[n * n / 2];
    third = new int[n * n / 3];
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (in_array(once, arr[i][j], once_size))
            {
                if (in_array(twice, arr[i][j], twice_size))
                {
                    if (!in_array(third, arr[i][j], third_size))
                    {
                        third[third_size++] = arr[i][j];
                    }
                }
                else
                {
                    twice[twice_size++] = arr[i][j];
                }
            }
            else
            {
                once[once_size++] = arr[i][j];
            }
        }
    }
    for (int i = 0; i < twice_size; ++i)
    {
        if (!in_array(third, twice[i], third_size))
            std::cout << twice[i] << ' ';
    }
    delete[] once;
    delete[] twice;
    delete[] third;
    return 0;
}
