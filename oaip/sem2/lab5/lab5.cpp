#include <iostream>
#include <cstring>
struct Node
{
    int value;
    char *text;
    unsigned char height = 1;
    Node *left = nullptr;
    Node *right = nullptr;
    Node()
    {
        text = new char[100];
    }
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

    int height(Node *node)
    {
        return node ? node->height : 0;
    }
    int bfactor(Node *node)
    {
        return height(node->right) - height(node->left);
    }
    void fixheight(Node *node)
    {
        unsigned char hl = height(node->left);
        unsigned char hr = height(node->right);
        node->height = ((hl > hr) ? hl : hr) + 1;
    }
    Node *rotateright(Node *node)
    {
        Node *l = node->left;
        node->left = l->right;
        l->right = node;
        fixheight(node);
        fixheight(l);

        return l;
    }
    Node *rotateleft(Node *node)
    {
        Node *r = node->right;
        node->right = r->left;
        r->left = node;
        fixheight(node);
        fixheight(r);
        return r;
    }

    Node *balance(Node *node)
    {
        fixheight(node);
        if (bfactor(node) == 2)
        {
            if (bfactor(node->right) < 0)
                node->right = rotateright(node->right);
            return rotateleft(node);
        }
        if (bfactor(node) == -2)
        {
            if (bfactor(node->left) > 0)
                node->left = rotateleft(node->left);
            return rotateright(node);
        }

        return node;
    }

    Node *insert(Node *node, int val, const char *text)
    {
        if (!node)
        {
            Node *tmp = new Node;
            tmp->value = val;
            strcpy(tmp->text, text);
            return tmp;
        }
        if (val < node->value)
            node->left = insert(node->left, val, text);
        else if (val > node->value)
            node->right = insert(node->right, val, text);
        return balance(node);
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

    Node *findmin(Node *node)
    {
        return (node->left) ? findmin(node->left) : node;
    }
    Node *removemin(Node *node)
    {
        if (node->left == nullptr)
            return node->right;
        node->left = removemin(node->left);
        return balance(node);
    }
    Node *remove(Node *node, int val)
    {
        if (!node)
            return nullptr;
        if (val < node->value)
            node->left = remove(node->left, val);
        else if (val > node->value)
            node->right = remove(node->right, val);
        else
        {
            Node *l = node->left;
            Node *r = node->right;
            delete[] node->text;
            delete node;
            if (r == nullptr)
                return l;
            Node *min = findmin(r);
            min->right = removemin(r);
            min->left = l;
            return balance(min);
        }
        return balance(node);
    }
    void print(Node *cur_node = nullptr)
    {
        if (cur_node == nullptr)
            cur_node = root;
        if (cur_node == nullptr)
            return;
        if (cur_node->right != nullptr)
            print(cur_node->right);

        std::cout << cur_node->value << " " << cur_node->text << std::endl;
        if (cur_node->left != nullptr)
            print(cur_node->left);
    }
    void print_prefix(Node *cur_node = nullptr)
    {
        if (cur_node == nullptr)
            cur_node = root;
        if (cur_node == nullptr)
            return;

        std::cout << cur_node->value << " " << cur_node->text << std::endl;
        if (cur_node->left != nullptr)
            print_prefix(cur_node->left);
        if (cur_node->right != nullptr)
            print_prefix(cur_node->right);
    }
    void print_postfix(Node *cur_node = nullptr)
    {
        if (cur_node == nullptr)
            cur_node = root;
        if (cur_node == nullptr)
            return;

        if (cur_node->left != nullptr)
            print_postfix(cur_node->left);
        if (cur_node->right != nullptr)
            print_postfix(cur_node->right);

        std::cout << cur_node->value << " " << cur_node->text << std::endl;
    }

    void print_tree(int layer, Node *cur_node = nullptr)
    {
        if (cur_node == nullptr && layer == 0)
            cur_node = root;
        if (cur_node == nullptr)
            return;
        if (cur_node->right != nullptr)
            print_tree(layer + 1, cur_node->right);
        for (int i = 0; i < layer; ++i)
            std::cout << '-';
        std::cout << cur_node->value << '\n';
        if (cur_node->left != nullptr)
            print_tree(layer + 1, cur_node->left);
    }
    void add(int val, const char *text)
    {
        root = insert(root, val, text);
    }
    void remove_el(int val)
    {
        root = remove(root, val);
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
        std::cout << "1.Add.\n2.Print.\n3.Print prefix.\n4.Print postfix.\n5.Num of words.\n6.Remove.\n7.Clear.\n8.Print formatted\n9.Exit.\n->";
        int num = 0;
        std::cin >> num;
        if (std::cin.fail())
        {
            std::cout << "Bad input\n";
            return 1;
        }
        if (num < 1 || num > 9)
        {
            std::cout << "Enter number between 1 and 9!\n";
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
            t.remove_el(n);
            break;
        }
        case 7:
        {
            t.clear();
            break;
        }
        case 8:
        {
            std::cout << "--------------------------------------------------\n";
            t.print_tree(0);
            break;
        }
        default:
        {
            return 0;
            break;
        }
        }
    }
    return 0;
}