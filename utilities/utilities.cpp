#include "utilities.h"
#include "point.h"
#include "utimer.cpp"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <string>

using namespace std;

vector <Point> read_points(string file_name)
{
    ifstream file(file_name);
    vector <Point> points;

    if (file.is_open())
    {
        int idx = 0;
        Point p;
        while (file >> p)
        {
            p.id = idx++;
            points.push_back(p);
        }

        file.close();
        return points;
    }
    else
    {
        cerr << "Can't open file " << file_name << endl;
        exit(-1);
    }
}

string write_neighbours(pair<int, std::vector<int>> p)
{
    stringstream neighbours;
    copy(p.second.begin(), p.second.end(), ostream_iterator<int>(neighbours, " "));
    return to_string(p.first) + ": " + neighbours.str() + "\n";
}

vector<int> kClosest_nth_element(vector<pair<int, float>> &A, int K)
{
    vector<int> res_ids;

    nth_element(A.begin(), A.begin() + K, A.end(), [](pair<int, float> &a, pair<int, float> &b)
                { return a.second < b.second; });

    sort(A.begin(), A.begin() + K, [](auto &a, auto &b)
              { return a.second < b.second; });

    transform(A.begin(), A.begin() + K, std::back_inserter(res_ids), [](auto const &pair)
                { return pair.first; });

    return res_ids;
}

vector<int> kClosest_partial_sort(vector<pair<int, float>> &A, int K)
{
    vector<int> res_ids;

    partial_sort(A.begin(), A.begin() + K, A.end(), [](pair<int, float> &a, pair<int, float> &b)
                 { return a.second < b.second; });

    transform(A.begin(), A.begin() + K, std::back_inserter(res_ids), [](auto const &pair)
              { return pair.first; });

    return res_ids;
}

// Simpy implementation of nth element algorithm (Quick select)
vector<int> kClosest_quick_select(vector<pair<int, float>> &A, int K)
{
    srand(42);
    //srand((unsigned)time(NULL));
    int l = 0, r = A.size() - 1;
    vector<int> res(K);

    while (true)
    {
        int p = partition(A, l, r);
        if (p + 1 == K)
            break;
        if (p + 1 < K)
            l = p + 1;
        else
            r = p - 1;
    }

    sort(A.begin(), A.begin() + K, [](auto &left, auto &right)
         { return left.second < right.second; });

    for (int i = 0; i < K; i++)
        res[i] = A[i].first;

    return res;
}

int partition(vector<pair<int, float>> &A, int low, int high)
{
    int p = rand() % (high - low + 1) + low;

    swap(A[high], A[p]);
    float pivot = A[high].second;

    for (int i = low; i < high; ++i)
    {
        if (A[i].second < pivot)
            swap(A[i], A[low++]);
    }
    swap(A[high], A[low]);
    return low;
}