#include <iostream>
#include <string>
#include <thread>
#include <ff/ff.hpp>
#include "utilities/utilities.h"
#include "utilities/utimer.cpp"
#include <ff/parallel_for.hpp>

using namespace std;
using namespace ff;

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
    ParallelForReduce<string> pfr(nworkers);
    vector<Point> points;

    {
        utimer t1("read_points");
        points = read_points(filename);
    }

    int size = points.size();

    {
        utimer t3("knn");
        pfr.parallel_reduce(
            outputs, "",
            0, points.size(),
            1,
            0, // static partitioning
            [&](const long i, string &local)
            {
                vector<pair<int, float>> neighbours;
                for (int j = 0; j < points.size(); j++)
                {
                    if (i == j)
                        continue;
                    neighbours.push_back(make_pair(j, points[i].squaredEuclideanDistance(points[j])));
                }
                local.append(write_neighbours(make_pair(i, kClosest_nth_element(neighbours, k))));
            },
            [](string &final, const string &partial)
            { final.append(partial); });
    }

    {
        utimer t4("write_results");
        string filename = "ff_parallel_reduce_" + string(argv[3]) + "_res.txt";
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