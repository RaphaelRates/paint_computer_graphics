#ifndef TRANSFORMERS_H
#define TRANSFORMERS_H

#include "data.h"

void translatePoint(int id, float dx, float dy);
void translateLine(int id, float dx, float dy);
void translatePolygon(int id, float dx, float dy);
void rotatePoint(int id, float angle);
void rotateLine(int id, float angle);
void rotatePolygon(int id, float angle);
void scaleLine(int id, float sx, float sy);
void scalePolygon(int id, float sx, float sy);
void mirrorPoint(int id, int direction);
void mirrorLine(int id, int direction);
void mirrorPolygon(int id, int direction);
void shearPoint(int id, float sx, float sy);
void shearLine(int id, float sx, float sy);
void shearPolygon(int id, float sx, float sy);

#endif
