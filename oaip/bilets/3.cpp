#include <iostream>
int main()
{
    int **A;
    int N, i, j;
    std::cin >> N;
    A = new int *[N];
    for (i = 0; i < N; ++i)
    {
        A[i] = new int[N];
        for (j = 0; j < N; ++j)
            std::cin >> A[i][j];
    }
    int sum = 0;
    int mult = 1;
    for (i = 0; i < N; ++i)
    {
        for (j = 0; j < N; ++j)
        {
            if (A[i][j] < 0)
                sum += A[i][j];
            if (A[i][j] % 2 == 0)
                mult *= A[i][j];
        }
    }
    std::cout << "sum: " << sum << "\nmult: " << mult << '\n';
    for (i = 0; i < N; ++i)
    {
        for (j = 0; j < N; ++j)
            std::cout << A[i][j] << ' ';
        std::cout << '\n';
    }
    int *B = new int[N];
    for (j = 0; j < N; ++j)
    {
        B[j] = 0;
        for (i = 0; i < N && B[j] == 0; ++i)
            B[j] = A[i][j] % 2 == 0;
        std::cout << B[j] << ' ';
    }
    delete[] A, B;
    return 0;
}