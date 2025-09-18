#include <iostream>
#include <math.h>
int main()
{
    double z, a, d, x;
    int func = 0;

    std::cout << "Выберите функцию: 1 - 2x; 2 - x^2; 3 - x/3. Введите соответствующий номер\n";
    std::cin >> func;
    if (std::cin.fail())
    {
        std::cout << "Введите число от 1 до 3\n";
        return 1;
    }
    if (func > 3 || func < 1)
    {
        std::cout << "Введите число от 1 до 3\n";
        return 1;
    }

    std::cout << "Введите z\n";
    std::cin >> z;
    if (std::cin.fail())
    {
        std::cout << "z должно быть числом!\n";
        return 1;
    }
    if (z > 0)
    {
        x = -3 * z;
        std::cout << "Так как z > 0, значения x = -3z, а именно: " << x << '\n';
    }
    else
    {
        x = z * z;
        std::cout << "Так как z <= 0, значения x = z^2, а именно: " << x << '\n';
    }

    std::cout << "Введите константы a и d\n";
    std::cin >> a >> d;
    if (std::cin.fail())
    {
        std::cout << "a и d должны быть числами!\n";
        return 1;
    }

    double func_value = a * sin(x) + d * exp(-(x + 3));
    double y = 2;
    switch (func)
    {
    case 1:
        y *= 2 * func_value;
        std::cout << "Результат вычисления у = 2 * (" << a << " * sin(" << x << ") + " << d << " * e^(-(" << x << "+3))) равен " << y << '\n';
        break;
    case 2:
        y *= func_value * func_value;
        std::cout << "Результат вычисления у = (" << a << " * sin(" << x << ") + " << d << " * e^(-(" << x << "+3)))^2 равен " << y << '\n';
        break;
    case 3:
        y *= func_value / 3;
        std::cout << "Результат вычисления у = (" << a << " * sin(" << x << ") + " << d << " * e^(-(" << x << "+3)))/3 равен " << y << '\n';
        break;
    }

    return 0;
}