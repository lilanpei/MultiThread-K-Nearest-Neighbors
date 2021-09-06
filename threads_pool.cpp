#include <iostream>
#include <string>
#include <thread>
#include "utilities/utilities.h"
#include "utilities/utimer.cpp"
#include "utilities/thread_pool.h"

using namespace std;

string knn(int low, int high, vector<Point> points, int k)
{
    string res = "";
    for (int i = low; i < high; i++)
    {
        vector<pair<int, float>> neighbours;
        for (int j = 0; j < points.size(); j++)
        {
            if (i == j)
                continue;
            neighbours.push_back(make_pair(j, points[i].squaredEuclideanDistance(points[j])));
        }
        res.append(write_neighbours(make_pair(i, kClosest_nth_element(neighbours, k))));
    }
    return res;
}

int main(int argc, char *argv[])
{
    utimer t0("total");
    if (argc < 4)
    {
        cerr << "Usage: " << argv[0] << " <'K' for KNN> <'W' for number of workers> <'S' for chunk size> <'N' for number of points>" << endl;
        exit(-1);
    }

    int k = atoi(argv[1]);
    int nworkers = atoi(argv[2]);
    int delta = max(nworkers, 32);
    string n_points = "";
    string filename = "data/input_";

    if (argc == 5)
    {
        delta = atoi(argv[3]);
        n_points = string(argv[4]);
        filename += n_points + ".csv";
    }
    else
    {
        n_points = string(argv[3]);
        filename += n_points + ".csv";
    }

    cout << "Chunk-size: " << delta << endl;

    string outpus = "";
    vector<Point> points;

    {
        utimer t1("read_points");
        points = read_points(filename);
    }

    int size = points.size();

    {
        utimer t3("knn");
        ThreadPool p(nworkers);
        vector<future<string>> results;

        for (int i = 0; i < size; i+=delta)
            results.emplace_back(p.Enqueue(knn, i, min(i + delta, size), points, k));

        for (auto && result : results)
            outpus.append(result.get());
    }

    {
        utimer t5("write_results");
        string filename = "threads_pool_" + n_points + "_res.txt";
        ofstream out(filename);
        if (!out.is_open())
        {
            cerr << "Can't open file " << filename << endl;
            exit(-1);
        }

        out << outpus << endl;
        out.close();
    }
    return 0;
}