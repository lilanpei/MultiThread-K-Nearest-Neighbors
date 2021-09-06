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
    vector<string> local(size);

    auto knn = [&points, &local](int low, int high, int k)
    {
        for (int i = low; i < high; i++)
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
        vector<thread> workers;
        int counter = 0;
        while (true)
        {
            if (counter < size)
            {
                workers.push_back(thread(knn, counter, min(counter + delta, size), k));
                counter += delta;
                if (workers.size() == nworkers)
                {
                    for (std::thread &t : workers)
                    {
                        try
                        {
                            t.join();
                        }
                        catch (const std::exception &e)
                        {
                            std::cout << "error : " << e.what() << std::endl;
                        }
                    }
                    workers.clear();
                }
            }
            else
            {
                break;
            }
        }

        if (workers.size())
        {
            for (std::thread &t : workers)
            {
                try
                {
                    t.join();
                }
                catch (const std::exception &e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        }

        for (int i = 0; i < size; i++)
            outpus.append(local[i]);
    }

    {
        utimer t5("write_results");
        string filename = "threads_barrier_" + n_points + "_res.txt";
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