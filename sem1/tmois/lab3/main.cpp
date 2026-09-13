#include <iostream>
#include <vector>
#include <any>
#include <array>
#include <algorithm>

template <typename T>
inline std::vector<T> overlap(std::vector<T> &X, std::vector<T> &Y)
{
    std::vector<T> result;
    for (int i = 0; i < Y.size(); ++i)
    {
        int j = 0;
        for (; j < X.size(); ++j)
            if (X[j] == Y[i])
                break;
        if (j != X.size())
            result.push_back(Y[i]);
    }
    return result;
}

template <typename T>
inline std::vector<T> join(std::vector<T> &X, std::vector<T> &Y)
{
    std::vector<T> result = X;
    for (int i = 0; i < Y.size(); ++i)
    {
        int j = 0;
        for (; j < result.size(); ++j)
            if (result[j] == Y[i])
                break;
        if (j == result.size())
            result.push_back(Y[i]);
    }

    return result;
}

template <typename T>
inline bool equal(std::vector<T> X, std::vector<T> Y)
{
    if (X.size() != Y.size())
        return false;
    for (int i = 0; i < X.size(); ++i)
    {
        int j;
        for (j = 0; j < Y.size(); ++j)
        {
            if (X[i] == Y[j])
                break;
        }
        if (j == Y.size())
            return false;
    }
    return true;
}

template <typename T>
inline std::vector<T> substract(std::vector<T> &X, std::vector<T> &Y)
{
    std::vector<T> result;
    for (int i = 0; i < X.size(); ++i)
    {
        int j = 0;
        for (; j < Y.size(); ++j)
            if (Y[j] == X[i])
                break;
        if (j == Y.size())
            result.push_back(X[i]);
    }

    return result;
}

template <typename T>
inline std::vector<std::array<T, 2>> inversion(std::vector<std::array<T, 2>> &graph)
{
    std::vector<std::array<T, 2>> result;
    for (int i = 0; i < graph.size(); ++i)
    {
        std::array<T, 2> inv = {graph[i][1], graph[i][0]};
        int j = 0;
        for (; j < result.size(); ++j)
            if (result[j] == inv)
                break;
        if (j == result.size())
            result.push_back(inv);
    }
    return result;
}

template <typename T>
inline std::vector<std::array<T, 2>> composition(std::vector<std::array<T, 2>> &graph1,
                                                 std::vector<std::array<T, 2>> &graph2)
{
    std::vector<std::array<T, 2>> result;
    for (int i = 0; i < graph1.size(); ++i)
    {
        for (int j = 0; j < graph2.size(); ++j)
        {
            if (graph1[i][1] == graph2[j][0])
            {
                std::array<T, 2> comp = {graph1[i][0], graph2[j][1]};
                int k = 0;
                for (; k < result.size(); ++k)
                    if (result[k] == comp)
                        break;
                if (k == result.size())
                    result.push_back(comp);
            }
        }
    }
    return result;
}

template <typename T>
inline std::vector<T> obraz(std::vector<std::array<T, 2>> &sootv, std::vector<T> &set)
{
    std::vector<T> result;
    for (int i = 0; i < sootv.size(); ++i)
    {
        int j = 0;
        for (; j < set.size(); ++j)
            if (set[j] == sootv[i][0])
                break;
        if (j != set.size())
        {
            int k = 0;
            for (; k < result.size(); ++k)
                if (result[k] == sootv[i][1])
                    break;
            if (k == result.size())
                result.push_back(sootv[i][1]);
        }
    }
    return result;
}

template <typename T>
inline std::vector<T> proobraz(std::vector<std::array<T, 2>> &sootv, std::vector<T> &set)
{
    std::vector<T> result;
    for (int i = 0; i < sootv.size(); ++i)
    {
        int j = 0;
        for (; j < set.size(); ++j)
            if (set[j] == sootv[i][1])
                break;
        if (j != set.size())
        {
            int k = 0;
            for (; k < result.size(); ++k)
                if (result[k] == sootv[i][0])
                    break;
            if (k == result.size())
                result.push_back(sootv[i][0]);
        }
    }
    return result;
}

template <typename T>
inline void sootv_join(std::vector<T> X1, std::vector<T> Y1, std::vector<std::array<T, 2>> graph1,
                       std::vector<T> X2, std::vector<T> Y2, std::vector<std::array<T, 2>> graph2,
                       std::vector<T> &resX, std::vector<T> &resY, std::vector<std::array<T, 2>> &resGraph)
{
    resX = join(X1, X2);
    resY = join(Y1, Y2);
    resGraph = join(graph1, graph2);
}

template <typename T>
inline void sootv_overlap(std::vector<T> X1, std::vector<T> Y1, std::vector<std::array<T, 2>> graph1,
                          std::vector<T> X2, std::vector<T> Y2, std::vector<std::array<T, 2>> graph2,
                          std::vector<T> &resX, std::vector<T> &resY, std::vector<std::array<T, 2>> &resGraph)
{
    resX = overlap(X1, X2);
    resY = overlap(Y1, Y2);
    resGraph = overlap(graph1, graph2);
}

template <typename T>
inline void sootv_substract(std::vector<T> X1, std::vector<T> Y1, std::vector<std::array<T, 2>> graph1,
                            std::vector<T> X2, std::vector<T> Y2, std::vector<std::array<T, 2>> graph2,
                            std::vector<T> &resX, std::vector<T> &resY, std::vector<std::array<T, 2>> &resGraph)
{
    resX = substract(X1, X2);
    resY = substract(Y1, Y2);
    resGraph = substract(graph1, graph2);
}

