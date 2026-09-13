#include <iostream>
#include <cstring>

using namespace std;

int get_prior(char token)
{
    if (token == '*' || token == '/')
        return 2;
    if (token == '+' || token == '-')
        return 1;
    return 0;
}

struct Stack
{
    char info;
    Stack *next;
};

void InStack(Stack *&p, char in)
{
    Stack *t = new Stack;
    t->info = in;
    t->next = p;
    p = t;
}

char OutStack(Stack *&p)
{
    Stack *t = p;
    char out = p->info;
    p = p->next;
    delete t;
    return out;
}

struct Stack2
{
    int info;
    Stack2 *next;
};

void InStack(Stack2 *&p, int in)
{
    Stack2 *t = new Stack2;
    t->info = in;
    t->next = p;
    p = t;
}

int OutStack(Stack2 *&p)
{
    Stack2 *t = p;
    int out = p->info;
    p = p->next;
    delete t;
    return out;
}

char *to_poland(const char *str)
{
    char *result = new char[strlen(str) * 2 + 1];
    int i = 0, j = 0;
    Stack *st = nullptr;

    while (str[i] != '\0')
    {
        if (isdigit(str[i]))
        {
            while (isdigit(str[i]))
            {
                result[j++] = str[i++];
            }
            result[j++] = ' ';
        }
        else if (str[i] == '(')
        {
            InStack(st, str[i++]);
        }
        else if (str[i] == ')')
        {
            while (st != nullptr && st->info != '(')
            {
                result[j++] = OutStack(st);
                result[j++] = ' ';
            }
            if (st != nullptr)
                OutStack(st);
            i++;
        }
        else if (get_prior(str[i]) > 0)
        {
            while (st != nullptr && get_prior(st->info) >= get_prior(str[i]))
            {
                result[j++] = OutStack(st);
                result[j++] = ' ';
            }
            InStack(st, str[i++]);
        }
        else
        {
            i++;
        }
    }

    while (st != nullptr)
    {
        result[j++] = OutStack(st);
        result[j++] = ' ';
    }

    result[j > 0 ? j - 1 : 0] = '\0';
    return result;
}

int calc(char *poland)
{
    Stack2 *st = nullptr;
    int i = 0, j = 0;
    char *cur_tok = new char[10];
    while (poland[i] != '\0')
    {
        while (isdigit(poland[i]))
            cur_tok[j++] = poland[i++];
        if (j != 0)
        {
            cur_tok[j] = '\0';
            j = 0;
            InStack(st, stoi(cur_tok));
        }
        switch (poland[i])
        {
        case ' ':
        {
            i++;
            break;
        }
        case '+':
        {
            int b = OutStack(st);
            int a = OutStack(st);
            InStack(st, a + b);
            i++;
            break;
        }
        case '-':
        {
            int b = OutStack(st);
            int a = OutStack(st);
            InStack(st, a - b);
            i++;
            break;
        }
        case '*':
        {
            int b = OutStack(st);
            int a = OutStack(st);
            InStack(st, a * b);
            i++;
            break;
        }
        case '/':
        {
            int b = OutStack(st);
            int a = OutStack(st);
            InStack(st, a / b);
            i++;
            break;
        }
        }
    }
    return OutStack(st);
}

int main()
{
    std::cout << "Enter primer\n";
    char *inp;
    std::cin >> inp;
    char *res = to_poland(inp);
    cout << "Poland: " << res << endl;
    cout << "Result: " << calc(res) << endl;
    delete[] res;
    return 0;
}
