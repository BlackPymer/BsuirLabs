#include <iostream>
using namespace std;
int main()
{
    int **a, m, n, *min_row, *max_row, *min_column, *max_column;
    cin >> m >> n;
    a = new int *[m];

    min_row = new int[m];
    max_row = new int[m];
    min_column = new int[n];
    max_column = new int[n];

    for (int i = 0; i < m; ++i)
    {
        a[i] = new int[n];
        for (int j = 0; j < n; ++j)
        {
            cin >> a[i][j];
            if (j == 0)
            {
                min_row[i] = j;
                max_row[i] = j;
            }
            if (i == 0)
            {
                min_column[j] = i;
                max_column[j] = i;
            }
            if (a[i][j] < a[i][min_row[i]])
                min_row[i] = j;
            if (a[i][j] > a[i][max_row[i]])
                max_row[i] = j;
            if (a[i][j] < a[min_column[j]][j])
                min_column[j] = i;
            if (a[i][j] > a[max_column[j]][j])
                max_column[j] = i;
        }
    }
    for (int i = 0; i < m; ++i)
    {
        if (max_column[min_row[i]] == i)
            cout << '(' << i << ", " << min_row[i] << ")\n";
        if (min_column[max_row[i]] == i)
            cout << '(' << i << ", " << max_row[i] << ")\n";
    }
    delete[] a, min_row, min_column, max_row, max_column;
    return 0;
}