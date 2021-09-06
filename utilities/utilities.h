#include <vector>
#include <iterator>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "point.h"

std::vector <Point> read_points(std::string file_name);
std::string write_neighbours(std::pair<int, std::vector<int>> p);
std::vector<int> kClosest_nth_element(std::vector<std::pair<int, float>> &A, int K);
std::vector<int> kClosest_partial_sort(std::vector<std::pair<int, float>> &A, int K);
std::vector<int> kClosest_quick_select(std::vector<std::pair<int, float>> &A, int K);
int partition(std::vector<std::pair<int, float>> &A, int low, int high);