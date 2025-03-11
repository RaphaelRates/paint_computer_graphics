#include <GL/glut.h>
#include <stdio.h>

#define MAX_SHAPES 10000

typedef struct {
    float x, y;
} Point;

typedef enum { DRAW, LINE, SQUARE } Mode;

typedef struct {
    Mode type;
    Point p1, p2;
} Shape;

Shape shapes[MAX_SHAPES];
int shapeCount = 0;
Mode currentMode = DRAW;
int drawing = 0;
Point tempP1, tempP2;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);  // Preto

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
        }
    }

    if (drawing) {
        glColor3f(0.5, 0.5, 0.5);  // Cinza para pré-visualização

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

    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    float normX = (float)x / (glutGet(GLUT_WINDOW_WIDTH) / 2.0f) - 1.0f;
    float normY = -(float)y / (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f) + 1.0f;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            tempP1.x = normX;
            tempP1.y = normY;
            tempP2 = tempP1;
            drawing = 1;
        } else if (state == GLUT_UP && drawing) {
            if (shapeCount < MAX_SHAPES) {
                shapes[shapeCount].type = currentMode;
                shapes[shapeCount].p1 = tempP1;
                shapes[shapeCount].p2 = tempP2;
                shapeCount++;
            }
            drawing = 0;
            glutPostRedisplay();
        }
    }
}

void motion(int x, int y) {
    float normX = (float)x / (glutGet(GLUT_WINDOW_WIDTH) / 2.0f) - 1.0f;
    float normY = -(float)y / (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f) + 1.0f;

    if (drawing) {
        tempP2.x = normX;
        tempP2.y = normY;
        glutPostRedisplay();
    }

    if (currentMode == DRAW && shapeCount < MAX_SHAPES) {
        shapes[shapeCount].type = DRAW;
        shapes[shapeCount].p1.x = normX;
        shapes[shapeCount].p1.y = normY;
        shapeCount++;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'c') {
        shapeCount = 0;
        glutPostRedisplay();
    } else if (key == 'l') {
        currentMode = LINE;
    } else if (key == 'q') {
        currentMode = SQUARE;
    } else if (key == 'd') {
        currentMode = DRAW;
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

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
