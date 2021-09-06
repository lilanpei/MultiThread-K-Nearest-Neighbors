#include <iostream>
#include <string>
#include <thread>
#include "utilities/utilities.h"
#include "utilities/utimer.cpp"
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

using namespace ff;
using namespace std;

struct Task_t
{
    Task_t(vector<Point> points, string local, int start, int end, int k)
        : points(points), local(local), start(start), end(end), k(k) {}
    vector<Point> points;
    string local;
    int start;
    int end;
    int k;
};

void knn(vector<Point> points, string &local, int start, int end, int k)
{
    for (int i = start; i < end; i++)
    {
        vector<pair<int, float>> neighbours;
        for (int j = 0; j < points.size(); j++)
        {
            if (i == j)
                continue;
            neighbours.push_back(make_pair(j, points[i].squaredEuclideanDistance(points[j])));
        }
        local.append(write_neighbours(make_pair(i, kClosest_nth_element(neighbours, k))));
    }
    return ;
}

struct firstThirdStage : ff_node_t<Task_t>
{
    firstThirdStage(vector<Point> points, string local, int delta, int k)
        : points(points), local(local), delta(delta), k(k) {}
    Task_t *svc(Task_t *task)
    {
        int size = points.size();
        if (task == nullptr)
        {
            for (int i = 0; i < size; i += delta)
            {
                Task_t *task = new Task_t(points, local, i, min(i + delta, size), k);
                ff_send_out(task);
            }
            return GO_ON;
        }
        //std::cout << "thirdStage received " << local << "\n";
        outpus.append(task->local);
        delete task;
        ntasks += delta;
        if (ntasks >= points.size())
            return EOS;
        return GO_ON;
    }
    void svc_end() 
    {
        //std::cout << "ntasks = " << ntasks << "\n";
        {
            utimer t3("write_results");
            string filename = "ff_farm_" + to_string(points.size()) + "_res.txt";
            ofstream out(filename);
            if (!out.is_open())
            {
                cerr << "Can't open file " << filename << endl;
                exit(-1);
            }

            out << outpus << endl;
            out.close();
        }
    }
    vector<Point> points;
    string local;
    int delta;
    int k ;
    size_t ntasks = 0;
    string outpus = "";
};

struct secondStage : ff_node_t<Task_t>
{
    Task_t *svc(Task_t *task)
    {
        //std::cout << "secondStage received " << "\n";
        knn(task->points, task->local, task->start, task->end, task->k);
        return task;
    }
};

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
    string local = "";

    firstThirdStage firstthird(points, local, delta, k);

    std::vector<std::unique_ptr<ff_node>> W;
    for (size_t i = 0; i < nworkers; ++i)
        W.push_back(make_unique<secondStage>());

    ff_Farm<Task_t> farm(std::move(W), firstthird);
    farm.remove_collector(); // needed because the collector is present by default in the ff_Farm
    farm.wrap_around();      // this call creates feedbacks from Workers to the Emitter

    farm.set_scheduling_ondemand(); // optional

    ffTime(START_TIME);
    {
        utimer t2("knn");
        if (farm.run_and_wait_end() < 0)
        {
            error("running farm");
            return -1;
        }
    }
    ffTime(STOP_TIME);
    //std::cout << "Myff_farm time: " << ffTime(GET_TIME) << "\n";
    return 0;
}
