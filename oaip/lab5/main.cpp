#include <iostream>

void clear_memory(double **arr, int n)
{
    for (int i = 0; i < n; ++i)
    {
        delete[] arr[i];
    }
    delete[] arr;
}
void print_arr(double **arr, int n, int m)
{
    std::cout << "{\n";
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            std::cout << arr[i][j] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "}\n";
}
int main()
{
    while (true)
    {
        double **arr;
        int n, m;
        std::cin >> n >> m;
        if (std::cin.fail())
        {
            std::cout << "n and m must be integers\n";
            std::cin.clear();
            std::cin.ignore(9e9, '\n');
            continue;
        }
        if (n <= 0 || m <= 0)
        {
            std::cout << "n and m must be positive\n";
            continue;
        }
        arr = new double *[n];
        for (int i = 0; i < n; ++i)
        {
            arr[i] = new double[m]();
        }

        bool inputGood = true;
        double min_el_val = __DBL_MAX__;
        int min_el_row = -1;
        double max_el_val = __DBL_MIN__;
        int max_el_row = -1;
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < m; ++j)
            {
                std::cin >> arr[i][j];
                if (std::cin.fail())
                {
                    std::cout << "array's elements must be doubles\n";
                    std::cin.clear();
                    std::cin.ignore(9e9, '\n');
                    inputGood = false;
                    break;
                }
                if (min_el_val >= arr[i][j])
                {
                    min_el_val = arr[i][j];
                    min_el_row = i;
                }
                if (max_el_val <= arr[i][j])
                {
                    max_el_val = arr[i][j];
                    max_el_row = i;
                }
            }
            if (!inputGood)
                break;
        }
        if (!inputGood)
        {
            clear_memory(arr, n);
            continue;
        }
        std::cout << "your entered array:\n";
        print_arr(arr, n, m);
        if (min_el_row != max_el_row)
        {
            double *tmp = arr[min_el_row];
            arr[min_el_row] = arr[max_el_row];
            arr[max_el_row] = tmp;
        }
        std::cout << "result after swap:\n";
        print_arr(arr, n, m);
        clear_memory(arr, n);
    }
    return 0;
}