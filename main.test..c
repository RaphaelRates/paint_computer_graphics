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

#define JOYSTICK_DOWN 0
#define JOYSTICK_X 1
#define JOYSTICK_CIRCLE 2
#define JOYSTICK_TRIANGLE 8
#define JOYSTICK_QUAD 4
#define JOYSTICK_L1 16
#define JOYSTICK_R1 32
#define JOYSTICK_L2 64
#define JOYSTICK_R2 128
#define JOYSTICK_START 256
#define JOYSTICK_L3 512
#define JOYSTICK_OPT 1024
#define JOYSTICK_R3 2048
#define JOYSTICK_ICON_BUTTON 4096
#define JOYSTICK_TOUCH 8192

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

//==================================================== VARIAVEIS ============================================================

Point *points = NULL;
Mesh *meshes = NULL;
Line *lines = NULL;

Mesh tempMesh;
Point tempPoint;
Line tempLine;

int pointCount = 0, lineCount = 0, meshCount = 0;
int isDrawing = 0, isDrawingPolygon = 0, isSelected;
MESSAGE showMessage = NONE_MESSAGE;

int joystickActive = 0;
int joystickButton = 0;
int buttonMask = 0;
int joystickX = 400, joystickY = 400;
int lastMouseX = -1, lastMouseY = -1;
int MousseActvive = 1;

int IdSelectedLine = -1;
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

TransformMode currentTransform = NONE_TRANSFORMER;
Mode currentMode = NONE_MESH;
int joystickMode = 0;
float colorLoading_r = 0.1f, colorLoading_g = 0.5f, colorLoading_b = 0.3f;

int saveObjectsToFile(const char *filename);
int loadObjectsFromFile(const char *filename);
void initLines();
void initPoints();
void initPolygons();
void display();
void drawPoint();
void drawLines();
void drawPolygon();
void drawPreviewPoint();
void drawPreviewLine();
void drawPreviewPolygon();
void drawSelectedObject();
void specialKeys(int key, int x, int y);
void init();
float distance(Point p1, Point p2);
int selectPoint(int x, int y);
int selectLine(int x, int y);
int selectPolygon(int x, int y);
void translatePoint(int id, float dx, float dy);
void translateLine(int id, float dx, float dy);
void translatePolygon(int id, float dx, float dy);
void rotatePoint(int id, float angle);
void rotateLine(int id, float angle);
void rotatePolygon(int id, float angle);
void scaleLine(int id, float sx, float sy);
void scalePolygon(int id, float sx, float sy);
void mirrorPoint(int id, int direction);
void mirrorLine(int id, int direction);
void mirrorPolygon(int id, int direction);
void shearPoint(int id, float sx, float sy);
void shearLine(int id, float sx, float sy);
void shearPolygon(int id, float sx, float sy);
void drawMessage();
Point calculatePolygonCenter(Mesh m);
Point calculateLineCenter(Line l);
int *windowSize();
void deleteSelectedObject();
void hideMessage(int value);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void timer(int value);
void passiveMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
Point scalePoint(Point point, Point reference, float scaleX, float scaleY);

int *windowSize()
{
    int *window = (int *)malloc(2 * sizeof(int));
    if (window == NULL)
    {
        printf("Erro ao alocar memoria!\n");
        exit(1);
    }

    window[0] = (int)(0.8 * glutGet(GLUT_SCREEN_WIDTH));
    window[1] = (int)(0.8 * glutGet(GLUT_SCREEN_HEIGHT));

    return window;
}

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

