#include <iostream>

double func(double n)
{
    if (n == 1)
        return 1 / 1.5;
    return 1 / (n + func(n - 1));
}
double func2(double n)
{
    double res = 1 / 1.5;
    for (int i = 2; i <= n; ++i)
    {
        res = 1 / (i + res);
    }
    return res;
}

int main()
{
    double n = 0;
    std::cin >> n;
    if (std::cin.fail())
    {
        std::cout << "Bad input\n";
        return 1;
    }
    std::cout << "The result: " << func(n) << '\n';
    std::cout << "The result: " << func2(n) << '\n';

    return 0;
}