#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_POLYGON_POINTS 100
#define MAX_SHAPES 1000

/**
 * @struct Point
 * @property float y
 * @property float x
 */
typedef struct {
    float x, y;
} Point;

/**
 * @struct Line
 * @property Point init @details ponto inicial da linha
 * @property Point end @details ponto final da linha
 */
typedef struct {
    Point init, end;
} Line;

/**
 * @struct Mesh
 * @property int numberPoint @details numero de pontos no pligono
 * @property Point vertices @details dados dos vertices/pontos do poligono
 */
typedef struct {
    int numberPoints;
    Point vertices[MAX_POLYGON_POINTS];
} Mesh;

/**
 * @enum Mode
 * @details Define os modos de desenhos do software
 */
typedef enum {
    VERTICE, LINE, POLYGON, CIRCLE
} Mode;

//==================================================== VARIAVEIS ============================================================
/**
 * @details todos os pontos desenhados em memória
 */
Point *points = NULL;

/**
 * @details todos os meshes/poligonos desenhados em memória
 */
Mesh *meshes = NULL;


/**
 * @details todas as linhas desenhados em memória
 */
Line *lines = NULL;

/**
 * @details dados do mesh/poligono temporario 
 */
Mesh tempMesh;

/**
 * @details dados do ponto temporario 
 */
Point tempPoint;

/**
 * @details dados da linha temporaria
 */
Line tempLine;

int pointCount = 0, lineCount = 0, meshCount = 0;
int idDrawing = 0, isDrawingPolygon = 0, isSelected;

int IdsSelectedLine = -1;
int IdSelectedPoint = -1;
int IdSelectedPolygon = -1;

int isTranslating = 0;
int isRotating = 0;
int isScaling = 0;
int isMirroring = 0;
int isShearing = 0;

Mode currentMode = VERTICE;

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 */
void initLines();

/**
 * @details Inicia o espaço de memoria dos Pontos
 * @return none
 */
void initPoints();

/**
 * @details Inicia o espaço de memoria das Poligonos
 * @return none
 * @
 */
void initPolygons();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void display();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void drawPoint();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void drawLines();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void drawPolygon();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void drawPreviewPoint();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void drawPreviewLine();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void drawPreviewPolygon();

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void mouse(int button, int state, int x, int y);

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void motion(int x, int y);

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void keyboard(unsigned char key, int x, int y);

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void init();


void initLines() {
    if (lines == NULL) lines = (Line *)malloc(MAX_SHAPES * sizeof(Line));
}

void initPoints() {
    if (points == NULL) points = (Point *)malloc(MAX_SHAPES * sizeof(Point));
}

void initPolygons() {
    if(meshes == NULL) meshes = (Mesh *)malloc(MAX_SHAPES * sizeof(Mesh));
}

void drawPoint() {
    glBegin(GL_POINTS);
    for (int i = 0; i < pointCount; i++) {
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();
}

void drawLines() {
    glBegin(GL_LINES);
    for (int i = 0; i < lineCount; i++) {
        glVertex2f(lines[i].init.x, lines[i].init.y);
        glVertex2f(lines[i].end.x, lines[i].end.y);
    }
    glEnd();
}

void drawPolygon() {
    for (int i = 0; i < meshCount; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < meshes[i].numberPoints; j++) {
            glVertex2f(meshes[i].vertices[j].x, meshes[i].vertices[j].y);
        }
        glEnd();
    }
}

void drawPreviewPoint() {
    if (currentMode == VERTICE && idDrawing) {
        glColor3f(0.8f, 1.0f, 0.4f);
        glBegin(GL_POINTS);
            glVertex2f(tempPoint.x, tempPoint.y);
        glColor3f(0.0f, 0.0f, 0.0f);
        glEnd();
    }
}

void drawPreviewLine() {
    if (currentMode == LINE && idDrawing) {
        glColor3f(0.8f, 1.0f, 0.4f);
        glBegin(GL_LINES);
            glVertex2f(tempLine.init.x, tempLine.init.y);
            glVertex2f(tempLine.end.x, tempLine.end.y);
        glColor3f(0.0f, 0.0f, 0.0f);
        glEnd();
    }
}

void drawPreviewPolygon() {
    if (isDrawingPolygon) {
        glColor3f(0.8f, 1.0f, 0.4f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < tempMesh.numberPoints; i++) {
            glVertex2f(tempMesh.vertices[i].x, tempMesh.vertices[i].y);
        }
        glColor3f(0.0f, 0.0f, 0.0f);
        glEnd();
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);
    glPointSize(3.5);
    drawPreviewPoint();
    drawPreviewLine();
    drawPreviewPolygon();
    drawPoint();
    drawLines();
    drawPolygon();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    int normX = x;
    int normY = 600 - y; // Inverter Y para coordenadas OpenGL

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            switch (currentMode) {
                case VERTICE:
                    points[pointCount++] = (Point){normX, normY};
                    printf("Ponto criado: (%d, %d) - Tipo: Vertice\n", normX, normY);
                    break;
                case LINE:
                    tempLine.init = (Point){normX, normY};
                    idDrawing = 1;
                    printf("Início da linha: (%d, %d) - Tipo: Linha\n", normX, normY);
                    break;
                case POLYGON:
                    if (isDrawingPolygon && tempMesh.numberPoints < MAX_POLYGON_POINTS) {
                        tempMesh.vertices[tempMesh.numberPoints++] = (Point){normX, normY};
                        printf("Ponto do polígono: (%d, %d) - Tipo: Polígono\n", normX, normY);
                    }
                    break;
            }
        } else if (state == GLUT_UP) {
            if (currentMode == LINE && idDrawing) {
                tempLine.end = (Point){normX, normY};
                lines[lineCount++] = tempLine;
                idDrawing = 0;
                printf("Fim da linha: (%d, %d) - Tipo: Linha\n", normX, normY);
            }
        }
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (currentMode == POLYGON && isDrawingPolygon) {
            meshes[meshCount++] = tempMesh;
            tempMesh.numberPoints = 0;
            isDrawingPolygon = 0;
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    int normX = x;
    int normY = 600 - y;

    switch (currentMode) {
        case LINE:
            if (idDrawing) {
                tempLine.end = (Point){normX, normY};
            }
            break;
        case POLYGON:
            if (isDrawingPolygon && tempMesh.numberPoints < MAX_POLYGON_POINTS) {
                tempMesh.vertices[tempMesh.numberPoints - 1] = (Point){normX, normY};
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'p':
            currentMode = POLYGON;
            isDrawingPolygon = 1;
            break;
        case 'l':
            currentMode = LINE;
            break;
        case 'v':
            currentMode = VERTICE;
            break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 800, 0, 600);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Paint - Simulator");
    init();
    initLines();
    initPoints();
    initPolygons();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
