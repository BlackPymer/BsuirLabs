#include <iostream>
#include <cstring>
struct Node
{
    int value;
    char *text;
    Node *left;
    Node *right;
};

struct Tree
{
    Node *root;
    Tree()
    {
        root = nullptr;
    }
    Tree(int val, const char *text)
    {
        root = new Node;
        root->value = val;
        root->text = new char[strlen(text) + 1];
        strcpy(root->text, text);
        root->left = nullptr;
        root->right = nullptr;
    }
    Tree(int *vals, char **texts, int size)
    {
        root = nullptr;
        for (int i = 0; i < size; i++)
            add(vals[i], texts[i]);
    }
    void add(int val, const char *text)
    {
        Node *tmp = new Node;
        tmp->value = val;
        tmp->text = new char[strlen(text) + 1];
        strcpy(tmp->text, text);
        if (root == nullptr)
        {
            root = tmp;
            return;
        }
        Node *cur_node = root;
        while (true)
        {
            if (cur_node->value > val)
            {
                if (cur_node->left == nullptr)
                {
                    cur_node->left = tmp;
                    return;
                }
                cur_node = cur_node->left;
            }
            else if (cur_node->value < val)
            {
                if (cur_node->right == nullptr)
                {
                    cur_node->right = tmp;
                    return;
                }
                cur_node = cur_node->right;
            }
            else
            {
                delete tmp;
                return;
            }
        }
    }
    void clear(Node *cur_node)
    {
        if (cur_node == nullptr)
            return;
        clear(cur_node->left);
        clear(cur_node->right);
        delete[] cur_node->text;
        delete cur_node;
    }
    void clear()
    {
        clear(root);
    }

    void remove(Node *node, Node *parent, bool left)
    {
        if (node->left == nullptr)
        {
            if (left)
                parent->left = node->right;
            else
                parent->right = node->right;
            delete[] node->text;
            delete node;
            return;
        }
        if (node->right == nullptr)
        {
            if (left)
                parent->left = node->left;
            else
                parent->right = node->left;
            delete[] node->text;
            delete node;
            return;
        }
        Node *cur_parent = node;
        Node *cur_node = node->right;
        while (cur_node->left != nullptr)
        {
            cur_parent = cur_node;
            cur_node = cur_node->left;
        }
        node->value = cur_node->value;
        delete[] node->text;
        node->text = new char[strlen(cur_node->text) + 1];
        strcpy(node->text, cur_node->text);

        if (cur_parent != node)
            cur_parent->left = cur_node->right; // update parent not to forget left part of the rightest node
        else
            node->right = cur_node->right;

        delete[] cur_node->text;
        delete cur_node;
    }

    void remove(int val)
    {
        if (root->value == val)
        {
            Node *tmp = new Node();
            tmp->right = nullptr;
            tmp->left = nullptr;
            remove(root, tmp, 0);
            if (tmp->right != nullptr)
                root = tmp->right;
            delete tmp;
            return;
        }
        bool left = 0;
        Node *parent = root;
        Node *cur_node = ((root->value < val) ? root->right : root->left);
        if (parent->left == cur_node)
            left = 1;
        while (cur_node != nullptr && cur_node->value != val)
        {
            parent = cur_node;
            if (cur_node->value > val)
            {
                left = 1;
                cur_node = cur_node->left;
            }
            else
            {
                left = 0;
                cur_node = cur_node->right;
            }
        }
        if (cur_node == nullptr)
            return;
        remove(cur_node, parent, left);
    }
    void print(Node *cur_node = nullptr)
    {
        if (cur_node == nullptr)
            cur_node = root;
        if (cur_node == nullptr)
            return;
        if (cur_node->left != nullptr)
            print(cur_node->left);
        std::cout << cur_node->value << " " << cur_node->text << std::endl;
        if (cur_node->right != nullptr)
            print(cur_node->right);
    }
    void print_prefix(Node *cur_node = nullptr)
    {
        if (cur_node == nullptr)
            cur_node = root;
        if (cur_node == nullptr)
            return;

        std::cout << cur_node->value << " " << cur_node->text << std::endl;
        if (cur_node->left != nullptr)
            print(cur_node->left);
        if (cur_node->right != nullptr)
            print(cur_node->right);
    }
    void print_postfix(Node *cur_node = nullptr)
    {
        if (cur_node == nullptr)
            cur_node = root;
        if (cur_node == nullptr)
            return;

        if (cur_node->left != nullptr)
            print(cur_node->left);
        if (cur_node->right != nullptr)
            print(cur_node->right);

        std::cout << cur_node->value << " " << cur_node->text << std::endl;
    }

    int num_of_words(char c, Node *node = nullptr)
    {
        if (node == nullptr)
            node = root;
        return int(node->text[0] == c) + (node->left == nullptr ? 0 : num_of_words(c, node->left)) + (node->right == nullptr ? 0 : num_of_words(c, node->right));
    }
    ~Tree()
    {
        clear();
    }
};

int main()
{
    Tree t;
    while (true)
    {
        std::cout << "--------------------------------------------------\n";
        std::cout << "1.Add.\n2.Print.\n3.Print prefix.\n4.Print postfix.\n5.Num of words.\n6.Remove.\n7.Clear.\n8.Exit.\n->";
        int num = 0;
        std::cin >> num;
        if (std::cin.fail())
        {
            std::cout << "Bad input\n";
            return 1;
        }
        if (num < 1 || num > 8)
        {
            std::cout << "Enter number between 1 and 8!\n";
            continue;
        }
        switch (num)
        {
        case 1:
        {
            std::cout << "Enter key: ";
            int key = 0;
            std::cin >> key;

            std::cout << "Enter val: ";
            char *text = new char[100];
            std::cin >> text;

            t.add(key, text);
            delete[] text;
            break;
        }
        case 2:
        {
            t.print();
            break;
        }
        case 3:
        {
            t.print_prefix();
            break;
        }
        case 4:
        {
            t.print_postfix();
            break;
        }
        case 5:
        {
            std::cout << "Enter symbol to calc texts start with: ";
            char c;
            std::cin >> c;
            std::cout << "Number of words start with " << c << ' ' << t.num_of_words(c) << '\n';
            break;
        }
        case 6:
        {
            std::cout << "Enter number to find: ";
            int n;
            std::cin >> n;
            t.remove(n);
            break;
        }
        case 7:
        {
            t.clear();
            break;
        }
        case 8:
            return 0;
            break;
        }
    }
    return 0;
}