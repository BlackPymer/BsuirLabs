#include "vocabulary.hpp"
#include <iostream>
#include <string>

int main()
{
    std::cout << "VOCABULARY\n";
    std::cout << "You may open file in format *word:translation* or start from zero. Write filename or 0:\n->";
    std::string filename;
    std::cin >> filename;
    Vocabulary voc;
    if (filename != "0")
    {
        try
        {
            voc = Vocabulary(filename);
        }
        catch (Vocabulary::FileOpeningFailedException &e)
        {
            std::cout << "Failed opening failed, continue with zero\n";
        }
    }
    while (true)
    {
        std::cout << "Menu\n1.Add word and translation\n2.Get translation\n3.Delete word\n 4. Get word count\n5. Exit\n->";
        int choice;
        std::cin >> choice;
        switch (choice)
        {
        case 1:
        {
            std::cout << "Enter word and translation:\n->";
            std::string word, translation;
            std::cin >> word >> translation;
            voc += std::make_pair(word, translation);
            break;
        }
        case 2:
        {
            std::cout << "Enter word:\n->";
            std::string word;
            std::cin >> word;
            try
            {
                std::cout << voc[word] << std::endl;
            }
            catch (Vocabulary::WordNotFoundException &e)
            {
                std::cout << "Word not found.\n";
            }
            break;
        }
        case 3:
        {
            std::cout << "Enter word:\n->";
            std::string word;
            std::cin >> word;
            voc -= word;
            break;
        }
        case 4:
        {
            std::cout << voc.GetWordsCount() << std::endl;
            break;
        }
        case 5:
        {
            exit(0);
        }
        default:
        {
            std::cout << "Invalid choice. Please try again.\n";
            break;
        }
        }
    }
    return 0;
}