void drawIconJoystick(int x, int y, int radius)
{
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * PI / 180.0;
        float dx = (radius + 2) * cos(angle);
        float dy = (radius + 2) * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
    glColor3f(0.0, 0.8, 0.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
    {
        float angle = i * PI / 180.0;
        float dx = (radius)*cos(angle);
        float dy = (radius)*sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void drawPoint()
{
    glPointSize(5.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < pointCount; i++)
    {
        if (i == IdSelectedPoint && isSelected)
            glColor3f(1.0f, 0.0f, 0.0f);
        else
            glColor3f(0.0f, 0.0f, 0.0f);

        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();
}

void drawLines()
{
    glLineWidth(2.0);
    for (int i = 0; i < lineCount; i++)
    {
        if (i == IdSelectedLine && isSelected)
            glColor3f(1.0f, 0.0f, 0.0f);
        else
            glColor3f(0.0f, 0.0f, 0.0f);

        glBegin(GL_LINES);
        glVertex2f(lines[i].init.x, lines[i].init.y);
        glVertex2f(lines[i].end.x, lines[i].end.y);
        glEnd();
    }
}

void drawPolygon()
{
    glLineWidth(2.0);
    for (int i = 0; i < meshCount; i++)
    {
        if (i == IdSelectedPolygon && isSelected)
            glColor3f(1.0f, 0.0f, 0.0f);
        else
            glColor3f(0.0f, 0.0f, 0.0f);

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
    if (currentMode == VERTICE && isDrawing)
    {
        glColor3f(0.8f, 1.0f, 0.4f);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        glVertex2f(tempPoint.x, tempPoint.y);
        glEnd();
    }
}

void drawPreviewLine()
{
    if (currentMode == LINE && isDrawing)
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

void drawMessage()
{
    int *window = windowSize();
    switch (showMessage)
    {
    case SAVE_MESH:
        glColor3f(0, 0.7, 0);
    case LOAD_MESH:
        glColor3f(0.6, 0.9, 0);
        break;
    case DELETED:
        glColor3f(0.8, 0.0, 0.1);
        break;
    default:
        glColor3f(0.0, 0.0, 0.0);
        break;
    }
    glRasterPos2f((window[0] * 3) / 7, window[1] - 20);

    const char *msg;
    switch (showMessage)
    {
    case LINE_LOG:
        msg = "Desenho para Linha";
        break;
    case    POLYLGON_LOG:
        msg = "Desenho para Pligono";
        break;
    case VERTCIE_LOG:
        msg = "Desenho para Vertice";
        break;
    case SAVE_MESH:
        msg = "Arquivo salvo com sucesso!";
        break;
    case LOAD_MESH:
        msg = "Arquivo carregado com sucesso!";
        break;
    case DELETED:
        msg = "Objeto deletado!";
        break;
    case IN_CONCERT:
        msg = "FUncionalidade em manutencao, sentimos muito!";
        break;
    default:
        msg = "PAINT - PINTA COM GL";
        break;
    }

    for (const char *c = msg; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
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

void hideMessage(int value)
{
    showMessage = NONE_MESSAGE;
    glutPostRedisplay();
}

void drawSelectedObject()
{
    if (isSelected)
    {
        glColor3f(0.0f, 0.5f, 1.0f);
        glLineWidth(1.0);

        if (IdSelectedPoint >= 0)
        {
            float radius = 8.0f;
            int segments = 20;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; i++)
            {
                float theta = 2.0f * PI * (float)i / (float)segments;
                float x = points[IdSelectedPoint].x + radius * cosf(theta);
                float y = points[IdSelectedPoint].y + radius * sinf(theta);
                glVertex2f(x, y);
            }
            glEnd();
        }
        else if (IdSelectedLine >= 0)
        {
            Point center = calculateLineCenter(lines[IdSelectedLine]);
            float radius = distance(center, lines[IdSelectedLine].init) + 10.0f;
            int segments = 20;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; i++)
            {
                float theta = 2.0f * PI * (float)i / (float)segments;
                float x = center.x + radius * cosf(theta);
                float y = center.y + radius * sinf(theta);
                glVertex2f(x, y);
            }
            glEnd();
        }
        else if (IdSelectedPolygon >= 0)
        {
            Point center = calculatePolygonCenter(meshes[IdSelectedPolygon]);
            float maxDist = 0.0f;
            for (int j = 0; j < meshes[IdSelectedPolygon].numberPoints; j++)
            {
                float dist = distance(center, meshes[IdSelectedPolygon].vertices[j]);
                if (dist > maxDist)
                    maxDist = dist;
            }
            float radius = maxDist + 10.0f;
            int segments = 20;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; i++)
            {
                float theta = 2.0f * PI * (float)i / (float)segments;
                float x = center.x + radius * cosf(theta);
                float y = center.y + radius * sinf(theta);
                glVertex2f(x, y);
            }
            glEnd();
        }
    }
}

void drawTransformInfo()
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(10, 10);

    char info[100] = "";

    if (isSelected)
    {
        char *objectType;
        if (IdSelectedPoint >= 0)
            objectType = "Ponto";
        else if (IdSelectedLine >= 0)
            objectType = "Linha";
        else if (IdSelectedPolygon >= 0)
            objectType = "Poligono";

        switch (currentTransform)
        {
        case TRANSLATE:
            sprintf(info, "Translacao de %s [Setas]", objectType);
            break;
        case ROTATE:
            sprintf(info, "Rotacao de %s [A/D] angulo: %.1f graus", objectType, rotationAngle);
            break;
        case SCALE:
            sprintf(info, "Escala de %s [+/-] X:%.1f Y:%.1f", objectType, scaleFactorX, scaleFactorY);
            break;
        case MIRROR:
            sprintf(info, "Reflexao de %s [X/Y]", objectType);
            break;
        case SHEAR:
            sprintf(info, "Cisalhamento de %s [Shift+Setas] X:%.1f Y:%.1f", objectType, shearFactorX, shearFactorY);
            break;
        default:
            sprintf(info, "%s selecionado", objectType);
        }

        // Renderiza o texto (simplificado, em um sistema real usaria glutBitmapCharacter)
        for (char *c = info; *c != '\0'; c++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }
}

float distance(Point p1, Point p2)
{
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

float pointToLineDistance(Point p, Line l)
{
    float x0 = p.x;
    float y0 = p.y;
    float x1 = l.init.x;
    float y1 = l.init.y;
    float x2 = l.end.x;
    float y2 = l.end.y;

    float l2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (l2 == 0.0)
        return distance(p, l.init);

    float t = ((x0 - x1) * (x2 - x1) + (y0 - y1) * (y2 - y1)) / l2;
    t = (t < 0) ? 0 : (t > 1) ? 1
                              : t;

    float projx = x1 + t * (x2 - x1);
    float projy = y1 + t * (y2 - y1);

    return sqrt((x0 - projx) * (x0 - projx) + (y0 - projy) * (y0 - projy));
}

int isPointInPolygon(Point p, Mesh m)
{
    int i, j, c = 0;
    for (i = 0, j = m.numberPoints - 1; i < m.numberPoints; j = i++)
    {
        if (((m.vertices[i].y > p.y) != (m.vertices[j].y > p.y)) &&
            (p.x < (m.vertices[j].x - m.vertices[i].x) * (p.y - m.vertices[i].y) /
                           (m.vertices[j].y - m.vertices[i].y) +
                       m.vertices[i].x))
            c = !c;
    }
    return c;
}

int selectPoint(int x, int y)
{
    Point clickPoint = {x, y};
    float minDist = SELECTION_THRESHOLD;
    int selectedId = -1;

    for (int i = 0; i < pointCount; i++)
    {
        float dist = distance(clickPoint, points[i]);
        if (dist < minDist)
        {
            minDist = dist;
            selectedId = i;
        }
    }

    return selectedId;
}

int selectLine(int x, int y)
{
    Point clickPoint = {x, y};
    float minDist = SELECTION_THRESHOLD;
    int selectedId = -1;

    for (int i = 0; i < lineCount; i++)
    {
        float dist = pointToLineDistance(clickPoint, lines[i]);
        if (dist < minDist)
        {
            minDist = dist;
            selectedId = i;
        }
    }

    return selectedId;
}

int selectPolygon(int x, int y)
{
    Point clickPoint = {x, y};

    for (int i = 0; i < meshCount; i++)
    {
        if (isPointInPolygon(clickPoint, meshes[i]))
        {
            return i;
        }
    }

    float minDist = SELECTION_THRESHOLD;
    int selectedId = -1;

    for (int i = 0; i < meshCount; i++)
    {
        for (int j = 0; j < meshes[i].numberPoints; j++)
        {
            int next = (j + 1) % meshes[i].numberPoints;
            Line edge = {meshes[i].vertices[j], meshes[i].vertices[next]};
            float dist = pointToLineDistance(clickPoint, edge);
            if (dist < minDist)
            {
                minDist = dist;
                selectedId = i;
            }
        }
    }

    return selectedId;
}

Point calculateLineCenter(Line l)
{
    Point center;
    center.x = (l.init.x + l.end.x) / 2.0f;
    center.y = (l.init.y + l.end.y) / 2.0f;
    return center;
}

Point calculatePolygonCenter(Mesh m)
{
    Point center = {0, 0};
    for (int i = 0; i < m.numberPoints; i++)
    {
        center.x += m.vertices[i].x;
        center.y += m.vertices[i].y;
    }
    center.x /= m.numberPoints;
    center.y /= m.numberPoints;
    return center;
}

void translatePoint(int id, float dx, float dy)
{
    points[id].x += dx;
    points[id].y += dy;
}

void translateLine(int id, float dx, float dy)
{
    lines[id].init.x += dx;
    lines[id].init.y += dy;
    lines[id].end.x += dx;
    lines[id].end.y += dy;
}

void translatePolygon(int id, float dx, float dy)
{
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        meshes[id].vertices[i].x += dx;
        meshes[id].vertices[i].y += dy;
    }
}

void rotatePoint(int id, float angle)
{
    float angleRad = angle * (PI / 180.0f);
    float s = sin(angleRad);
    float c = cos(angleRad);
    float x = points[id].x;
    float y = points[id].y;

    points[id].x = x * c - y * s;
    points[id].y = x * s + y * c;
}

void rotatePointAroundCenter(Point *p, Point center, float angle)
{
    float angleRad = angle * (PI / 180.0f);
    float s = sin(angleRad);
    float c = cos(angleRad);

    float x = p->x - center.x;
    float y = p->y - center.y;

    float xnew = x * c - y * s;
    float ynew = x * s + y * c;

    p->x = xnew + center.x;
    p->y = ynew + center.y;
}

void rotateLine(int id, float angle)
{
    Point center = calculateLineCenter(lines[id]);
    rotatePointAroundCenter(&lines[id].init, center, angle);
    rotatePointAroundCenter(&lines[id].end, center, angle);
}

void rotatePolygon(int id, float angle)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        rotatePointAroundCenter(&meshes[id].vertices[i], center, angle);
    }
}

void scalePointAroundCenter(Point *p, Point center, float sx, float sy)
{
    float x = p->x - center.x;
    float y = p->y - center.y;

    x *= sx;
    y *= sy;

    p->x = x + center.x;
    p->y = y + center.y;
}

void scaleLine(int id, float sx, float sy)
{
    Point center = calculateLineCenter(lines[id]);
    scalePointAroundCenter(&lines[id].init, center, sx, sy);
    scalePointAroundCenter(&lines[id].end, center, sx, sy);
}

void scalePolygon(int id, float sx, float sy)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        scalePointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}

void mirrorPointAroundAxis(Point *p, Point center, int direction)
{
    if (direction == 0)
    {
        p->x = 2 * center.x - p->x;
    }
    else
    {
        p->y = 2 * center.y - p->y;
    }
}

void mirrorPoint(int id, int direction)
{
    Point origin = {0, 0};
    mirrorPointAroundAxis(&points[id], origin, direction);
}

void mirrorLine(int id, int direction)
{
    Point center = calculateLineCenter(lines[id]);
    mirrorPointAroundAxis(&lines[id].init, center, direction);
    mirrorPointAroundAxis(&lines[id].end, center, direction);
}

void mirrorPolygon(int id, int direction)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        mirrorPointAroundAxis(&meshes[id].vertices[i], center, direction);
    }
}

