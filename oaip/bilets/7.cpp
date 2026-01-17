#include <iostream>
int main()
{
    char a = 3;
    std::cout << typeid(a).name();
    return 0;
}