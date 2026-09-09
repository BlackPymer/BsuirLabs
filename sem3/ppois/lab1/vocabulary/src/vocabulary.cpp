#include "vocabulary.hpp"
#include <fstream>
#include <memory>
Vocabulary::Vocabulary(std::string filename)
{
    _words = NULL;
    _wordsCount = 0;
    std::ifstream file(filename);
    if (!file)
        throw FileOpeningFailedException();
    std::string line;
    while (getline(file, line))
    {
        std::string key, value;
        key = line.substr(0, line.find(':'));
        value = line.substr(line.find(':'));
        *this += std::make_pair(key, value);
    }
}
Vocabulary::Vocabulary()
{
    _words = NULL;
    _wordsCount = 0;
}
Vocabulary::Vocabulary(const Vocabulary &other)
{
    _wordsCount = other._wordsCount;
}

std::string &Vocabulary::operator[](std::string word)
{
    if (_wordsCount == 0)
        throw WordNotFoundException();

    WordPair *nword = _words;
    while (true)
    {
        if (nword->Word.compare(word) == 0)
            return nword->Translation;
        else if (nword->Word.compare(word) > 0)
        {
            if (nword->right != NULL)
                nword = nword->right;
            else
            {
                throw WordNotFoundException();
                break;
            }
        }
        else
        {
            if (nword->left != NULL)
                nword = nword->left;
            else
            {
                throw WordNotFoundException();
                break;
            }
        }
    }
}
void Vocabulary::operator+=(const std::pair<std::string, std::string> &word)
{
    if (_wordsCount == 0)
        _words = new WordPair(word);
    else
    {
        WordPair *nword = _words;
        while (true)
        {
            if (nword->Word.compare(word.first) >= 0)
            {
                if (nword->right != NULL)
                    nword = nword->right;
                else
                {
                    nword->right = new WordPair(word);
                    break;
                }
            }
            else
            {
                if (nword->left != NULL)
                    nword = nword->left;
                else
                {
                    nword->left = new WordPair(word);
                    break;
                }
            }
        }
    }
    _wordsCount++;
}
void Vocabulary::operator-=(const std::string &word)
{
    if (_wordsCount == 0)
        return;

    WordPair *curr = _words;
    WordPair *parent = nullptr;
    bool isLeftChild = false;
    while (curr != nullptr)
    {
        int cmp = curr->Word.compare(word);
        if (cmp == 0)
            break;
        else if (cmp > 0)
        {
            parent = curr;
            curr = curr->right;
            isLeftChild = false;
        }
        else
        {
            parent = curr;
            curr = curr->left;
            isLeftChild = true;
        }
    }

    if (curr == nullptr)
        return;

    if (curr->right == nullptr)
    {
        if (parent == nullptr)
            _words = curr->left;
        else if (isLeftChild)
            parent->left = curr->left;
        else
            parent->right = curr->left;
        delete curr;
    }
    else if (curr->left == nullptr)
    {
        if (parent == nullptr)
            _words = curr->right;
        else if (isLeftChild)
            parent->left = curr->right;
        else
            parent->right = curr->right;
        delete curr;
    }
    else
    {
        WordPair *successorParent = curr;
        WordPair *successor = curr->left;
        while (successor->right != nullptr)
        {
            successorParent = successor;
            successor = successor->right;
        }
        curr->Word = successor->Word;
        curr->Translation = successor->Translation;
        if (successorParent == curr)
            successorParent->left = successor->left;
        else
            successorParent->right = successor->left;
        delete successor;
    }

    _wordsCount--;
}
void Vocabulary::Clear()
{
    if (_wordsCount == 0)
        return;

    std::vector<WordPair *> stack;
    stack.push_back(_words);

    while (!stack.empty())
    {
        WordPair *node = stack.back();
        stack.pop_back();

        if (node->right != NULL)
            stack.push_back(node->right);
        if (node->left != NULL)
            stack.push_back(node->left);

        delete node;
    }

    _words = NULL;
    _wordsCount = 0;
}

void Vocabulary::operator=(const Vocabulary &other)
{
    if (this == &other)
        return;

    Clear();

    _wordsCount = other._wordsCount;
    if (_wordsCount == 0)
    {
        _words = NULL;
        return;
    }

    std::vector<std::pair<WordPair *, WordPair *>> stack;
    _words = new WordPair(other._words->Word, other._words->Translation);
    stack.push_back(std::make_pair(other._words, _words));

    while (!stack.empty())
    {
        WordPair *src = stack.back().first;
        WordPair *dst = stack.back().second;
        stack.pop_back();

        if (src->right != NULL)
        {
            dst->right = new WordPair(src->right->Word, src->right->Translation);
            stack.push_back(std::make_pair(src->right, dst->right));
        }
        if (src->left != NULL)
        {
            dst->left = new WordPair(src->left->Word, src->left->Translation);
            stack.push_back(std::make_pair(src->left, dst->left));
        }
    }
}

Vocabulary::~Vocabulary()
{
    Clear();
}

int Vocabulary::GetWordsCount() { return _wordsCount; }