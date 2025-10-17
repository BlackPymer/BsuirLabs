#include <iostream>
#include <unordered_set>
#include <vector>
#include <set>
using namespace std;

struct pair_hash
{
    size_t operator()(const pair<int, int> &p) const
    {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

inline unordered_set<pair<int, int>, pair_hash> inversion(unordered_set<pair<int, int>, pair_hash> graph)
{
    for (pair<int, int> tuple : graph)
        swap(tuple.first, tuple.second);
    return graph;
}

inline unordered_set<pair<int, int>, pair_hash> composition(const unordered_set<pair<int, int>, pair_hash> &graph1,
                                                            const unordered_set<pair<int, int>, pair_hash> &graph2)
{
    unordered_set<pair<int, int>, pair_hash> result = {};
    vector<pair<int, int>> g1 = vector<pair<int, int>>(graph1.begin(), graph1.end());
    vector<pair<int, int>> g2 = vector<pair<int, int>>(graph2.begin(), graph2.end());
    for (int i = 0; i < min(g1.size(), g2.size()); ++i)
    {
        if (g1[i].second == g2[i].first)
            result.insert(pair<int, int>(g1[i].first, g2[i].second));
        else if (g1[i].second == g2[i].second)
            result.insert(pair<int, int>(g1[i].first, g2[i].first));
        else if (g1[i].first == g2[i].first)
            result.insert(pair<int, int>(g1[i].second, g2[i].second));
        else if (g1[i].first == g2[i].second)
            result.insert(pair<int, int>(g1[i].second, g2[i].first));
        else
        {
            cout << "Impossible to do composition\n";
            result.clear();
            break;
        }
    }
    return result;
}

inline unordered_set<int> projection1(const unordered_set<pair<int, int>, pair_hash> &graph)
{
    unordered_set<int> result;
    for (pair<int, int> tuple : graph)
        result.insert(tuple.first);

    return result;
}

inline unordered_set<int> projection2(const unordered_set<pair<int, int>, pair_hash> &graph)
{
    unordered_set<int> result;
    for (pair<int, int> tuple : graph)
        result.insert(tuple.second);

    return result;
}

inline void print_graph(const unordered_set<pair<int, int>, pair_hash> &graph)
{
    cout << "{";
    for (pair<int, int> tuple : graph)
        cout << '<' << tuple.first << ',' << tuple.second << ">,";
    cout << "}\n";
}

int main()
{
    unordered_set<pair<int, int>, pair_hash> last = {};
    while (true)
    {
        cout << "choose operation:\n1.Inversion\n2.Composition\n3.Oblast opredeleniya\n4.Mnozhestvo znacheniy\n";
        int inp;
        cin >> inp;
        if (cin.fail())
        {
            cout << "wrong input\n";
            return 1;
        }
        unordered_set<pair<int, int>, pair_hash> graph1;
        unordered_set<pair<int, int>, pair_hash> graph2;
        unordered_set<int> res;
        int n;
        switch (inp)
        {
        case 1:
            cout << "Enter the number of pairs (write 0 to use previous result):\n";
            cin >> n;
            if (cin.fail())
            {
                cout << "wrong input\n";
                return 1;
            }

            if (n == 0)
                graph1 = last;
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    pair<int, int> p;
                    cin >> p.first >> p.second;
                    graph1.insert(p);
                }
                if (cin.fail())
                {
                    cout << "wrong input\n";
                    return 1;
                }
            }
            last = inversion(graph1);
            print_graph(last);
            break;
        case 2:
            cout << "Enter the number of pairs for the first graphic (write 0 to use previous result):\n";
            cin >> n;
            if (cin.fail())
            {
                cout << "wrong input\n";
                return 1;
            }

            if (n == 0)
                graph1 = last;
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    pair<int, int> p;
                    cin >> p.first >> p.second;
                    graph1.insert(p);
                }
                if (cin.fail())
                {
                    cout << "wrong input\n";
                    return 1;
                }
            }
            cout << "Enter the number of pairs for the second graphic (write 0 to use previous result):\n";
            cin >> n;
            if (cin.fail())
            {
                cout << "wrong input\n";
                return 1;
            }

            if (n == 0)
                graph2 = last;
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    pair<int, int> p;
                    cin >> p.first >> p.second;
                    graph2.insert(p);
                }
                if (cin.fail())
                {
                    cout << "wrong input\n";
                    return 1;
                }
            }
            last = composition(graph1, graph2);
            print_graph(last);
            break;

        case 3:
            cout << "Enter the number of pairs for graphic (write 0 to use previous result):\n";
            cin >> n;
            if (cin.fail())
            {
                cout << "wrong input\n";
                return 1;
            }

            if (n == 0)
                graph1 = last;
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    pair<int, int> p;
                    cin >> p.first >> p.second;
                    graph1.insert(p);
                }
                if (cin.fail())
                {
                    cout << "wrong input\n";
                    return 1;
                }
            }
            res = projection1(graph1);
            cout << '{';
            for (int i : res)
                cout << i << ',';
            cout << "}\n";
            last = graph1;
            break;
        case 4:
            cout << "Enter the number of pairs for graphic (write 0 to use previous result):\n";

            cin >> n;
            if (cin.fail())
            {
                cout << "wrong input\n";
                return 1;
            }

            if (n == 0)
                graph1 = last;
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    pair<int, int> p;
                    cin >> p.first >> p.second;
                    graph1.insert(p);
                }
                if (cin.fail())
                {
                    cout << "wrong input\n";
                    return 1;
                }
            }
            res = projection2(graph1);
            cout << '{';
            for (int i : res)
                cout << i << ',';
            cout << "}\n";
            last = graph1;
            break;
        default:
            cout << "You should choose number from 1 to 4\n";
            break;
        }
    }
    return 0;
}