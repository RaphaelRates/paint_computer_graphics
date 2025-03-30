#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_POLYGON_POINTS 100
#define MAX_SHAPES 1000
#define CIRCLE_SEGMENTS 12
#define POINT_RADIUS 3.0f
#define PI 3.14159265358979323846
#define SELECTION_THRESHOLD 10.0

typedef struct
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
    CIRCLE,
    SELECT,
    SELECTION
} Mode;

typedef enum {
    NONE, TRANSLATE, ROTATE, SCALE, MIRROR, SHEAR
} TransformMode;

//==================================================== VARIAVEIS ============================================================

Point *points = NULL;
Mesh *meshes = NULL;

Line *lines = NULL;
Mesh tempMesh;

Point tempPoint;

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

Point referencePoint;
Point lastMousePos;

float rotationAngle = 0.0f;
float scaleFactorX = 1.0f;
float scaleFactorY = 1.0f;
int mirrorDirection = 0;
float shearFactorX = 0.0f;
float shearFactorY = 0.0f;

TransformMode currentTransform = NONE;

Mode currentMode = VERTICE;
float colorLoading_r = 0.1f, colorLoading_g = 0.5f, colorLoading_b = 0.3f;
int saveObjectsToFile(const char *filename);
int loadObjectsFromFile(const char *filename);
void initLines();
void initPoints();
void initPolygons();
void display();
void drawCircle(float x, float y, float radius);
void drawPoint();
void drawLines();
void drawPolygon();
void drawPreviewPoint();
void drawPreviewLine();
void drawPreviewPolygon();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void timer(int value);
void passiveMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
Point scalePoint(Point point, Point reference, float scaleX, float scaleY);

/**
 * @details Inicia o espaço de memoria das linhas
 * @return none
 * @
 */
void init();

void initLines()
{
    if (lines == NULL)
        lines = (Line *)malloc(MAX_SHAPES * sizeof(Line));
}

void initPoints()
{
    if (points == NULL)
        points = (Point *)malloc(MAX_SHAPES * sizeof(Point));
}

void initPolygons()
{
    if (meshes == NULL)
        meshes = (Mesh *)malloc(MAX_SHAPES * sizeof(Mesh));
}

// Função para desenhar um círculo em um ponto específico
void drawCircle(float x, float y, float radius)
{
    float theta;
    glBegin(GL_POLYGON);
    for (int i = 0; i < CIRCLE_SEGMENTS; i++)
    {
        theta = 2.0f * M_PI * i / CIRCLE_SEGMENTS;
        glVertex2f(x + radius * cosf(theta), y + radius * sinf(theta));
    }
    glEnd();
}

int showMessage = 0;

void hideMessage(int value)
{
    showMessage = 0;
    glutPostRedisplay();
}

void drawPoint()
{
    for (int i = 0; i < pointCount; i++)
    {
        drawCircle(points[i].x, points[i].y, POINT_RADIUS);
    }
}

void drawLines()
{
    glBegin(GL_LINES);
    for (int i = 0; i < lineCount; i++)
    {
        glVertex2f(lines[i].init.x, lines[i].init.y);
        glVertex2f(lines[i].end.x, lines[i].end.y);
    }
    glEnd();
}

void drawPolygon()
{
    for (int i = 0; i < meshCount; i++)
    {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < meshes[i].numberPoints; j++)
        {
            glVertex2f(meshes[i].vertices[j].x, meshes[i].vertices[j].y);
        }
        glEnd();
    }
}

void drawPreviewPoint()
{
    if (currentMode == VERTICE && idDrawing)
    {
        glColor3f(colorLoading_r, colorLoading_g, colorLoading_b);
        drawCircle(tempPoint.x, tempPoint.y, POINT_RADIUS);
        glColor3f(0.0f, 0.0f, 0.0f);
    }
}

int saveObjectsToFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return 0;
    }

    fprintf(file, "[PONTOS]\n");
    fprintf(file, "Cont: %d\n", pointCount);
    for (int i = 0; i < pointCount; i++)
    {
        fprintf(file, "P%d: %.f, %.f\n", i, points[i].x, points[i].y);
    }
    fprintf(file, "\n");

    fprintf(file, "[LINHAS]\n");
    fprintf(file, "Cont: %d\n", lineCount);
    for (int i = 0; i < lineCount; i++)
    {
        fprintf(file, "L%d: (%.f, %.f) -> (%.f, %.f)\n",
                i, lines[i].init.x, lines[i].init.y,
                lines[i].end.x, lines[i].end.y);
    }
    fprintf(file, "\n");

    fprintf(file, "[POLIGONOS]\n");
    fprintf(file, "Cont: %d\n", meshCount);
    for (int i = 0; i < meshCount; i++)
    {
        fprintf(file, "Poligono %d - Pontas: %d\n", i, meshes[i].numberPoints);
        for (int j = 0; j < meshes[i].numberPoints; j++)
        {
            fprintf(file, "  V%d: %.f, %.f\n",
                    j, meshes[i].vertices[j].x, meshes[i].vertices[j].y);
        }
    }

    fclose(file);
    printf("Os objetos foram salvos no arquivo %s.\n", filename);
    return 1;
}

int loadObjectsFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir ao carregar objetos do arquivo.\n");
        return 0;
    }

    pointCount = 0;
    lineCount = 0;
    meshCount = 0;
    tempMesh.numberPoints = 0;

    char line[256];
    int currentSection = 0;
    int currentPolygonIndex = -1;

    while (fgets(line, sizeof(line), file))
    {

        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0)
            continue;

        if (strcmp(line, "[PONTOS]") == 0)
        {
            currentSection = 1;
            continue;
        }
        else if (strcmp(line, "[LINHAS]") == 0)
        {
            currentSection = 2;
            continue;
        }
        else if (strcmp(line, "[POLIGONOS]") == 0)
        {
            currentSection = 3;
            continue;
        }

        switch (currentSection)
        {
        case 1:
        {
            if (strstr(line, "Cont:") != NULL)
                continue;

            float x, y;
            if (sscanf(line, "P%d: %f, %f", &(int){0}, &x, &y) == 3)
            {
                if (pointCount < MAX_SHAPES)
                {
                    points[pointCount++] = (Point){x, y};
                }
            }
            break;
        }
        case 2:
        {
            if (strstr(line, "Cont:") != NULL)
                continue;

            Point init, end;
            if (sscanf(line, "L%d: (%f, %f) -> (%f, %f)",
                       &(int){0}, &init.x, &init.y, &end.x, &end.y) == 5)
            {
                if (lineCount < MAX_SHAPES)
                {
                    lines[lineCount++] = (Line){init, end};
                }
            }
            break;
        }
        case 3:
        {
            if (strstr(line, "Cont:") != NULL)
                continue;

            if (strstr(line, "Poligono") != NULL)
            {

                if (currentPolygonIndex >= 0 && meshes[currentPolygonIndex].numberPoints > 0)
                {
                    meshCount++;
                }

                currentPolygonIndex++;
                if (currentPolygonIndex >= MAX_SHAPES)
                    break;

                int numPoints;
                if (sscanf(line, "Poligono %d - Pontas: %d", &(int){0}, &numPoints) == 2)
                {
                    meshes[currentPolygonIndex].numberPoints = 0;
                }
                continue;
            }

            float x, y;
            if (sscanf(line, "  V%d: %f, %f", &(int){0}, &x, &y) == 3)
            {
                if (currentPolygonIndex >= 0 &&
                    currentPolygonIndex < MAX_SHAPES &&
                    meshes[currentPolygonIndex].numberPoints < MAX_POLYGON_POINTS)
                {
                    meshes[currentPolygonIndex].vertices[meshes[currentPolygonIndex].numberPoints++] = (Point){x, y};
                }
            }
            break;
        }
        }
    }

    if (currentPolygonIndex >= 0 && meshes[currentPolygonIndex].numberPoints > 0)
    {
        meshCount++;
    }

    fclose(file);
    printf("Os objetos foram carregados!\n");
    return 1;
}

void drawPreviewLine()
{
    if (currentMode == LINE && idDrawing)
    {
        glColor3f(colorLoading_r, colorLoading_g, colorLoading_b);
        glBegin(GL_LINES);
        glVertex2f(tempLine.init.x, tempLine.init.y);
        glVertex2f(tempLine.end.x, tempLine.end.y);
        glColor3f(0.0f, 0.0f, 0.0f);
        glEnd();
    }
}

void drawPreviewPolygon()
{
    if (isDrawingPolygon)
    {
        glColor3f(colorLoading_r, colorLoading_g, colorLoading_b);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < tempMesh.numberPoints; i++)
        {
            glVertex2f(tempMesh.vertices[i].x, tempMesh.vertices[i].y);
        }
        glColor3f(0.0f, 0.0f, 0.0f);
        glEnd();
    }
}

