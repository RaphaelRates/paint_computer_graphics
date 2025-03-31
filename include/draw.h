#ifndef DRAW_H
#define DRAW_H

#include <GL/glut.h>
#include "data.h"

void display();
void drawPoint();
void drawLines();
void drawPolygon();
void drawPreviewPoint();
void drawPreviewLine();
void drawPreviewPolygon();
void drawSelectedObject();
void drawMessage();

#endif
