#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>

#define MAX_POINTS 100

// Estrutura para armazenar pontos ou linhas
typedef struct {
    float x1, y1, x2, y2;
    int isLine; // 0 para ponto, 1 para linha
    float scale;
    float rotation;
} Shape;

Shape shapes[MAX_POINTS];
int shapeCount = 0;
int selectedShape = -1;
int drawingLine = 0;
int isFirstClick = 1;
float tempX, tempY;

// Função para desenhar os pontos e linhas
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < shapeCount; i++) {
        glPushMatrix();

        float midX = (shapes[i].x1 + shapes[i].x2) / 2.0f;
        float midY = (shapes[i].y1 + shapes[i].y2) / 2.0f;

        glTranslatef(midX, midY, 0.0f);
        glScalef(shapes[i].scale, shapes[i].scale, 1.0f);
        glRotatef(shapes[i].rotation, 0.0f, 0.0f, 1.0f);
        glTranslatef(-midX, -midY, 0.0f);

        if (i == selectedShape)
            glColor3f(1.0, 1.0, 0.0);
        else
            glColor3f(0.0, 1.0, 0.0);

        if (shapes[i].isLine) {
            glBegin(GL_LINES);
                glVertex2f(shapes[i].x1, shapes[i].y1);
                glVertex2f(shapes[i].x2, shapes[i].y2);
            glEnd();
        } else {
            glBegin(GL_POLYGON);
                glVertex2f(shapes[i].x1 - 0.02f, shapes[i].y1 - 0.02f);
                glVertex2f(shapes[i].x1 + 0.02f, shapes[i].y1 - 0.02f);
                glVertex2f(shapes[i].x1 + 0.02f, shapes[i].y1 + 0.02f);
                glVertex2f(shapes[i].x1 - 0.02f, shapes[i].y1 + 0.02f);
            glEnd();
        }

        glPopMatrix();
    }
    glutSwapBuffers();
}

float screenToWorldX(int x) {
    return (x / (float)glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f;
}

float screenToWorldY(int y) {
    return 1.0f - (y / (float)glutGet(GLUT_WINDOW_HEIGHT)) * 2.0f;
}

void selectNearestShape(float worldX, float worldY) {
    float minDistance = 0.1f; // Distância máxima para selecionar
    selectedShape = -1;

    for (int i = 0; i < shapeCount; i++) {
        float dx, dy, distance;

        if (shapes[i].isLine) {
            // Distância do ponto ao centro da linha
            float midX = (shapes[i].x1 + shapes[i].x2) / 2.0f;
            float midY = (shapes[i].y1 + shapes[i].y2) / 2.0f;
            dx = midX - worldX;
            dy = midY - worldY;
        } else {
            dx = shapes[i].x1 - worldX;
            dy = shapes[i].y1 - worldY;
        }

        distance = sqrt(dx * dx + dy * dy);

        if (distance < minDistance) {
            minDistance = distance;
            selectedShape = i;
        }
    }
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        float worldX = screenToWorldX(x);
        float worldY = screenToWorldY(y);

        if (button == GLUT_LEFT_BUTTON) {
            if (drawingLine) {
                if (isFirstClick) {
                    tempX = worldX;
                    tempY = worldY;
                    isFirstClick = 0;
                } else {
                    shapes[shapeCount++] = (Shape){tempX, tempY, worldX, worldY, 1, 1.0f, 0.0f};
                    isFirstClick = 1;
                }
            } else {
                shapes[shapeCount++] = (Shape){worldX, worldY, worldX, worldY, 0, 1.0f, 0.0f};
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            selectNearestShape(worldX, worldY);
        }

        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'l': drawingLine = 1; break;
        case 'p': drawingLine = 0; break;
        case '+': if (selectedShape != -1) shapes[selectedShape].scale += 0.1f; break;
        case '-': if (selectedShape != -1 && shapes[selectedShape].scale > 0.1f) shapes[selectedShape].scale -= 0.1f; break;
        case 'r': if (selectedShape != -1) shapes[selectedShape].rotation -= 10.0f; break;
        case 't': if (selectedShape != -1) shapes[selectedShape].rotation += 10.0f; break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Desenho de Pontos e Linhas");

    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
