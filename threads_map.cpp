#include <iostream>
#include <string>
#include <thread>
#include "utilities/utilities.h"
#include "utilities/utimer.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    utimer t0("total");
    if (argc < 4)
    {
        cerr << "Usage: " << argv[0] << " <'K' for KNN> <'W' for number of workers> <'N' for number of points>" << endl;
        exit(-1);
    }

    int k = atoi(argv[1]);
    int nworkers = atoi(argv[2]);
    string filename = "data/input_" + string(argv[3]) + ".csv";
    string outputs = "";

    vector<Point> points;

    {
        utimer t1("read_points");
        points = read_points(filename);
    }

    int size = points.size();
    vector<string> local(size);

    auto knn = [&points, &local](const pair<int, int> range, const int k)
    {
        for (int i = range.first; i < range.second; i++)
        {
            vector<pair<int, float>> neighbours;
            for (int j = 0; j < points.size(); j++)
            {
                if (i == j)
                    continue;
                neighbours.push_back(make_pair(j, points[i].squaredEuclideanDistance(points[j])));
            }
            local[i] = write_neighbours(make_pair(i, kClosest_nth_element(neighbours, k)));
        }
        return;
    };

    {
        utimer t3("knn");
        vector<thread> threads;
        vector<pair<int, int>> ranges(nworkers);
        int delta = points.size() / nworkers;

        {
            utimer t4("split");
            for (int i = 0; i < nworkers; i++)
            {
                ranges[i] = make_pair(i * delta, (i != (nworkers - 1) ? (i + 1) * delta : points.size()));
                threads.push_back(thread(knn, ranges[i], k));
            }
        }

        for (thread &t : threads)
            t.join();

        for (int i = 0; i < size; i++)
            outputs.append(local[i]);
    }

    {
        utimer t5("write_results");
        string filename = "threads_map_" + string(argv[3]) + "_res.txt";
        ofstream out(filename);
        if (!out.is_open())
        {
            cerr << "Can't open file " << filename << endl;
            exit(-1);
        }

        out << outputs << endl;
        out.close();
    }
    return 0;
}