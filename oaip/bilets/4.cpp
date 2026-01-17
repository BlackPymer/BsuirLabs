#include <iostream>

int main()
{
    int **arr, N, i, j;
    std::cin >> N;
    arr = new int *[N];
    for (i = 0; i < N; ++i)
    {
        arr[i] = new int[N];
        for (j = 0; j < N; ++j)
            std::cin >> arr[i][j];
    }
    for (i = 0; i < N; ++i)
    {
        for (j = 0; j < N; ++j)
        {
            if (arr[j][i] < 0)
            {
                for (int k = N - 1; k >= 0; --k)
                {
                    if (arr[k][i] == 0)
                    {
                        arr[j][i] ^= arr[k][i];
                        arr[k][i] ^= arr[j][i];
                        arr[j][i] ^= arr[k][i];
                        break;
                    }
                }
                break;
            }
        }
    }
    for (i = 0; i < N; ++i)
    {
        for (j = 0; j < N; ++j)
            std::cout << arr[i][j] << ' ';
        std::cout << '\n';
    }
    return 0;
}