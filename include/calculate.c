#include <calculate.h>

Point calculateLineCenter(Line l)
{
    Point center;
    center.x = (l.init.x + l.end.x) / 2.0f;
    center.y = (l.init.y + l.end.y) / 2.0f;
    return center;
}

Point calculatePolygonCenter(Mesh m)
{
    Point center = {0, 0};
    for (int i = 0; i < m.numberPoints; i++)
    {
        center.x += m.vertices[i].x;
        center.y += m.vertices[i].y;
    }
    center.x /= m.numberPoints;
    center.y /= m.numberPoints;
    return center;
}

float distance(Point p1, Point p2)
{
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}