void shearPointAroundCenter(Point *p, Point center, float sx, float sy)
{
    float x = p->x - center.x;
    float y = p->y - center.y;

    float xnew = x + sx * y;
    float ynew = sy * x + y;

    p->x = xnew + center.x;
    p->y = ynew + center.y;
}

void shearPoint(int id, float sx, float sy)
{
    Point origin = {0, 0};
    shearPointAroundCenter(&points[id], origin, sx, sy);
}

void shearLine(int id, float sx, float sy)
{
    Point center = calculateLineCenter(lines[id]);
    shearPointAroundCenter(&lines[id].init, center, sx, sy);
    shearPointAroundCenter(&lines[id].end, center, sx, sy);
}

void shearPolygon(int id, float sx, float sy)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        shearPointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}

void deleteSelectedObject()
{
    if (!isSelected)
        return;

    if (IdSelectedPoint >= 0)
    {
        for (int i = IdSelectedPoint; i < pointCount - 1; i++)
        {
            points[i] = points[i + 1];
        }
        pointCount--;
        printf("Ponto ID %d foi deletado\n", IdSelectedPoint);
    }
    else if (IdSelectedLine >= 0)
    {
        for (int i = IdSelectedLine; i < lineCount - 1; i++)
        {
            lines[i] = lines[i + 1];
        }
        lineCount--;
        printf("Linha ID %d foi deletado\n", IdSelectedLine);
    }
    else if (IdSelectedPolygon >= 0)
    {
        for (int i = IdSelectedPolygon; i < meshCount - 1; i++)
        {
            meshes[i] = meshes[i + 1];
        }
        meshCount--;
        printf("Poligono ID %d foi deletado\n", IdSelectedPolygon);
    }

    IdSelectedPoint = -1;
    IdSelectedLine = -1;
    IdSelectedPolygon = -1;
    isSelected = 0;
    currentTransform = NONE_TRANSFORMER;
}

