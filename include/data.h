#ifndef DATA_H
#define DATA_H

#define MAX_POLYGON_POINTS 100
#define MAX_SHAPES 1000
#define CIRCLE_SEGMENTS 12
#define POINT_RADIUS 3.0f
#define PI 3.14159265358979323846
#define SELECTION_THRESHOLD 10.0

#define JOYSTICK_DOWN 0
#define JOYSTICK_X 2
#define JOYSTICK_CIRCLE 4
#define JOYSTICK_TRIANGLE 8
#define JOYSTICK_QUAD 1
#define JOYSTICK_L1 16
#define JOYSTICK_R1 32
#define JOYSTICK_L3 1024
#define JOYSTICK_R3 2048
#define JOYSTICK_START 256 
#define JOYSTICK_R2 128
#define JOYSTICK_OPT 512
#define JOYSTICK_L2 64 
#define JOYSTICK_ICON_BUTTON 4096
#define JOYSTICK_TOUCH 8192

typedef structk
{
    float x, y;
} Point;

typedef struct
{
    Point init, end;
} Line;

typedef struct
{
    int numberPoints;
    Point vertices[MAX_POLYGON_POINTS];
} Mesh;

typedef enum
{
    VERTICE,
    LINE,
    POLYGON,
    NONE_MESH,
    SELECTION
} Mode;

typedef enum
{
    SAVE_MESH,
    LOAD_MESH,
    DELETED,
    NONE_MESSAGE,
    IN_CONCERT,
    LINE_LOG,
    VERTCIE_LOG,
    POLYLGON_LOG
} MESSAGE;

typedef enum
{
    NONE_TRANSFORMER,
    TRANSLATE,
    ROTATE,
    SCALE,
    MIRROR,
    SHEAR
} TransformMode;

typedef enum {
    CAT_IDLE, CAT_TRANSLATE, CAT_ROTATE, CAT_SCALE, CAT_MIRROR, CAT_SHEAR
} CatAnimationState;



//==================================================== VARIAVEIS ============================================================

void initLines();
void initPoints();
void initPolygons();
void display();
void specialKeys(int key, int x, int y);
void init();
void drawMessage();
int *windowSize();
void hideMessage(int value);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void timer(int value);
void passiveMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void saveScreenshotBMP(const char *filename, int width, int height);
void idleFunc();

void drawCat();
void updateCatAnimation();
void setCatState(CatAnimationState state);
void idleFunc();


#endif
