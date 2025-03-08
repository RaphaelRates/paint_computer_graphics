#include <GL/glut.h>
#include <math.h>


void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y - height);
    glVertex2f(x, y - height);
    glEnd();
}

void drawDiamond(float cx, float cy, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(cx, cy + height / 2);
    glVertex2f(cx + width / 2, cy);
    glVertex2f(cx, cy - height / 2);
    glVertex2f(cx - width / 2, cy);
    glEnd();
}

void drawCircle(float cx, float cy, float r, int num_segments, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawRectangle(-0.9f, 0.9f, 1.8f, 1.8f, 0.0f, 0.6f, 0.2f);
    drawDiamond(0.0f, 0.0f, 1.2f, 0.9f, 1.0f, 0.8f, 0.0f);
    drawCircle(0.0f, 0.0f, 0.35f, 100, 0.0f, 0.0f, 1.0f);
    
    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Bandeira do Brasil");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}