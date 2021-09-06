#ifndef KNN_POINT_H
#define KNN_POINT_H
#include <cmath>
#include <iostream>

class Point
{
public:
    Point(int id, float x, float y);
    Point();
    float squaredEuclideanDistance(const Point &point);
    friend std::istream &operator>>(std::istream &in, Point &point);
    int id;
    double x, y;
};

#endif
