#ifndef TRANSFORMERS_H
#define TRANSFORMERS_H

#include "data.h"
#include "arqs.h"

void drawTransformInfo(int isSelected, int IdSelectedPoint, int IdSelectedLine, int IdSelectedPolygon, TransformMode currentTransform, float rotationAngle, float scaleFactorX, float scaleFactorY,float shearFactorX,float shearFactorY);
float distance(Point p1, Point p2);
float pointToLineDistance(Point p, Line l);
int isPointInPolygon(Point p, Mesh m);
int selectPoint(int x, int y, int pointCount,Point *points);
int selectLine(int x, int y, int lineCount,Line *lines);
int selectPolygon(int x, int y, int meshCount, Mesh *meshes);
void translatePoint(int id, float dx, float dy, Point *points);
void translateLine(int id, float dx, float dy, Line *lines);
void translatePolygon(int id, float dx, float dy, Mesh *meshes);
void rotatePoint(int id, float angle, Point * points);
void rotatePointAroundCenter(Point *p, Point center, float angle);
void rotateLine(int id, float angle, Line *lines);
void rotatePolygon(int id, float angle, Mesh *meshes);
void scalePointAroundCenter(Point *p, Point center, float sx, float sy);
void scaleLine(int id, float sx, float sy,  Line *lines);
void scalePolygon(int id, float sx, float sy, Mesh *meshes);
void mirrorPointAroundAxis(Point *p, Point center, int direction);
void mirrorPoint(int id, int direction, Point *points);
void mirrorLine(int id, int direction, Line *lines);
void mirrorPolygon(int id, int direction, Mesh *meshes);
void shearPointAroundCenter(Point *p, Point center, float sx, float sy);
void shearPoint(int id, float sx, float sy, Point *points);
void shearLine(int id, float sx, float sy, Line *lines);
void shearPolygon(int id, float sx, float sy,Mesh *meshes);

#endif // TRANSFORMERS_H
