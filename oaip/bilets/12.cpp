#include <iostream>
#include <cstring>

int main()
{
    char *str = new char[100];
    std::cin.getline(str, 100);
    int cur_word_len = 0;
    bool in_word = false;
    int n = strlen(str);
    char **words = new char *[50];
    for (int i = 0; i < 50; ++i)
    {
        words[i] = new char[100];
        words[i][0] = '0';
    }
    int word_nums = 0;
    for (int i = 0; i < n + 1; ++i)
    {
        if (str[i] == ' ' || str[i] == '\0')
        {

            if (words[word_nums][0] != '0')
                words[word_nums][cur_word_len] = '\0';
            word_nums++;
            cur_word_len = 0;
        }
        else
        {
            words[word_nums][cur_word_len++] = str[i];
        }
    }
    for (int i = 0; i < word_nums - 1; ++i)
    {
        for (int j = 0; j < word_nums - 1 - i; ++j)
        {
            if (strlen(words[j]) < strlen(words[j + 1]))
            {
                char *tmp = words[j];
                words[j] = words[j + 1];
                words[j + 1] = tmp;
            }
        }
    }
    for (int i = 0; i < word_nums; ++i)
    {
        for (int j = 0; j < strlen(words[i]); ++j)
            std::cout << words[i][j];
        std::cout << ' ';
    }
    delete[] str;
    delete[] words;
    return 0;
}
