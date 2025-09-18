#include <iostream>
#include <vector>
#include <set>
std::vector<int> join(std::vector<int> X, std::vector<int> Y)
{
    std::vector<int> result;
    int x_ind = 0, y_ind = 0;

    while (x_ind < X.size() && y_ind < Y.size())
    {
        if (X[x_ind] < Y[y_ind])
        {
            if (result.size() == 0 || result[result.size() - 1] != X[x_ind])
                result.push_back(X[x_ind]);
            ++x_ind;
        }
        else
        {
            if (result.size() == 0 || result[result.size() - 1] != Y[y_ind])
                result.push_back(Y[y_ind]);
            ++y_ind;
        }
    }
    for (int i = x_ind; i < X.size(); i++)
    {
        if (X[i] != result.back())
            result.push_back(X[i]);
    }
    for (int i = y_ind; i < Y.size(); i++)
    {
        if (Y[i] != result.back())
            result.push_back(Y[i]);
    }
    return result;
}

std::vector<int> overlap(std::vector<int> X, std::vector<int> Y)
{
    if (Y.size() == 0)
        return {};
    std::vector<int> result;
    int y = 0;
    for (int i = 0; i < X.size(); i++)
    {
        while (y < Y.size() - 1 && Y[y] < X[i])
            ++y;
        if (Y[y] == X[i])
        {
            result.push_back(Y[y]);
            ++y;
        }
    }
    return result;
}

std::vector<int> substract(std::vector<int> X, std::vector<int> Y)
{
    if (Y.size() == 0)
        return X;
    std::vector<int> result;
    int y = 0;
    for (int i = 0; i < X.size(); i++)
    {
        while (y < Y.size() - 1 && Y[y] < X[i])
            ++y;
        if (Y[y] != X[i])
        {
            result.push_back(X[i]);
        }
    }
    return result;
}

std::vector<int> simmetrical_substract(std::vector<int> X, std::vector<int> Y)
{
    std::vector<int> result;
    int x_ind = 0, y_ind = 0;
    while (x_ind < X.size() && y_ind < Y.size())
    {
        if (X[x_ind] < Y[y_ind])
        {
            if (result.size() == 0 || result[result.size() - 1] != X[x_ind])
                result.push_back(X[x_ind]);
            ++x_ind;
        }
        else if (X[x_ind] > Y[y_ind])
        {
            if (result.size() == 0 || result[result.size() - 1] != Y[y_ind])
                result.push_back(Y[y_ind]);
            ++y_ind;
        }
        else
        {
            ++y_ind;
            ++x_ind;
        }
    }
    for (int i = x_ind; i < X.size(); i++)
        result.push_back(X[i]);
    for (int i = y_ind; i < Y.size(); i++)
        result.push_back(Y[i]);
    return result;
}
std::vector<std::vector<int>> split(std::vector<int> X, int length = 2)
{
    if (length >= X.size() || length <= 0)
        return std::vector<std::vector<int>>{X};

    std::vector<std::vector<int>> result;
    std::vector<int> current;
    for (int i = 0; i < X.size(); i++)
    {
        current.push_back(X[i]);
        if ((i + 1) % length == 0)
        {
            result.push_back(current);
            current.clear();
        }
    }
    if (!current.empty())
        result.push_back(current);
    return result;
}

std::vector<int> dopolnenie(std::vector<int> U, std::vector<int> X)
{
    int u = 0;
    for (int i : X)
    {
        while (U[u] < i)
            ++u;
        if (U[u] != i)
            throw std::invalid_argument("X не является строгим подмножеством U");
    }
    return substract(U, X);
}

void print_vec(std::vector<int> X)
{
    std::cout << "{ ";
    for (int i : X)
        std::cout << i << ' ';
    std::cout << "}\n";
}

void print_vec2d(std::vector<std::vector<int>> X)
{
    std::cout << "{ ";
    for (auto i : X)
    {
        print_vec(i);
        std::cout << ' ';
    }
    std::cout << "}\n";
}

bool get_2_vec_inp(std::vector<int> &X, std::vector<int> &Y)
{
    std::cout << "Введите длину первого множетсва\n";
    int x_size = -1, y_size = -1;
    if (!(std::cin >> x_size).good() || x_size < 0)
        return false;
    std::cout << "Введите целочисленные элементы первого множества через пробел (повторения будут удалены, элементы будут отсортированы)\n";
    std::set<int> s;
    for (int i = 0; i < x_size; i++)
    {
        int num = 0;
        if (!(std::cin >> num).good())
            return false;
        s.insert(num);
    }
    X.assign(s.begin(), s.end());

    std::cout << "Введите длину второго множетсва\n";
    if (!(std::cin >> y_size).good() || y_size < 0)
        return false;
    std::cout << "Введите целочисленные элементы второго множества через пробел (повторения будут удалены, элементы будут отсортированы)\n";
    s.clear();
    for (int i = 0; i < y_size; i++)
    {
        int num = 0;
        if (!(std::cin >> num).good())
            return false;
        s.insert(num);
    }
    Y.assign(s.begin(), s.end());
    return true;
}

int main()
{
    while (true)
    {
        std::cout << "Выберите операцию над множествами: Объединение (1), пересечение (2), разность (3), симметричная разность (4), дополнение (5), разбиение(6)\n";
        int inp = 0;
        if (!(std::cin >> inp).good())
        {
            std::cout << "Введите число от 1 до 6";
            continue;
        }
        std::vector<int> X, Y;
        int x_size = -1, y_size = -1;
        std::set<int> s;
        int k = 0;

        switch (inp)
        {
        case 1:
            if (get_2_vec_inp(X, Y))
                print_vec(join(X, Y));
            break;
        case 2:
            if (get_2_vec_inp(X, Y))
                print_vec(overlap(X, Y));
            break;
        case 3:
            if (get_2_vec_inp(X, Y))
                print_vec(substract(X, Y));
            break;
        case 4:
            if (get_2_vec_inp(X, Y))
                print_vec(simmetrical_substract(X, Y));
            break;
        case 5:
            if (get_2_vec_inp(X, Y))
                print_vec(join(X, Y));
            break;
        case 6:
            std::cout << "Введите длину множества\n";
            if (!(std::cin >> x_size).good() || x_size < 0)
                return false;
            std::cout << "Введите целочисленные элементы множества через пробел (повторения будут удалены, элементы будут отсортированы)\n";
            for (int i = 0; i < x_size; i++)
            {
                int num = 0;
                if (!(std::cin >> num).good())
                    return false;
                s.insert(num);
            }
            X.assign(s.begin(), s.end());
            std::cout << "Введите коэффициент разбиения\n";
            if ((std::cin >> k).good())
                print_vec2d(split(X, k));
            break;
        default:
            std::cout << "Введите число от 1 до 6\n";
            break;
        }
    }
    return 0;
}
