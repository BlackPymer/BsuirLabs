#include <iostream>

int main()
{
    int **arr, N;
    std::cin >> N;
    arr = new int *[N];
    for (int i = 0; i < N; ++i)
    {
        arr[i] = new int[N];
        for (int j = 0; j < N; ++j)
        {
            std::cin >> arr[i][j];
        }
    }
    for (int i = 0; i < N - 1; i += 2)
    {
        // variant 1

        int *tmp = arr[i];
        arr[i] = arr[i + 1];
        arr[i + 1] = tmp;
        /*
        // variant 2
        for (int j = 0; j < N; ++j)
        {
            arr[i + 1][j] ^= arr[i][j];
            arr[i][j] ^= arr[i + 1][j];
            arr[i + 1][j] ^= arr[i][j];
        }*/
    }
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
            std::cout << arr[i][j] << ' ';
        std::cout << "\n";
    }
    delete[] arr;
    return 0;
}