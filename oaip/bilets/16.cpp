#include <iostream>
#include <cstring>
// Дана строка. Удалить 'abc', после которого идёт число
int main()
{
    char *str = new char[1000];
    std::cin.getline(str, 1000);
    int i = 0, j = 0;

    while (str[i] != '\0')
    {
        if (str[i] == 'a' && str[i + 1] == 'b' && str[i + 2] == 'c' && str[i + 3] >= '0' && str[i + 3] <= '9')
            i += 3;
        else
            str[j++] = str[i++];
    }
    str[j] = '\0';
    for (int i = 0; i < strlen(str); ++i)
    {
        std::cout << str[i];
    }
    delete[] str;
    return 0;
}