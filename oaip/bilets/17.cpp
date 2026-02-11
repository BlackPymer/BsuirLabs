#include <iostream>
#include <cstring>

void change_order(char *str)
{
    int n = strlen(str);
    char **res = new char *[n / 2];
    for (int i = 0; i < n / 2; ++i)
    {
        res[i] = new char[1000];
    }
    int wordNum = 0;
    int wordSize = 0;
    for (int i = 0; i < n + 1; ++i)
    {
        if (str[i] == ' ' || str[i] == '\0')
        {
            if (wordSize != 0)
            {
                res[wordNum++][wordSize] = '\0';
                wordSize = 0;
            }
        }
        else
        {
            res[wordNum][wordSize++] = str[i];
        }
    }
    for (int i = 0; i < wordNum - 1; ++i)
    {
        for (int j = 0; j < wordNum - i - 1; ++j)
        {
            if (strlen(res[j]) < strlen(res[j + 1]))
            {
                char *tmp = res[j];
                res[j] = res[j + 1];
                res[j + 1] = tmp;
            }
        }
    }
    int k = 0;
    for (int i = 0; i < wordNum; ++i)
    {
        for (int j = 0; j < strlen(res[i]); ++j)
        {
            str[k++] = res[i][j];
        }
        str[k++] = ' ';
    }
    str[k - 1] = '\0';
    delete[] res;
}

int main()
{
    char *str = new char[1000];
    std::cin.getline(str, 1000);
    change_order(str);
    std::cout << str;
    delete[] str;
    return 0;
}
