#ifndef DRAW_H
#define DRAW_H

#include <GL/glut.h>
#include "data.h"

void drawPoint(Point *points, int pointCount, int isSelected, int IdSelectedPoint);
void drawLines(Line *lines, int lineCount, int IdSelectedLine, int isSelected);
void drawPolygon(Mesh *meshes, int meshCount, int IdSelectedPolygon, int isSelected);
void drawPreviewPoint(Mode currentMode, int isDrawing, Point tempPoint);
void drawPreviewLine(Mode currentMode, int isDrawing, Point tempPoint, float colorLoading_r, float colorLoading_g, float colorLoading_b, Line tempLine);
void drawPreviewPolygon(int isDrawingPolygon, float colorLoading_r, float colorLoading_g, float colorLoading_b, Mesh tempMesh);
void drawSelectedObject(int isSelected, int IdSelectedPoint,int IdSelectedLine, int IdSelectedPolygon, Point *points,Line *lines, Mesh *meshes);
void drawMessage(MESSAGE showMessage);

#endif
