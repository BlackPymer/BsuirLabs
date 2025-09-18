#include <iostream>
#include <math.h>
#include <iomanip>
int fact(int num)
{
    if (num == 0)
        return 1;
    if (num < 0)
        std::__throw_invalid_argument("num must be >= 0");
    int res = 1;
    for (int i = 2; i <= num; i++)
        res *= i;
    return res;
}
int main()
{
    while (true)
    {
        double a, b, h, n;
        std::cin >> a >> b >> h >> n;
        if (std::cin.fail())
        {
            std::cout << "Enter only numbers\n";
            return 0;
        }
        else
        {
            if (a > b)
            {
                std::cout << "a must be <= b\n";
                continue;
            }
            if (n > 16 || n < 0)
            {
                std::cout << "n must be from 0 to 16\n";
                continue;
            }
            std::cout << "x" << std::setw(20) << "Y(x)" << std::setw(20) << "S(x)" << std::setw(20) << "|Y(x)-S(x)|\n";
            // a+=h;
            while (a <= b) // while (a < b)
            {
                double S = 0, Y = 0;
                std::cout << a << std::setw(20);
                Y = (exp(a) - exp(-a)) / 2;
                std::cout << Y << std::setw(20);
                for (int k = 0; k <= n; k++)
                {
                    S += pow(a, 2 * k + 1) / (double)fact(2 * k + 1);
                }
                std::cout << S << std::setw(20);
                std::cout << abs(Y - S);
                a += h;
                std::cout << "\n";
            }
        }
    }
    return 0;
}