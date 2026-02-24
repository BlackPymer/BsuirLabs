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
    // пример обратный тому что на доске
    int n = 5, m = 7;
    Matrix B(n, m);
    B[0][0] = B[0][1] = B[0][2] = B[0][3] = B[1][0] = B[1][4] = B[2][1] = B[2][5] = B[3][2] = B[3][6] = B[4][3] = B[4][4] = B[4][6] = 1;
    B.print();

    Matrix A(n, n);
    for (int j = 0; j < m; ++j) // по столбцам
    {
        int v1 = -1, v2 = -1; // вершины которых соединяет j-ое ребро
        for (int i = 0; i < n; ++i)
        {
            if (B[i][j] == 1)
            {
                if (v1 != -1)
                {
                    v2 = i;
                    break;
                }
                v1 = i;
            }
        }
        if (v2 == -1) // петля
            A[v1][v1] = 1;
        else
            A[v1][v2] = A[v2][v1] = 1;
    }
    A.print();
}