void keyboard(unsigned char key, int x, int y)
{
    int *window = windowSize();
    switch (key)
    {
    case 'p':
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        joystickActive = 0;
        currentMode = POLYGON;
        showMessage = POLYLGON_LOG;
        isDrawingPolygon = 1;
        tempMesh.numberPoints = 0;
        glutTimerFunc(2000, hideMessage, 0);
        printf("Modo: Desenhar Poligono\n");
        break;
    case 'l':
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        joystickActive = 0;
        isDrawing = 1;
        currentMode = LINE;
        showMessage = LINE_LOG;
        glutTimerFunc(2000, hideMessage, 0);
        printf("Modo: Desenhar Linha\n");
        break;
    case 'v':
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        currentMode = VERTICE;
        showMessage = VERTCIE_LOG;
        glutTimerFunc(2000, hideMessage, 0);
        printf("Modo: Desenhar Vertice\n");
        break;
    case 's':
        glutSetCursor(GLUT_CURSOR_CYCLE);
        joystickActive = 0;
        currentMode = SELECTION;

        printf("Modo: Selecao\n");
        break;
    case 't':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_CYCLE);
            joystickActive = 0;
            currentTransform = TRANSLATE;
            printf("Transformacao: Translacao\n");
        }
        break;
    case 'r':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_UP_DOWN);
            joystickActive = 0;
            currentTransform = ROTATE;
            printf("Transformacao: Rotacao\n");
        }
        break;
    case 'e':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_SPRAY);
            joystickActive = 0;
            currentTransform = SCALE;
            printf("Transformacao: Escala\n");
        }
        break;
    case 'm':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            joystickActive = 0;
            currentTransform = MIRROR;
            printf("Transformacao: Reflexao\n");
        }
        break;
    case 'c':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            joystickActive = 0;
            currentTransform = SHEAR;
            printf("Transformacao: Cisalhamento\n");
        }
        break;
    case 'h':
        printf("\n\n===== OpenGL Paint com Transforma��es Geom�tricas =====\n");
        joystickActive = 0;
        printf("Modos de desenho:\n");
        printf("  v: Modo Vertice\n");
        printf("  l: Modo Linha\n");
        printf("  p: Modo Poligono\n");
        printf("  s: Modo Selecao\n\n");

        printf("Transformacoes (apos selecionar um objeto):\n");
        printf("  t: Ativar translacao (use as setas)\n");
        printf("  r: Ativar rotacao (use A/D ou roda do mouse)\n");
        printf("  e: Ativar escala (use +/- ou roda do mouse)\n");
        printf("  m: Ativar reflexao (use x/y para espelhar)\n");
        printf("  c: Ativar cisalhamento (use shift + setas)\n\n");
        printf("  D: Deletar um objeto\n");

        printf("Outras operacoes:\n");
        printf("  Botao do meio do mouse: Alternar entre transformacoes\n");
        printf("  Botao direito: Cancelar selecao ou fechar poligono\n");

        break;
    case 'x':
        if (isSelected && currentTransform == MIRROR)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            mirrorDirection = 0;
            if (IdSelectedPoint >= 0)
                mirrorPoint(IdSelectedPoint, 0);
            else if (IdSelectedLine >= 0)
                mirrorLine(IdSelectedLine, 0);
            else if (IdSelectedPolygon >= 0)
                mirrorPolygon(IdSelectedPolygon, 0);
            printf("Reflexao horizontal aplicada\n");
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case 'y':
        if (isSelected && currentTransform == MIRROR)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            mirrorDirection = 1;
            if (IdSelectedPoint >= 0)
                mirrorPoint(IdSelectedPoint, 1);
            else if (IdSelectedLine >= 0)
                mirrorLine(IdSelectedLine, 1);
            else if (IdSelectedPolygon >= 0)
                mirrorPolygon(IdSelectedPolygon, 1);
            printf("Reflexao vertical aplicada\n");
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case 'a':
        if (isSelected && currentTransform == ROTATE)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            rotationAngle -= 5.0f;
            if (IdSelectedPoint >= 0)
                rotatePoint(IdSelectedPoint, -5.0f);
            else if (IdSelectedLine >= 0)
                rotateLine(IdSelectedLine, -5.0f);
            else if (IdSelectedPolygon >= 0)
                rotatePolygon(IdSelectedPolygon, -5.0f);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case 'd':
        if (isSelected && currentTransform == ROTATE)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            rotationAngle += 5.0f;
            if (IdSelectedPoint >= 0)
                rotatePoint(IdSelectedPoint, 5.0f);
            else if (IdSelectedLine >= 0)
                rotateLine(IdSelectedLine, 5.0f);
            else if (IdSelectedPolygon >= 0)
                rotatePolygon(IdSelectedPolygon, 5.0f);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case '+':
    case '=':
        if (isSelected && currentTransform == SCALE)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            scaleFactorX *= 1.1f;
            scaleFactorY *= 1.1f;
            if (IdSelectedLine >= 0)
                scaleLine(IdSelectedLine, 1.1f, 1.1f);
            else if (IdSelectedPolygon >= 0)
                scalePolygon(IdSelectedPolygon, 1.1f, 1.1f);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case '-':
    case '_':
        if (isSelected && currentTransform == SCALE)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            scaleFactorX *= 0.9f;
            scaleFactorY *= 0.9f;
            if (IdSelectedLine >= 0)
                scaleLine(IdSelectedLine, 0.9f, 0.9f);
            else if (IdSelectedPolygon >= 0)
                scalePolygon(IdSelectedPolygon, 0.9f, 0.9f);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case 'D':
        glutSetCursor(GLUT_CURSOR_WAIT);
        joystickActive = 0;
        showMessage = DELETED;
        deleteSelectedObject();
        glutTimerFunc(2000, hideMessage, 0);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 'S':
        glutSetCursor(GLUT_CURSOR_WAIT);
        joystickActive = 0;
        saveObjectsToFile("objetos.txt");
        showMessage = SAVE_MESH;
        glutTimerFunc(2000, hideMessage, 0);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 'L':
        if (joystickActive)
            return;
        glutSetCursor(GLUT_CURSOR_WAIT);
        joystickActive = 0;
        showMessage = LOAD_MESH;
        loadObjectsFromFile("objetos.txt");
        glutTimerFunc(2000, hideMessage, 0);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void joystick(int unsigned buttons, int x, int y, int z)
{
    int *window = windowSize();

    int moveX = 0, moveY = 0;
    int normX = joystickX;
    int normY = joystickY;

    printf("%d, %d, %d\n", buttons, currentMode == LINE, isDrawing);
    if (x > 0)
        moveX = x / 80;
    else if (x < 0)
        moveX = x / 80;

    if (y > 0)
        moveY = -y / 80;
    else if (y < 0)
        moveY = -y / 80;

    joystickX += moveX;
    joystickY += moveY;

    joystickActive = (x != 0 || y != 0);
    MousseActvive = !joystickActive;

    if (joystickX < 0)
        joystickX = 0;
    if (joystickX > window[0])
        joystickX = window[0];
    if (joystickY < 0)
        joystickY = 0;
    if (joystickY > window[1])
        joystickY = window[1];

    int previousButtonMask = buttonMask;
    buttonMask = buttons;
    switch (buttons)
    {
    case JOYSTICK_DOWN:
        if ((currentMode == LINE) && isDrawing && joystickActive)
        {
            tempLine.end = (Point){joystickX, joystickY};
            lines[lineCount++] = tempLine;
            isDrawing = 0;
            printf("Linha criada: (%d, %d) - Tipo: Linha\n", joystickX, joystickY);
        }
        break;

    case JOYSTICK_X: // X
        if (buttonMask != previousButtonMask)
        {
            showMessage = LOAD_MESH;
            loadObjectsFromFile("objetos.txt");
            glutTimerFunc(2000, hideMessage, 0);
            break;
        }

    case JOYSTICK_CIRCLE: // O
        if (buttonMask != previousButtonMask)
        {
            showMessage = DELETED;
            deleteSelectedObject();
            glutTimerFunc(2000, hideMessage, 0);
            break;
        }

    case JOYSTICK_QUAD: // Quadrado
        if (currentMode == VERTICE && buttonMask != previousButtonMask)
        {
            points[pointCount++] = (Point){joystickX, joystickY};
            printf("Ponto criado: (%d, %d) - Tipo: Vertice\n", joystickX, joystickY);
        }
        else if (currentMode == LINE && !isDrawing && buttonMask != previousButtonMask)
        {
            tempLine.init = (Point){joystickX, joystickY};
            isDrawing = 1;
            printf("inicio da linha: (%d, %d) - Tipo: Linha\n", joystickX, joystickY);
        }
        else if (currentMode == LINE && buttons == JOYSTICK_QUAD)
        {
            tempLine.end = (Point){joystickX, joystickY};
        }
        else if (currentMode == POLYGON && isDrawingPolygon && tempMesh.numberPoints < MAX_POLYGON_POINTS)
        {
            printf("polygono: (%d, %d) - Tipo: Linha\n", joystickX, joystickY);
        }
        else if (currentMode == SELECTION && !isDrawing && buttonMask != previousButtonMask)
        {
            IdSelectedPoint = -1;
            IdSelectedLine = -1;
            IdSelectedPolygon = -1;
            isSelected = 0;
            IdSelectedPoint = selectPoint(joystickX, joystickY);

            if (IdSelectedPoint >= 0)
            {
                isSelected = 1;
                printf("Ponto selecionado: ID %d\n", IdSelectedPoint);
            }
            else
            {
                IdSelectedLine = selectLine(joystickX, joystickY);

                if (IdSelectedLine >= 0)
                {
                    isSelected = 1;
                    printf("Linha selecionada: ID %d\n", IdSelectedLine);
                }
                else
                {
                    IdSelectedPolygon = selectPolygon(joystickX, joystickY);

                    if (IdSelectedPolygon >= 0)
                    {
                        isSelected = 1;
                        printf("Poligono selecionado: ID %d\n", IdSelectedPolygon);
                    }
                }
            }

            if (isSelected)
            {
                if (IdSelectedPoint >= 0)
                {
                    referencePoint = points[IdSelectedPoint];
                }
                else if (IdSelectedLine >= 0)
                {
                    referencePoint = calculateLineCenter(lines[IdSelectedLine]);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    referencePoint = calculatePolygonCenter(meshes[IdSelectedPolygon]);
                }
            }
        }

        break;
    case JOYSTICK_TRIANGLE: // Triângulo
        if (buttonMask != previousButtonMask)
        {
            saveObjectsToFile("objetos.txt");
            showMessage = SAVE_MESH;
            glutTimerFunc(2000, hideMessage, 0);
            break;
        }

    case JOYSTICK_L1: // L1
        if (buttonMask != previousButtonMask)
        {
            if (currentMode == SELECTION)
            {
                currentMode = VERTICE;
            }
            else
            {
                printf("Modo desenho: %d\n", currentMode);
                currentMode++;
            }
        }
        break;

    case JOYSTICK_R1: // R1
        if (buttonMask != previousButtonMask)
        {
            if (currentTransform == SHEAR)
            {
                currentTransform = NONE_TRANSFORMER;
            }
            else
            {
                printf("Modo tranformacao: %d\n", currentTransform);
                currentTransform++;
            }
        }
        break;
    case JOYSTICK_L2:
        break; // L2
    case JOYSTICK_R2:
        break; // R2
    case JOYSTICK_START:
        break; // Share
    case JOYSTICK_OPT:
        break; // Options
    case JOYSTICK_L3:
        if (buttonMask != previousButtonMask)
        {
            if (isSelected)
            {
                IdSelectedPoint = -1;
                IdSelectedLine = -1;
                IdSelectedPolygon = -1;
                isSelected = 0;
                currentTransform = NONE_TRANSFORMER;
                printf("Sele��o removida.\n");
            }
        }
        break; // L3
    case JOYSTICK_R3:
        break;                 // R3
    case JOYSTICK_ICON_BUTTON: // PS
        exit(0);
        break;
    case JOYSTICK_TOUCH: // Touchpad
        showMessage = IN_CONCERT;
        glutTimerFunc(2000, hideMessage, 0);
        break;

    default:
        break;
    }

    // printf("%d %d\n", j)
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
    int *window = windowSize();
    int normX = x;
    int normY = window[1] - y;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            lastMousePos.x = normX;
            lastMousePos.y = normY;

            switch (currentMode)
            {
            case VERTICE:
                points[pointCount++] = (Point){normX, normY};
                printf("Ponto criado: (%d, %d) - Tipo: Vertice\n", normX, normY);
                break;

            case LINE:
                tempLine.init = (Point){normX, normY};
                isDrawing = 1;
                printf("Inicio da linha: (%d, %d) - Tipo: Linha\n", normX, normY);
                break;

            case POLYGON:
                if (isDrawingPolygon && tempMesh.numberPoints < MAX_POLYGON_POINTS)
                {
                    tempMesh.vertices[tempMesh.numberPoints++] = (Point){normX, normY};
                    printf("Ponto do poligono: (%d, %d) - Tipo: Poligono\n", normX, normY);
                }
                break;

            case SELECTION:
                IdSelectedPoint = -1;
                IdSelectedLine = -1;
                IdSelectedPolygon = -1;
                isSelected = 0;
                IdSelectedPoint = selectPoint(normX, normY);

                if (IdSelectedPoint >= 0)
                {
                    isSelected = 1;
                    printf("Ponto selecionado: ID %d\n", IdSelectedPoint);
                }
                else
                {
                    IdSelectedLine = selectLine(normX, normY);

                    if (IdSelectedLine >= 0)
                    {
                        isSelected = 1;
                        printf("Linha selecionada: ID %d\n", IdSelectedLine);
                    }
                    else
                    {
                        IdSelectedPolygon = selectPolygon(normX, normY);

                        if (IdSelectedPolygon >= 0)
                        {
                            isSelected = 1;
                            printf("Poligono selecionado: ID %d\n", IdSelectedPolygon);
                        }
                    }
                }

                if (isSelected)
                {
                    if (IdSelectedPoint >= 0)
                    {
                        referencePoint = points[IdSelectedPoint];
                    }
                    else if (IdSelectedLine >= 0)
                    {
                        referencePoint = calculateLineCenter(lines[IdSelectedLine]);
                    }
                    else if (IdSelectedPolygon >= 0)
                    {
                        referencePoint = calculatePolygonCenter(meshes[IdSelectedPolygon]);
                    }
                }
                break;
            }
        }
        else if (state == GLUT_UP)
        {
            if (currentMode == LINE && isDrawing)
            {
                tempLine.end = (Point){normX, normY};
                lines[lineCount++] = tempLine;
                isDrawing = 0;
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
            printf("Poligono fechado e armazenado.\n");
        }
        else if (isSelected)
        {
            IdSelectedPoint = -1;
            IdSelectedLine = -1;
            IdSelectedPolygon = -1;
            isSelected = 0;
            currentTransform = NONE_TRANSFORMER;
            printf("Sele��o removida.\n");
        }
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && isSelected)
    {
        currentTransform = (currentTransform + 1) % 5;

        switch (currentTransform)
        {
        case TRANSLATE:
            printf("Modo: Translacao\n");
            break;
        case ROTATE:
            printf("Modo: Rotacao\n");
            break;
        case SCALE:
            printf("Modo: Escala\n");
            break;
        case MIRROR:
            printf("Modo: Reflexao\n");
            break;
        case SHEAR:
            printf("Modo: Cisalhamento\n");
            break;
        }
    }
    else if (button == 3 && isSelected)
    {
        switch (currentTransform)
        {
        case SCALE:
            scaleFactorX *= 1.1f;
            scaleFactorY *= 1.1f;

            if (IdSelectedLine >= 0)
            {
                scaleLine(IdSelectedLine, 1.1f, 1.1f);
            }
            else if (IdSelectedPolygon >= 0)
            {
                scalePolygon(IdSelectedPolygon, 1.1f, 1.1f);
            }
            break;
        case ROTATE:
            rotationAngle += 5.0f;

            if (IdSelectedPoint >= 0)
            {
                rotatePoint(IdSelectedPoint, 5.0f);
            }
            else if (IdSelectedLine >= 0)
            {
                rotateLine(IdSelectedLine, 5.0f);
            }
            else if (IdSelectedPolygon >= 0)
            {
                rotatePolygon(IdSelectedPolygon, 5.0f);
            }
            break;
        }
    }
    else if (button == 4 && isSelected)
    {
        switch (currentTransform)
        {
        case SCALE:
            scaleFactorX *= 0.9f;
            scaleFactorY *= 0.9f;

            if (IdSelectedLine >= 0)
            {
                scaleLine(IdSelectedLine, 0.9f, 0.9f);
            }
            else if (IdSelectedPolygon >= 0)
            {
                scalePolygon(IdSelectedPolygon, 0.9f, 0.9f);
            }
            break;
        case ROTATE:
            rotationAngle -= 5.0f;

            if (IdSelectedPoint >= 0)
            {
                rotatePoint(IdSelectedPoint, -5.0f);
            }
            else if (IdSelectedLine >= 0)
            {
                rotateLine(IdSelectedLine, -5.0f);
            }
            else if (IdSelectedPolygon >= 0)
            {
                rotatePolygon(IdSelectedPolygon, -5.0f);
            }
            break;
        }
    }

    glutPostRedisplay();
}

