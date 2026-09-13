#include <iostream>
#include <cstring>
bool is_vowel(char c);
char **get_words_with_vowels(char *str);
void sort_words(char **words);
int main()
{
    char *str = new char[1000];
    std::cin.getline(str, 1000);
    char **words = get_words_with_vowels(str);
    sort_words(words);
    for (int i = 0; strcmp(words[i], "") != 0; ++i)
    {
        std::cout << words[i] << '\n';
    }
    delete[] str;
    delete[] words;
    return 0;
}

char **get_words_with_vowels(char *str)
{
    int n = strlen(str);
    int max_vowels = 0;
    int cur_vowels = 0;
    char *cur_word = new char[n];
    int word_size = 0;
    int words_count = 0;
    char **words = new char *[n];
    for (int i = 0; i < n; ++i)
        words[i] = new char[n];
    for (int i = 0; i < n + 1; ++i)
    {
        if (str[i] == ' ' || str[i] == '\0')
        {
            if (cur_vowels == max_vowels)
            {
                cur_word[word_size] = '\0';
                strcpy(words[words_count++], cur_word);
            }
            else if (cur_vowels > max_vowels)
            {
                max_vowels = cur_vowels;
                words_count = 1;
                strcpy(words[0], cur_word);
            }
            word_size = 0;
            cur_vowels = 0;
        }
        else
        {
            if (is_vowel(str[i]))
                ++cur_vowels;
            cur_word[word_size++] = str[i];
        }
    }
    words[words_count] = (char *)"\0";
    delete[] cur_word;
    return words;
}
bool is_vowel(char c)
{
    char *vowels = (char *)"aeyuoiAEYUIO";
    for (int i = 0; i < 12; ++i)
    {
        if (c == vowels[i])
            return true;
    }
    return 0;
}
void sort_words(char **words)
{
    int n = 0;
    for (int i = 0; strcmp(words[i], "\0") != 0; ++i)
        ++n;
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - 1 - i; ++j)
        {
            if (strcmp(words[j], words[j + 1]) > 0)
            {
                char *word = words[j];
                words[j] = words[j + 1];
                words[j + 1] = word;
            }
        }
    }
}
