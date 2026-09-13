#include <iostream>

inline void strcpy(char *p, const char *q)
{
    while (*p++ = *q++)
        ;
}
int main()
{
    char *str = new char[100];

    strcpy(str, "Hello");
    std::cout << str << std::endl;
    delete[] str;
    return 0;
}