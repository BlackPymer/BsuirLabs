#include <iostream>
#include <string.h>

int main()
{
    int n;
    std::cin >> n;
    std::cin.ignore();
    if (std::cin.fail())
    {
        std::cout << "the string size must be a number\n";
        return 0;
    }
    if (n <= 0)
    {
        std::cout << "the string must be a positive\n";
        return 0;
    }
    char *str = new char[n];
    std::cin.getline(str, n);

    int word_number = 0, word_length = 0, word_sym = -1, max_word_length = 0, max_word_number = 0, max_word_sym_number = -1;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == ' ')
        {
            if (word_length >= max_word_length)
            {
                max_word_length = word_length;
                max_word_number = word_number;
                max_word_sym_number = word_sym;
            }
            word_length = 0;
            continue;
        }
        if (word_length == 0)
        {
            ++word_number;
            word_sym = i + 1;
        }
        ++word_length;
    }
    if (word_length >= max_word_length)
    {
        max_word_length = word_length;
        max_word_number = word_number;
        max_word_sym_number = word_sym;
    }
    std::cout << "the biggest word starts at " << max_word_sym_number << " symbol. It's size: " << max_word_length << ". This is the " << max_word_number << " number in string.\n";
    delete[] str;
    return 0;
}