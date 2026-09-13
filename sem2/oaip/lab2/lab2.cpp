#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int random(int from)
{
    return int(double(rand()) / double(RAND_MAX) * from * 2 - from);
}

struct Stack
{
    int info;
    Stack *next;
};

Stack *OutStack(Stack *p, int *out)
{
    Stack *t = p;
    *out = p->info;
    p = p->next;
    delete t;
    return p;
}

Stack *InStack(Stack *p, int in)
{
    Stack *t = new Stack;
    t->info = in;
    t->next = p;
    return t;
}

void View(Stack *p)
{
    Stack *t = p;
    while (t != NULL)
    {
        cout << " " << t->info << endl;
        t = t->next;
    }
}

void Del_All(Stack **p)
{
    Stack *t;
    while (*p != NULL)
    {
        t = *p;
        *p = (*p)->next;
        delete t;
    }
}

void Sort_info(Stack *p)
{
    Stack *t = NULL, *t1;
    int r;
    do
    {
        for (t1 = p; t1->next != t; t1 = t1->next)
            if (t1->info > t1->next->info)
            {
                r = t1->info;
                t1->info = t1->next->info;
                t1->next->info = r;
            }
        t = t1;
    } while (p->next != t);
}

void avg(Stack *p)
{
    Stack *t = p;
    double avg = 0;
    int num = 0;
    do
    {
        avg += t->info;
        t = t->next;
        num++;
    } while (t->next != NULL);
    avg / num;
    p->info = int(avg);
}

int main()
{
    int i, in, n, kod;
    Stack *begin = NULL;
    srand(time(NULL));

    cout << "Input kol = ";
    cin >> n;
    for (i = 1; i <= n; i++)
    {
        in = random(100);
        begin = InStack(begin, in);
    }
    std::cout << "Unsorted: \n";
    View(begin);
    std::cout << "Sorted: \n";
    Sort_info(begin);
    View(begin);
    avg(begin);
    std::cout << "avg: \n";
    View(begin);
    return 0;
}