template <typename T>
inline void sootv_inversion(std::vector<T> &X, std::vector<T> &Y, std::vector<std::array<T, 2>> &graph)
{

    std::vector<T> tmp(X.begin(), X.end());
    X = Y;
    Y = tmp;
    graph = inversion(graph);
}

template <typename T>
inline void sootv_composition(std::vector<T> X1, std::vector<T> Y1, std::vector<std::array<T, 2>> graph1,
                              std::vector<T> X2, std::vector<T> Y2, std::vector<std::array<T, 2>> graph2,
                              std::vector<T> &resX, std::vector<T> &resY, std::vector<std::array<T, 2>> &resGraph)
{
    if (!equal(X2, Y1))
    {
        std::cout << "impossible to do composition\n";
        return;
    }
    resX = X1;
    resY = Y2;
    resGraph = composition(graph1, graph2);
}

template <typename T>
inline void print_vec(std::vector<T> X)
{
    std::cout << "{ ";
    for (T i : X)
        std::cout << i << ' ';
    std::cout << "}\n";
}
template <typename T>
inline void print_vec(const std::vector<std::array<T, 2>> &X)
{
    std::cout << "{ ";
    for (auto const &arr : X)
        std::cout << "(" << arr[0] << "," << arr[1] << ") ";
    std::cout << "}\n";
}

template <typename T>
inline void print_sootv(const std::vector<T> &x, const std::vector<T> &y, const std::vector<std::array<T, 2>> graph)
{
    std::cout << "Г=<";
    print_vec(x);
    print_vec(y);
    print_vec(graph);
    std::cout << ">\n";
}

inline void input(std::vector<std::string> &x,
                  std::vector<std::string> &y,
                  std::vector<std::array<std::string, 2>> &graph)
{
    int size = 0;
    std::cout << "Enter X's elements quantity\n";
    std::cin >> size;
    x.resize(size);
    std::cout << "Enter X's elements\n";
    for (int i = 0; i < size; ++i)
        std::cin >> x[i];

    std::cout << "Enter Y's elements quantity\n";
    std::cin >> size;
    y.resize(size);
    std::cout << "Enter Y's elements\n";
    for (int i = 0; i < size; ++i)
        std::cin >> y[i];

    std::cout << "Enter graph's pairs quantity\n";
    std::cin >> size;
    graph.resize(size);
    std::cout << "Enter graph's pairs\n";
    for (int i = 0; i < size; ++i)
        std::cin >> graph[i][0] >> graph[i][1];
}

int main()
{
    std::vector<std::string> x;
    std::vector<std::string> y;
    std::vector<std::array<std::string, 2>> graph;
    while (true)
    {
        std::cout << "Choose operation:\n1.Obraz.\n2.Proobraz.\n3.Join.\n4.Overlap.\n5.Substract.\n6.Inversion.\n7.Composition.\nAny other to exit.\n";
        int operation;
        std::cin >> operation;
        std::vector<std::string> x1;
        std::vector<std::string> y1;
        std::vector<std::array<std::string, 2>> graph1;
        std::vector<std::string> x2;
        std::vector<std::string> y2;
        std::vector<std::array<std::string, 2>> graph2;
        switch (operation)
        {
        case 1:
        {
            std::cout << "Enter sootvetstvie:\n";
            input(x1, y1, graph1);
            std::cout << "Enter size of modmnozhetvo\n";
            int size;
            std::cin >> size;
            x2.resize(size);
            std::cout << "Enter it's elements\n";
            for (int i = 0; i < size; ++i)
                std::cin >> x2[i];
            print_vec(obraz(graph1, x2));
            break;
        }
        case 2:
        {
            std::cout << "Enter sootvetstvie:\n";
            input(x1, y1, graph1);
            std::cout << "Enter size of modmnozhetvo\n";
            int size;
            std::cin >> size;
            x2.resize(size);
            std::cout << "Enter it's elements\n";
            for (int i = 0; i < size; ++i)
                std::cin >> x2[i];
            print_vec(proobraz(graph1, x2));
            break;
        }
        case 3:
        {
            std::cout << "Enter sootvetstvie 1:\n";
            input(x1, y1, graph1);
            std::cout << "Enter sootvetstvie 2:\n";
            input(x2, y2, graph2);
            sootv_join(x1, y1, graph1, x2, y2, graph2, x, y, graph);
            print_sootv(x, y, graph);
            break;
        }
        case 4:
        {
            std::cout << "Enter sootvetstvie 1:\n";
            input(x1, y1, graph1);
            std::cout << "Enter sootvetstvie 2:\n";
            input(x2, y2, graph2);
            sootv_overlap(x1, y1, graph1, x2, y2, graph2, x, y, graph);
            print_sootv(x, y, graph);
            break;
        }
        case 5:
        {
            std::cout << "Enter sootvetstvie 1:\n";
            input(x1, y1, graph1);
            std::cout << "Enter sootvetstvie 2:\n";
            input(x2, y2, graph2);
            sootv_substract(x1, y1, graph1, x2, y2, graph2, x, y, graph);
            print_sootv(x, y, graph);
            break;
        }
        case 6:
        {
            std::cout << "Enter sootvetstvie:\n";
            input(x, y, graph);
            sootv_inversion(x, y, graph);
            print_sootv(x, y, graph);
            break;
        }
        case 7:
        {
            std::cout << "Enter sootvetstvie 1:\n";
            input(x1, y1, graph1);
            std::cout << "Enter sootvetstvie 2:\n";
            input(x2, y2, graph2);
            sootv_composition(x1, y1, graph1, x2, y2, graph2, x, y, graph);
            print_sootv(x, y, graph);
            break;
        }
        default:
            return 0;
            break;
        }
    }
    return 0;
}