void motion(int x, int y)
{
    int *window = windowSize();
    int normX = x;
    int normY = window[1] - y;

    switch (currentMode)
    {
    case LINE:
        if (isDrawing)
        {
            tempLine.end = (Point){normX, normY};
        }
        break;
    case POLYGON:
        if (isDrawingPolygon && tempMesh.numberPoints > 0)
        {
            tempMesh.vertices[tempMesh.numberPoints - 1] = (Point){normX, normY};
        }
        break;
    case SELECTION:
        if (isSelected && currentTransform == TRANSLATE)
        {
            float dx = normX - lastMousePos.x;
            float dy = normY - lastMousePos.y;

            if (IdSelectedPoint >= 0)
            {
                translatePoint(IdSelectedPoint, dx, dy);
            }
            else if (IdSelectedLine >= 0)
            {
                translateLine(IdSelectedLine, dx, dy);
            }
            else if (IdSelectedPolygon >= 0)
            {
                translatePolygon(IdSelectedPolygon, dx, dy);
            }

            lastMousePos.x = normX;
            lastMousePos.y = normY;
        }
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
    float step = 5.0f;
    int shiftPressed = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    if (isSelected)
    {
        switch (key)
        {
        case GLUT_KEY_UP:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, 0, step);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, 0, step);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, 0, step);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorY += 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, 0, 0.1f);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, 0, 0.1f);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, 0, 0.1f);
                }
            }
            break;

        case GLUT_KEY_DOWN:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, 0, -step);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, 0, -step);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, 0, -step);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorY -= 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, 0, -0.1f);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, 0, -0.1f);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, 0, -0.1f);
                }
            }
            break;

        case GLUT_KEY_LEFT:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, -step, 0);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, -step, 0);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, -step, 0);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorX -= 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, -0.1f, 0);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, -0.1f, 0);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, -0.1f, 0);
                }
            }
            break;

        case GLUT_KEY_RIGHT:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, step, 0);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, step, 0);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, step, 0);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorX += 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, 0.1f, 0);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, 0.1f, 0);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, 0.1f, 0);
                }
            }
            break;
        }
    }
    glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
    int *window = windowSize();
    if (MousseActvive)
    {
        lastMouseX = x;
        lastMouseY = y;
        joystickActive = 0;
    }
    else
    {
        glutSetCursor(GLUT_CURSOR_NONE);
    }
}

