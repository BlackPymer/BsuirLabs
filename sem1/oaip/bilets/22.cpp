#include <iostream>
int main()
{
    int *arr, N, i;
    std::cin >> N;
    if (N < 1)
    {
        std::cout << "N must be natural\n";
        delete[] arr;
        return 0;
    }
    arr = new int[N];
    for (i = 0; i < N; ++i)
        std::cin >> arr[i];
    if (i != N)
    {
        std::cout << "Bad input\n";
        delete[] arr;
        return 0;
    }
    int min, max;
    min = max = arr[0];
    std::cout << arr[0] << ' ';
    for (i = 1; i < N; ++i)
    {
        std::cout << arr[i] << ' ';
        if (arr[i] < min)
            min = arr[i];
        else if (arr[i] > max)
            max = arr[i];
    }
    std::cout << '\n';
    std::cout << "min: " << min << " max: " << max << '\n';
    if (N < 4)
    {
        std::cout << "its impossible to change symbols to 7, because array is too short\n";
        delete[] arr;
        return 0;
    }

    int el_ind = 0, num_meet = 0;

    for (i = N - 1; i > N - 5; --i)

        arr[i] = 7;
    for (i = 0; i < N; ++i)
        std::cout << arr[i] << ' ';

    delete[] arr;
    return 0;
}