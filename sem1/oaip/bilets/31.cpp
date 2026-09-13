#include <iostream>
#include <cstring>
int main()
{
    char *s;
    s = new char[255];
    fgets(s, 255, stdin);
    bool contains_w = 0;
    char *current_word = new char[100];
    char *last_word = new char[100];
    char *second_word = new char[100];
    int j = 0, k = 0, i;
    for (i = 0; s[i] != NULL; i++)
    {
        if (s[i] != ' ')
        {
            if (s[i] == 'w')
                contains_w = 1;
            current_word[j++] = s[i];
        }
        else
        {
            current_word[j] = '\0';
            if (j > 0)
                k++;
            if (k == 2)
                strcpy(second_word, current_word);
            strcpy(last_word, current_word);
            if (contains_w)
                std::cout << current_word << " ";
            contains_w = 0;
            j = 0;
        }
    }
    if (contains_w)
        std::cout << current_word << std::endl;
    if (j > 0)
        k++;
    if (k > 1)
    {
        for (j = 0; last_word[j] != '\0'; j++)
            ;
        if (j == 3)
            std::cout << last_word << std::endl;
        else
            std::cout << second_word << std::endl;
    }
    else
        std::cout << "fuck yourself stupid idiot i hate you" << std::endl;
    delete[] s, current_word, last_word, second_word;
    return 0;
}
