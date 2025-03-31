#ifndef ARQS_H
#define ARQS_H

#include <stdio.h>
#include <stdlib.h>
#include "data.h"

int saveObjectsToFile(const char *filename,  int pointCount,int lineCount, int meshCount, Point *points,Line *lines ,Mesh *meshes);
int loadObjectsFromFile(const char *filename, int pointCount,int lineCount, int meshCount, Point *points,Line *lines ,Mesh *meshes, Mesh tempMesh);

#endif
