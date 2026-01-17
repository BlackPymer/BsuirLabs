#include <iostream>
int main()
{
    int N, **A, i, j, mult = 1, num = 0;
    std::cin >> N;
    A = new int *[N];
    for (i = 0; i < N; ++i)
    {
        A[i] = new int[N];
        for (j = 0; j < N; ++j)
        {
            std::cin >> A[i][j];
            if (A[i][j] != 0)
            {
                ++num;
                if (A[i][j] > 0)
                    mult *= A[i][j];
            }
        }
    }
    if (i != N || j != N)
    {
        std::cout << "Bad input\n";
        return 0;
    }
    std::cout << "mult: " << mult << "\nnum: " << num << '\n';
    for (i = 0; i < N; ++i)
    {
        for (j = 0; j < N; ++j)
            std::cout << A[i][j] << ' ';
        std::cout << "\n";
    }
    int *B = new int[N];
    for (i = 0; i < N; ++i)
    {
        bool v = 1;
        for (j = 0; j < N; ++j)
        {
            if (A[j][i] < 0)
            {
                v = 0;
            }
        }
        B[i] = v;
    }
    for (i = 0; i < N; ++i)
    {
        std::cout << B[i] << ' ';
    }
    delete[] A, B;
    return 0;
}