#include "point.h"

Point::Point(int id, float x, float y) : id(id), x(x), y(y) {}

Point::Point() { Point(-1, 0, 0); }

float Point::squaredEuclideanDistance(const Point &point)
{
    float x = this->x - point.x;
    float y = this->y - point.y;

    return pow(x, 2) + pow(y, 2);
}

std::istream &operator>>(std::istream &is, Point &point)
{
    is >> point.x >> point.y;
    return is;
}