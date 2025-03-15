#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_POLYGON_POINTS 100
#define MAX_SHAPES 1000

typedef struct {
    float x, y;
} Point;

typedef enum { DRAW, LINE, SQUARE, POLYGON } Mode;

typedef struct {
    Mode type;
    Point p1, p2;
    Point* polygon;  
    int polygonSize;
} Shape;

Shape* shapes = NULL;
int shapeCount = 0;
Mode currentMode = DRAW;
int drawing = 0;
Point tempP1, tempP2;

Shape* tempPolygon;
int polygonDrawing = 0;

void initShapes() {
    shapes = (Shape*)malloc(MAX_SHAPES * sizeof(Shape));
    tempPolygon = (Shape*)malloc(sizeof(Shape));
    tempPolygon->polygon = (Point*)malloc(MAX_POLYGON_POINTS * sizeof(Point));
    tempPolygon->polygonSize = 0;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);

    glPointSize(3.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].type == DRAW) {
            glVertex2f(shapes[i].p1.x, shapes[i].p1.y);
        }
    }
    glEnd();

    glBegin(GL_LINES);
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].type == LINE) {
            glVertex2f(shapes[i].p1.x, shapes[i].p1.y);
            glVertex2f(shapes[i].p2.x, shapes[i].p2.y);
        }
    }
    glEnd();

    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].type == SQUARE) {
            glBegin(GL_LINE_LOOP);
            glVertex2f(shapes[i].p1.x, shapes[i].p1.y);
            glVertex2f(shapes[i].p2.x, shapes[i].p1.y);
            glVertex2f(shapes[i].p2.x, shapes[i].p2.y);
            glVertex2f(shapes[i].p1.x, shapes[i].p2.y);
            glEnd();
        } else if (shapes[i].type == POLYGON) {
            glBegin(GL_LINE_LOOP);
            for (int j = 0; j < shapes[i].polygonSize; j++) {
                glVertex2f(shapes[i].polygon[j].x, shapes[i].polygon[j].y);
            }
            glEnd();
        }
    }

    if (drawing) {
        glColor3f(0.5, 0.5, 0.5);
        if (currentMode == LINE) {
            glBegin(GL_LINES);
            glVertex2f(tempP1.x, tempP1.y);
            glVertex2f(tempP2.x, tempP2.y);
            glEnd();
        } else if (currentMode == SQUARE) {
            glBegin(GL_LINE_LOOP);
            glVertex2f(tempP1.x, tempP1.y);
            glVertex2f(tempP2.x, tempP1.y);
            glVertex2f(tempP2.x, tempP2.y);
            glVertex2f(tempP1.x, tempP2.y);
            glEnd();
        }
    }

    if (polygonDrawing) {
        glColor3f(0.5, 0.5, 0.5);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < tempPolygon->polygonSize; i++) {
            glVertex2f(tempPolygon->polygon[i].x, tempPolygon->polygon[i].y);
        }
        glVertex2f(tempP2.x, tempP2.y);
        glEnd();
    }

    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    float normX = (float)x / (glutGet(GLUT_WINDOW_WIDTH) / 2.0f) - 1.0f;
    float normY = -(float)y / (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f) + 1.0f;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (currentMode == DRAW) {
            if (shapeCount < MAX_SHAPES) {
                shapes[shapeCount].type = DRAW;
                shapes[shapeCount].p1.x = normX;
                shapes[shapeCount].p1.y = normY;
                shapeCount++;
            }
        } else if (currentMode == POLYGON) {
            if (tempPolygon->polygonSize < MAX_POLYGON_POINTS) {
                tempPolygon->polygon[tempPolygon->polygonSize++] = (Point){normX, normY};
                polygonDrawing = 1;
            }
        } else {
            tempP1.x = normX;
            tempP1.y = normY;
            tempP2 = tempP1;
            drawing = 1;
        }
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (drawing && (currentMode == LINE || currentMode == SQUARE)) {
            if (shapeCount < MAX_SHAPES) {
                Shape newShape = {currentMode, tempP1, tempP2, NULL, 0};
                shapes[shapeCount++] = newShape;
            }
            drawing = 0;
        }
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (polygonDrawing) {
            if (shapeCount < MAX_SHAPES) {
                shapes[shapeCount] = *tempPolygon;
                shapes[shapeCount].type = POLYGON;
                shapeCount++;
            }
            polygonDrawing = 0;
            tempPolygon->polygonSize = 0;
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    float normX = (float)x / (glutGet(GLUT_WINDOW_WIDTH) / 2.0f) - 1.0f;
    float normY = -(float)y / (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f) + 1.0f;

    if (drawing) {
        tempP2.x = normX;
        tempP2.y = normY;
    } else if (polygonDrawing) {
        tempP2.x = normX;
        tempP2.y = normY;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'c') {
        free(shapes); 
        shapes = NULL;
        shapeCount = 0;
        tempPolygon->polygonSize = 0;
        polygonDrawing = 0;
        drawing = 0;
        glutPostRedisplay();
    } else if (key == 'l') {
        currentMode = LINE;
        drawing = 0;
        polygonDrawing = 0;
    } else if (key == 'q') {
        currentMode = SQUARE;
        drawing = 0;
        polygonDrawing = 0;
    } else if (key == 'd') {
        currentMode = DRAW;
        drawing = 0;
        polygonDrawing = 0;
    } else if (key == 'p') {
        currentMode = POLYGON;
        drawing = 0;
    }
}

void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Paint OpenGL");

    initShapes();
    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
