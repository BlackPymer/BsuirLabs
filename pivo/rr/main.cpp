#include <vector>
#include <iostream>
#include <iomanip>
void printWeights(const std::vector<std::vector<int>> &weights, int v, int INF)
{
    std::cout << "\nMatrix of the shortest pathes:\n";
    for (int i = 0; i < v; ++i)
    {
        for (int j = 0; j < v; ++j)
        {
            if (weights[i][j] == INF)
            {
                std::cout << "INF" << "\t";
            }
            else
            {
                std::cout << weights[i][j] << "\t";
            }
        }
        std::cout << "\n";
    }
}

int main()
{
    std::cout << "Enter number of vertex\n-> ";
    int v = 0;
    std::cin >> v;
    int e = 0;
    std::cout << "Enter number of edges\n-> ";
    std::cin >> e;
    std::vector<std::vector<int>> graph = std::vector<std::vector<int>>(e, std::vector<int>(3));
    for (int i = 0; i < e; ++i)
    {
        std::cout << "Enter first vertex\n-> ";
        std::cin >> graph[i][0];
        std::cout << "Enter second vertex\n-> ";
        std::cin >> graph[i][1];
        std::cout << "Enter weight of edge\n-> ";
        std::cin >> graph[i][2];
        if (graph[i][2] >= __INT_MAX__ / 2)
        {
            std::cout << "Max value of weight can be " << __INT_MAX__ / 2 << ".\n";
            return 1;
        }
    }
    std::vector<std::vector<int>> weights = std::vector<std::vector<int>>(v, std::vector<int>(v, __INT_MAX__ / 2));
    for (auto vec : graph)
    {
        weights[vec[0]][vec[1]] = vec[2];
    }
    for (int i = 0; i < v; ++i)
        weights[i][i] = 0;

    for (int k = 0; k < v; ++k)
    {
        for (int i = 0; i < v; ++i)
        {
            for (int j = 0; j < v; ++j)
            {
                if (weights[i][j] > weights[i][k] + weights[k][j])
                    weights[i][j] = weights[i][k] + weights[k][j];
            }
        }
    }
    printWeights(weights, v, __INT_MAX__ / 2);
    return 0;
}