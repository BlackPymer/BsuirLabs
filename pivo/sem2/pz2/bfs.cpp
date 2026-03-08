#include <iostream>
#include <vector>

using namespace std;
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
bool bfs(Matrix A, int n, int b, int e)
{
    vector<int> adj = vector<int>(n * n, -1);    // Array adj;
    vector<int> visited = vector<int>(n, false); // Array visited;
    int k = 0;
    adj[k] = b;
    bool status = 0;
    ++k;
    for (int i = 0; i < k; ++i)
    {
        int cur = adj[i];
        visited[i] = true;
        for (int j = 0; j < n; ++j)
        {
            if (visited[j])
                continue;
            if (A[cur][j] == 1 && !visited[j])
            {
                adj[k] = j;
                ++k;

                if (j == e)
                {
                    status = true;
                    break;
                }
            }
        }
        if (status)
        {
            break;
        }
    }
    return status;
}
bool bfs_recursion(Matrix A, int n, int b, int e, vector<int> &adj, vector<int> &visited, int k = 0, int i = 0)
{
    if (b == e)
        return true;
    bool status = 0;
    int cur = b;
    visited[cur] = true;
    for (int j = 0; j < n; ++j)
    {
        if (visited[j])
            continue;
        if (A[cur][j] == 1 && !visited[j])
        {
            adj[k] = j;
            ++k;

            if (j == e)
            {
                status = true;
                break;
            }
        }
    }
    if (i != k)
        return bfs_recursion(A, n, adj[i], e, adj, visited, k, i + 1);
    else
        return false;
}
int main()
{
    int n = 4;
    Matrix A(n, n);
    A[0][1] = A[1][0] = A[1][1] = A[1][2] = A[2][1] = A[3][2] = A[2][3] = 1;
    A.print();
    vector<int> adj(n * n, -1);
    vector<int> vis = vector<int>(n, false);
    adj[0] = 0;
    std::cout << bfs_recursion(A, n, 0, 3, adj, vis, 1) << std::endl;
}