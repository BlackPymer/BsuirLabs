#include <iostream>

int main()
{
    double x, y, z, min;
    std::cin >> x >> y >> z;
    if (x < y)
    {
        if (x < z)
            min = x;
        else
            min = z;
    }
    else
    {
        if (y < z)
            min = y;
        else
            min = z;
    }
    std::cout << min << '\n';
    return 0;
}