Point scalePoint(Point point, Point reference, float scaleX, float scaleY)
{
    Point scaled;
    scaled.x = reference.x + (point.x - reference.x) * scaleX;
    scaled.y = reference.y + (point.y - reference.y) * scaleY;
    return scaled;
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'p':
        currentMode = POLYGON;
        isDrawingPolygon = 1;
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        break;
    case 'l':
        currentMode = LINE;
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        break;
    case 'v':
        currentMode = VERTICE;
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        break;
    case 'o':
        currentMode = SELECT;
        glutSetCursor(GLUT_CURSOR_UP_DOWN);
        printf("modo de selecao\n");
        break;
    case 'd':
        printf("modo de deletar: ");
        glutSetCursor(GLUT_CURSOR_WAIT);
        if (IdSelectedPoint != -1)
        {
            points[IdSelectedPoint] = points[pointCount - 1];
            pointCount--;
            printf("ponto\n");
        }
        else if (IdsSelectedLine != -1)
        {
            lines[IdsSelectedLine] = lines[lineCount - 1];
            printf("linha\n");
            lineCount--;
        }
        else if (IdSelectedPolygon != -1)
        {
            meshes[IdSelectedPolygon] = meshes[meshCount - 1];
            printf("poligono\n");
            meshCount--;
        }
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 't':
        if (IdSelectedPoint != -1 || IdsSelectedLine != -1 || IdSelectedPolygon != -1)
        {
            isTranslating = !isTranslating;
            isMirroring = isScaling = isShearing = isRotating = 0;
            glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
            printf("modo de translacao\n");
        }
        break;
    case 'r':
        if (IdSelectedPoint != -1 || IdsSelectedLine != -1 || IdSelectedPolygon != -1)
        {
            isRotating = !isRotating;
            isMirroring = isScaling = isShearing = isTranslating = 0;
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            printf("modo de rotacao\n");
        }
        break;
    case 'e':
        if (IdSelectedPoint != -1 || IdsSelectedLine != -1 || IdSelectedPolygon != -1)
        {
            isScaling = !isScaling;
            isMirroring = isRotating = isShearing = isTranslating = 0;
            glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
            printf("modo de escala\n");
        }
        break;
    case 'm':
        if (IdSelectedPoint != -1 || IdsSelectedLine != -1 || IdSelectedPolygon != -1)
        {
            isMirroring = !isMirroring;
            isRotating = isTranslating = isShearing = isScaling = 0;
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            printf("modo de espelhamento\n");
        }
        break;
    case 'c':
        if (IdSelectedPoint != -1 || IdsSelectedLine != -1 || IdSelectedPolygon != -1)
        {
            isShearing = !isShearing;
            isRotating = isTranslating = isMirroring = isScaling = 0;
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            printf("modo de cisalhamento\n");
        }
        break;
    case 's':
        glutSetCursor(GLUT_CURSOR_WAIT);
        saveObjectsToFile("objetos.txt");
        showMessage = 1;
        glutPostRedisplay();
        glutTimerFunc(3000, hideMessage, 0);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 'L':
        glutSetCursor(GLUT_CURSOR_WAIT);
        loadObjectsFromFile("objetos.txt");
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    if (height == 0)
        height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
    int normX = x;
    int normY = 600 - y;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            switch (currentMode)
            {
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
                if (isDrawingPolygon && tempMesh.numberPoints < MAX_POLYGON_POINTS)
                {
                    tempMesh.vertices[tempMesh.numberPoints++] = (Point){normX, normY};
                    printf("Ponto do polígono: (%d, %d) - Tipo: Polígono\n", normX, normY);
                }
                break;
            }
        }
        else if (state == GLUT_UP)
        {
            if (currentMode == LINE && idDrawing)
            {
                tempLine.end = (Point){normX, normY};
                lines[lineCount++] = tempLine;
                idDrawing = 0;
                printf("Fim da linha: (%d, %d) - Tipo: Linha\n", normX, normY);
            }
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        if (currentMode == POLYGON && isDrawingPolygon)
        {
            meshes[meshCount++] = tempMesh;
            tempMesh.numberPoints = 0;
            isDrawingPolygon = 0;
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y)
{
    int normX = x;
    int normY = 600 - y;

    switch (currentMode)
    {
    case LINE:
        if (idDrawing)
        {
            tempLine.end = (Point){normX, normY};
        }
        break;
    case POLYGON:
        if (isDrawingPolygon && tempMesh.numberPoints < MAX_POLYGON_POINTS)
        {
            tempMesh.vertices[tempMesh.numberPoints - 1] = (Point){normX, normY};
        }
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
    printf("Mouse movido para (%d, %d)\n", x, y);
}

void timer(int value)
{
    printf("Timer ativado após %d ms!\n", value);
    glutTimerFunc(1000, timer, value + 1); // Chama de novo após 1 segundo
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 800, 0, 600);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);
    drawPreviewPoint();
    drawPreviewLine();
    drawPreviewPolygon();
    drawPoint();
    drawLines();
    drawPolygon();
    if (showMessage)
    {
        glColor3f(0, 0.7, 0);
        glRasterPos2f(300, 560);
        const char *msg = "Arquivo salvo com sucesso!";
        for (const char *c = msg; *c != '\0'; c++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Paint - Simulator");
    init();
    initLines();
    initPoints();
    initPolygons();
    loadObjectsFromFile("objetos.txt");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}