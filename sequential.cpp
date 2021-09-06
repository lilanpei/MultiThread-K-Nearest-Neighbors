#include <iostream>
#include <string>
#include "utilities/utilities.h"
#include "utilities/utimer.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    utimer t0("total");
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <'K' for KNN> <'N' for number of points>" << endl;
        exit(-1);
    }

    int k = atoi(argv[1]);
    string filename = "data/input_" + string(argv[2]) + ".csv";

    vector<Point> points;
    string outpus = "";

    {
        utimer t1("read_points");
        points = read_points(filename);
    }

    int size = points.size();

    {
        utimer t2("knn");

        for (int i = 0; i < size; i++)
        {
            vector<pair<int, float>> neighbours;

            for (int j = 0; j < size; j++)
            {
                if (i == j)
                    continue;
                neighbours.push_back(make_pair(j, points[i].squaredEuclideanDistance(points[j])));
            }

            outpus.append(write_neighbours(make_pair(i, kClosest_nth_element(neighbours, k))));
        }
    }

    {
        utimer t3("write results");
        string filename = "sequential_" + string(argv[2]) + "_res.txt";
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
