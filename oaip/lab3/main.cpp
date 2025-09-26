#include <iostream>
#include <math.h>
#include <iomanip>
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
            std::cout << "x" << std::setw(20) << "Y(x)" << std::setw(20) << "S(x)" << std::setw(20) << "|Y(x)-S(x)|\n";
            while (a <= b)
            {
                double S = 0, Y = 0, p = a;
                std::cout << a << std::setw(20);
                Y = (exp(a) - exp(-a)) / 2;
                std::cout << Y << std::setw(20);

                for (int k = 1; k <= n + 1; k++)
                {
                    S += p;
                    p *= pow(a, 2) / ((2 * k + 1) * 2 * k);
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