void timer(int value)
{
    glutTimerFunc(10, timer, value + 1);
}

void init()
{
    int *window = windowSize();
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, window[0], 0, window[1]);
    glViewport(0, 0, window[0], window[1]);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);
    if (joystickActive)
    {
        drawIconJoystick(joystickX, joystickY, 8);
    }
    drawPreviewPoint();
    drawPreviewLine();
    drawPreviewPolygon();
    drawPoint();
    drawLines();
    drawPolygon();
    drawSelectedObject();
    drawTransformInfo();
    drawMessage();
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    int *window = windowSize();
    glutInitWindowSize(window[0], window[1]);
    glutCreateWindow("OpenGL Paint - Simulator");
    init();
    initLines();
    initPoints();
    initPolygons();
    loadObjectsFromFile("objetos.txt");
    glutJoystickFunc(joystick, 10);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutPassiveMotionFunc(passiveMotion);
    timer(0);
    glutMouseFunc(mouse);

    glutMotionFunc(motion);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    printf("\n\n===== OpenGL Paint =====\n");
    printf("Modos de desenho:\n");
    printf("  v: Modo Virtice\n");
    printf("  l: Modo Linha\n");
    printf("  p: Modo Poligono\n");
    printf("  s: Modo Selecao\n\n");

    printf("Transformacoes (apos selecionar um objeto):\n");
    printf("  t: Ativar translacao (use as setas)\n");
    printf("  r: Ativar rotacao (use A/D ou roda do mouse)\n");
    printf("  e: Ativar escala (use +/- ou roda do mouse)\n");
    printf("  m: Ativar reflexao (use x/y para espelhar)\n");
    printf("  c: Ativar cisalhamento (use shift + setas)\n\n");

    printf("Outras operacoes:\n");
    printf("  Botao do meio do mouse: Alternar entre transformacoes\n");
    printf("  Botao direito: Cancelar selecao ou fechar poligono\n");

    if (points)
        free(points);
    if (lines)
        free(lines);
    if (meshes)
        free(meshes);
    return 0;
}