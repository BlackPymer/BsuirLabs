#include <iostream>
#include <cstring>
/*
Дан текст. Найти количество групп состоящих из цифр подряд и максимальную длину из всех таких групп.
*/
int main()
{
    char *str = new char[1000];
    std::cin.getline(str, 1000);
    int groups = 0, cur_group_size = 0, max_group_size = 0;
    for (int i = 0; i < strlen(str); ++i)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            if (cur_group_size == 0)
                groups++;
            cur_group_size++;
            max_group_size = (max_group_size < cur_group_size ? cur_group_size : max_group_size);
        }
        else
            cur_group_size = 0;
    }
    std::cout << "Num of groups: " << groups << "\nmax groups length: " << max_group_size;
    delete[] str;
    return 0;
}