#include <iostream>

#include <cstdlib>
#include <ctime>

int random(int from)
{
    return int(double(rand()) / double(RAND_MAX) * from * 2 - from);
}

struct Node
{
    int value;
    Node *next;
    Node *prev;
};

struct Queue
{
    Node *head = new Node;
    Node *tail = new Node;

    Queue()
    {
        head->next = tail;
        tail->prev = head;
        tail->next = head;
        head->prev = tail;
    }
    void AddToBegining(int val)
    {
        Node *node = new Node;
        node->value = val;
        node->prev = head;
        node->next = head->next;
        head->next = node;
        node->next->prev = node;
    }
    void AddToEnd(int val)
    {
        Node *node = new Node;
        node->value = val;
        node->next = tail;
        node->prev = tail->prev;
        tail->prev = node;
        node->prev->next = node;
    }
    int last() { return tail->prev->value; }
    int first() { return head->next->value; }
    void ViewFromEnd()
    {
        Node *t = tail->prev;
        while (t != head)
        {
            std::cout << t->value << ' ';
            t = t->prev;
        }
        std::cout << '\n';
    }
    void ViewFromBegining()
    {
        Node *t = head->next;
        while (t != tail)
        {
            std::cout << t->value << ' ';
            t = t->next;
        }

        std::cout << '\n';
    }
    ~Queue()
    {
        Node *t = head;
        while (t != tail)
        {
            Node *next = t->next;
            delete t;
            t = next;
        }
        delete t;
    }
    void avg()
    {
        double average = 0;
        Node *t = head->next;
        int num = 0;
        while (t != tail)
        {
            average += t->value;
            t = t->next;
            num++;
        }
        head->next->value = int(average) / num;
    }
};

int main()
{
    Queue q;
    int n, k;
    std::cout << "number of elements = ";
    std::cin >> n;
    for (int i = 0; i < n; ++i)
        q.AddToBegining(random(100));
    q.ViewFromBegining();
    do
    {
        std::cout << "0. Exit program. 1. Add to begining. 2. Add to end. 3. View from begining. 4. View from end. 5. Avg.: ";
        std::cin >> n;
        if (std::cin.fail())
        {
            std::cout << "bad input\n";
            return 1;
        }
        switch (n)
        {
        case 0:
            return 0;
            break;
        case 1:
            std::cout << "value = ";
            std::cin >> k;
            q.AddToBegining(k);
            break;
        case 2:
            std::cout << "value = ";
            std::cin >> k;
            q.AddToEnd(k);
            break;
        case 3:
            q.ViewFromBegining();
            break;
        case 4:
            q.ViewFromEnd();
            break;
        case 5:
            q.avg();
        default:
            break;
        }

    } while (n != 0);
    return 0;
}