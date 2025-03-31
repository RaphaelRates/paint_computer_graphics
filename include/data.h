#ifndef DATA_H
#define DATA_H

#define MAX_POLYGON_POINTS 100
#define MAX_SHAPES 1000
#define SELECTION_THRESHOLD 10.0

typedef struct {
    float x, y;
} Point;

typedef struct {
    Point init, end;
} Line;

typedef struct {
    int numberPoints;
    Point vertices[MAX_POLYGON_POINTS];
} Mesh;

typedef enum {
    VERTICE,
    LINE,
    POLYGON,
    NONE_MESH
} Mode;

typedef enum {
    SAVE_MESH,
    LOAD_MESH,
    DELETED,
    NONE_MESSAGE
} MESSAGE;

typedef enum {
    NONE_TRANSFORMER,
    TRANSLATE,
    ROTATE,
    SCALE,
    MIRROR,
    SHEAR
} TransformMode;

// Variáveis globais
extern Point *points;
extern Mesh *meshes;
extern Line *lines;
extern int pointCount, lineCount, meshCount;
extern Mode currentMode;
extern MESSAGE showMessage;

#endif
