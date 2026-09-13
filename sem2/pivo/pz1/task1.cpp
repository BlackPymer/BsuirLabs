#include <iostream>
#include <vector>

struct Matrix
{
    std::vector<std::vector<int>> data;
    Matrix(int rows, int columns)
    {
        data = std::vector<std::vector<int>>(rows, std::vector<int>(columns, 0));
    }
    void print()
    {
        std::cout << "[\n";
        for (auto i : data)
        {
            for (int j : i)
            {
                std::cout << j << ' ';
            }
            std::cout << '\n';
        }
        std::cout << "]\n";
    }
    std::vector<int> &operator[](size_t row)
    {
        return data[row];
    }
    const std::vector<int> &operator[](size_t row) const
    {
        return data[row];
    }
};

int main()
{
    // создание начальной матрицы как на доске
    int n = 5;
    Matrix A(n, n);
    A[0][1] = A[0][2] = A[0][3] = A[0][4] = A[2][2] = A[1][0] = A[1][4] = A[2][0] = A[3][0] = A[4][0] = A[4][1] = A[4][3] = A[3][4] = 1;
    A.print();

    // код с доски
    int m = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i; j < n; ++j)
            m += A[i][j]; // или if(A[i][j]==1)m++;
    }
    Matrix B(n, m);
    int e = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i; j < n; ++j)
        {
            if (A[i][j] == 1)
            {
                B[i][e] = 1;
                B[j][e] = 1;
                ++e;
            }
        }
    }
    B.print();
}