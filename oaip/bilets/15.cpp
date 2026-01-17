#include <iostream>

int main()
{
    int *arr, n;
    std::cin >> n;
    arr = new int[n];
    int min_ind = -1, max_ind = -1;
    for (int i = 0; i < n; ++i)
    {
        std::cin >> arr[i];
        if (arr[i] % 2 == 0)
        {
            if (max_ind == -1)
                max_ind = i;
            else if (arr[max_ind] < arr[i])
                max_ind = i;
        }
        else
        {
            if (min_ind == -1)
                min_ind = i;
            else if (arr[min_ind] > arr[i])
                min_ind = i;
        }
    }
    if (min_ind == -1 || max_ind == -1)
    {
        for (int i = 0; i < n; ++i)
        {
            arr[i] = 0;
        }
    }
    else
    {
        int temp = arr[min_ind];
        arr[min_ind] = arr[max_ind];
        arr[max_ind] = temp;
    }
    for (int i = 0; i < n; ++i)
        std::cout << arr[i] << ' ';
    delete[] arr;
    return 0;
}
