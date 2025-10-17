#include <iostream>
using namespace std;
unsigned long fact(unsigned long num)
{
    if (num == 0 || num == 1)
        return 1;
    return num * fact(num - 1);
}

unsigned long fact2(unsigned long num)
{
    unsigned long res = 2;
    if (num == 0 || num == 1)
        return 1;

    for (unsigned long i = 3; i <= num; ++i)
        res *= i;
    return res;
}
int main()
{
    while (true)
    {
        unsigned long n, k;
        cout << "Enter n and k:\n";
        cin >> n >> k;
        if (cin.fail())
        {
            cout << "n and k must be positive integers\n";
            break;
        }
        if (n < k)
        {
            cout << "n must be more or equal k\n";
            continue;
        }
        if (n > 20)
        {
            cout << "Too big numbers, we cant calc factorial of more than 20\n";
            continue;
        }
        cout << "The result is " << (fact(n) / (fact(k) * fact(n - k))) << '\n';
        cout << "The result is " << (fact2(n) / (fact2(k) * fact2(n - k))) << '\n';
    }
    return 0;
}