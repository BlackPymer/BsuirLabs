#include <iostream>
#include <math.h>
int main()
{
    double a, b, z1, z2;
    if (!(std::cin >> a >> b).good())
    {
        std::cout << "Incorrect data input\n";
        return 1;
    }
    z1 = pow((cos(a) - cos(b)), 2) - pow((sin(a) - sin(b)), 2);
    z2 = -4 * pow(sin((a - b) / 2), 2) * cos(a + b);

    z1 = std::round(z1 * 1e9) / 1e9;
    z2 = std::round(z2 * 1e9) / 1e9;
    std::cout << z1 << '\t' << z2 << '\n';
    if (z1 == z2)
    {
        std::cout << "results are equal\n";
    }
    else
    {
        std::cout << "results are not equal\n";
    }
    return 0;
}