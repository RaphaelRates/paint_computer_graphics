#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_POLYGON_POINTS 100
#define MAX_SHAPES 1000
#define PI 3.14159265358979323846
#define SELECTION_THRESHOLD 10.0

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
    VERTICE, LINE, POLYGON, CIRCLE, SELECTION
} Mode;

/**
 * @enum TransformMode
 * @details Define os modos de transformação
 */
typedef enum {
    NONE, TRANSLATE, ROTATE, SCALE, MIRROR, SHEAR
} TransformMode;

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
int idDrawing = 0, isDrawingPolygon = 0, isSelected = 0;

int IdSelectedLine = -1;
int IdSelectedPoint = -1;
int IdSelectedPolygon = -1;

// Ponto de referência para transformações
Point referencePoint;
// Armazena a posição anterior do mouse para cálculo de deslocamento
Point lastMousePos;

TransformMode currentTransform = NONE;
Mode currentMode = VERTICE;

// Angulo de rotação em graus
float rotationAngle = 0.0f;
// Fatores de escala
float scaleFactorX = 1.0f;
float scaleFactorY = 1.0f;
// Direção de espelhamento (0 = horizontal, 1 = vertical)
int mirrorDirection = 0;
// Fatores de cisalhamento
float shearFactorX = 0.0f;
float shearFactorY = 0.0f;

// Protótipos de funções
void initLines();
void initPoints();
void initPolygons();
void display();
void drawPoint();
void drawLines();
void drawPolygon();
void drawSelectedObject();
void drawPreviewPoint();
void drawPreviewLine();
void drawPreviewPolygon();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
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
Point calculatePolygonCenter(Mesh m);
Point calculateLineCenter(Line l);

// Funções de inicialização
void initLines() {
    if (lines == NULL) lines = (Line *)malloc(MAX_SHAPES * sizeof(Line));
}

void initPoints() {
    if (points == NULL) points = (Point *)malloc(MAX_SHAPES * sizeof(Point));
}

void initPolygons() {
    if(meshes == NULL) meshes = (Mesh *)malloc(MAX_SHAPES * sizeof(Mesh));
}

// Funções de desenho
void drawPoint() {
    glPointSize(5.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < pointCount; i++) {
        if (i == IdSelectedPoint && isSelected)
            glColor3f(1.0f, 0.0f, 0.0f); // Vermelho para o ponto selecionado
        else
            glColor3f(0.0f, 0.0f, 0.0f); // Preto para os demais

        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();
}

void drawLines() {
    glLineWidth(2.0);
    for (int i = 0; i < lineCount; i++) {
        if (i == IdSelectedLine && isSelected)
            glColor3f(1.0f, 0.0f, 0.0f); // Vermelho para a linha selecionada
        else
            glColor3f(0.0f, 0.0f, 0.0f); // Preto para as demais

        glBegin(GL_LINES);
        glVertex2f(lines[i].init.x, lines[i].init.y);
        glVertex2f(lines[i].end.x, lines[i].end.y);
        glEnd();
    }
}

void drawPolygon() {
    glLineWidth(2.0);
    for (int i = 0; i < meshCount; i++) {
        if (i == IdSelectedPolygon && isSelected)
            glColor3f(1.0f, 0.0f, 0.0f); // Vermelho para o polígono selecionado
        else
            glColor3f(0.0f, 0.0f, 0.0f); // Preto para os demais

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < meshes[i].numberPoints; j++) {
            glVertex2f(meshes[i].vertices[j].x, meshes[i].vertices[j].y);
        }
        glEnd();
    }
}

void drawSelectedObject() {
    // Desenha um indicador de seleção (círculo) ao redor do objeto selecionado
    if (isSelected) {
        glColor3f(0.0f, 0.5f, 1.0f);
        glLineWidth(1.0);

        if (IdSelectedPoint >= 0) {
            float radius = 8.0f;
            int segments = 20;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; i++) {
                float theta = 2.0f * PI * (float)i / (float)segments;
                float x = points[IdSelectedPoint].x + radius * cosf(theta);
                float y = points[IdSelectedPoint].y + radius * sinf(theta);
                glVertex2f(x, y);
            }
            glEnd();
        } else if (IdSelectedLine >= 0) {
            Point center = calculateLineCenter(lines[IdSelectedLine]);
            float radius = distance(center, lines[IdSelectedLine].init) + 10.0f;
            int segments = 20;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; i++) {
                float theta = 2.0f * PI * (float)i / (float)segments;
                float x = center.x + radius * cosf(theta);
                float y = center.y + radius * sinf(theta);
                glVertex2f(x, y);
            }
            glEnd();
        } else if (IdSelectedPolygon >= 0) {
            Point center = calculatePolygonCenter(meshes[IdSelectedPolygon]);

            // Encontra o raio baseado no ponto mais distante
            float maxDist = 0.0f;
            for (int j = 0; j < meshes[IdSelectedPolygon].numberPoints; j++) {
                float dist = distance(center, meshes[IdSelectedPolygon].vertices[j]);
                if (dist > maxDist) maxDist = dist;
            }

            float radius = maxDist + 10.0f;
            int segments = 20;
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < segments; i++) {
                float theta = 2.0f * PI * (float)i / (float)segments;
                float x = center.x + radius * cosf(theta);
                float y = center.y + radius * sinf(theta);
                glVertex2f(x, y);
            }
            glEnd();
        }
    }
}

void drawPreviewPoint() {
    if (currentMode == VERTICE && idDrawing) {
        glColor3f(0.8f, 1.0f, 0.4f);
        glPointSize(5.0);
        glBegin(GL_POINTS);
            glVertex2f(tempPoint.x, tempPoint.y);
        glEnd();
    }
}

void drawPreviewLine() {
    if (currentMode == LINE && idDrawing) {
        glColor3f(0.8f, 1.0f, 0.4f);
        glLineWidth(2.0);
        glBegin(GL_LINES);
            glVertex2f(tempLine.init.x, tempLine.init.y);
            glVertex2f(tempLine.end.x, tempLine.end.y);
        glEnd();
    }
}

void drawPreviewPolygon() {
    if (isDrawingPolygon) {
        glColor3f(0.8f, 1.0f, 0.4f);
        glLineWidth(2.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < tempMesh.numberPoints; i++) {
            glVertex2f(tempMesh.vertices[i].x, tempMesh.vertices[i].y);
        }
        glEnd();
    }
}

void drawTransformInfo() {
    // Exibe informações sobre a transformação atual no canto inferior esquerdo
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(10, 10);

    char info[100] = "";

    if (isSelected) {
        char *objectType;
        if (IdSelectedPoint >= 0)
            objectType = "Ponto";
        else if (IdSelectedLine >= 0)
            objectType = "Linha";
        else if (IdSelectedPolygon >= 0)
            objectType = "Polígono";

        switch (currentTransform) {
            case TRANSLATE:
                sprintf(info, "Translação de %s [Setas]", objectType);
                break;
            case ROTATE:
                sprintf(info, "Rotação de %s [A/D] Ângulo: %.1f°", objectType, rotationAngle);
                break;
            case SCALE:
                sprintf(info, "Escala de %s [+/-] X:%.1f Y:%.1f", objectType, scaleFactorX, scaleFactorY);
                break;
            case MIRROR:
                sprintf(info, "Reflexão de %s [X/Y]", objectType);
                break;
            case SHEAR:
                sprintf(info, "Cisalhamento de %s [Shift+Setas] X:%.1f Y:%.1f", objectType, shearFactorX, shearFactorY);
                break;
            default:
                sprintf(info, "%s selecionado", objectType);
        }

        // Renderiza o texto (simplificado, em um sistema real usaria glutBitmapCharacter)
        for (char *c = info; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);

    drawPreviewPoint();
    drawPreviewLine();
    drawPreviewPolygon();
    drawPoint();
    drawLines();
    drawPolygon();
    drawSelectedObject();
    drawTransformInfo();

    glutSwapBuffers();
}

float distance(Point p1, Point p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

float pointToLineDistance(Point p, Line l) {
    float x0 = p.x;
    float y0 = p.y;
    float x1 = l.init.x;
    float y1 = l.init.y;
    float x2 = l.end.x;
    float y2 = l.end.y;

    // Comprimento da linha ao quadrado
    float l2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (l2 == 0.0) return distance(p, l.init); // Linha é um ponto

    // Projeção do ponto na linha, parametrizada como 0 <= t <= 1
    float t = ((x0 - x1) * (x2 - x1) + (y0 - y1) * (y2 - y1)) / l2;
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;

    // Ponto mais próximo na linha
    float projx = x1 + t * (x2 - x1);
    float projy = y1 + t * (y2 - y1);

    return sqrt((x0 - projx) * (x0 - projx) + (y0 - projy) * (y0 - projy));
}

int isPointInPolygon(Point p, Mesh m) {
    int i, j, c = 0;
    for (i = 0, j = m.numberPoints - 1; i < m.numberPoints; j = i++) {
        if (((m.vertices[i].y > p.y) != (m.vertices[j].y > p.y)) &&
            (p.x < (m.vertices[j].x - m.vertices[i].x) * (p.y - m.vertices[i].y) /
            (m.vertices[j].y - m.vertices[i].y) + m.vertices[i].x))
            c = !c;
    }
    return c;
}

// Funções de seleção
int selectPoint(int x, int y) {
    Point clickPoint = {x, y};
    float minDist = SELECTION_THRESHOLD;
    int selectedId = -1;

    for (int i = 0; i < pointCount; i++) {
        float dist = distance(clickPoint, points[i]);
        if (dist < minDist) {
            minDist = dist;
            selectedId = i;
        }
    }

    return selectedId;
}

int selectLine(int x, int y) {
    Point clickPoint = {x, y};
    float minDist = SELECTION_THRESHOLD;
    int selectedId = -1;

    for (int i = 0; i < lineCount; i++) {
        float dist = pointToLineDistance(clickPoint, lines[i]);
        if (dist < minDist) {
            minDist = dist;
            selectedId = i;
        }
    }

    return selectedId;
}

int selectPolygon(int x, int y) {
    Point clickPoint = {x, y};

    // Primeiro verifica se o clique está dentro de algum polígono
    for (int i = 0; i < meshCount; i++) {
        if (isPointInPolygon(clickPoint, meshes[i])) {
            return i;
        }
    }

    // Se não estiver dentro, verifica se está próximo do contorno
    float minDist = SELECTION_THRESHOLD;
    int selectedId = -1;

    for (int i = 0; i < meshCount; i++) {
        for (int j = 0; j < meshes[i].numberPoints; j++) {
            int next = (j + 1) % meshes[i].numberPoints;
            Line edge = {meshes[i].vertices[j], meshes[i].vertices[next]};
            float dist = pointToLineDistance(clickPoint, edge);
            if (dist < minDist) {
                minDist = dist;
                selectedId = i;
            }
        }
    }

    return selectedId;
}

// Funções para cálculo de centro
Point calculateLineCenter(Line l) {
    Point center;
    center.x = (l.init.x + l.end.x) / 2.0f;
    center.y = (l.init.y + l.end.y) / 2.0f;
    return center;
}

Point calculatePolygonCenter(Mesh m) {
    Point center = {0, 0};
    for (int i = 0; i < m.numberPoints; i++) {
        center.x += m.vertices[i].x;
        center.y += m.vertices[i].y;
    }
    center.x /= m.numberPoints;
    center.y /= m.numberPoints;
    return center;
}

// Funções de transformação
void translatePoint(int id, float dx, float dy) {
    points[id].x += dx;
    points[id].y += dy;
}

void translateLine(int id, float dx, float dy) {
    lines[id].init.x += dx;
    lines[id].init.y += dy;
    lines[id].end.x += dx;
    lines[id].end.y += dy;
}

void translatePolygon(int id, float dx, float dy) {
    for (int i = 0; i < meshes[id].numberPoints; i++) {
        meshes[id].vertices[i].x += dx;
        meshes[id].vertices[i].y += dy;
    }
}

void rotatePoint(int id, float angle) {
    // Rotação em relação à origem
    float angleRad = angle * (PI / 180.0f);
    float s = sin(angleRad);
    float c = cos(angleRad);
    float x = points[id].x;
    float y = points[id].y;

    points[id].x = x * c - y * s;
    points[id].y = x * s + y * c;
}

void rotatePointAroundCenter(Point *p, Point center, float angle) {
    float angleRad = angle * (PI / 180.0f);
    float s = sin(angleRad);
    float c = cos(angleRad);

    // Translate point to origin
    float x = p->x - center.x;
    float y = p->y - center.y;

    // Rotate point
    float xnew = x * c - y * s;
    float ynew = x * s + y * c;

    // Translate point back
    p->x = xnew + center.x;
    p->y = ynew + center.y;
}

void rotateLine(int id, float angle) {
    Point center = calculateLineCenter(lines[id]);
    rotatePointAroundCenter(&lines[id].init, center, angle);
    rotatePointAroundCenter(&lines[id].end, center, angle);
}

void rotatePolygon(int id, float angle) {
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++) {
        rotatePointAroundCenter(&meshes[id].vertices[i], center, angle);
    }
}

void scalePointAroundCenter(Point *p, Point center, float sx, float sy) {
    // Translate point to origin
    float x = p->x - center.x;
    float y = p->y - center.y;

    // Scale point
    x *= sx;
    y *= sy;

    // Translate point back
    p->x = x + center.x;
    p->y = y + center.y;
}

void scaleLine(int id, float sx, float sy) {
    Point center = calculateLineCenter(lines[id]);
    scalePointAroundCenter(&lines[id].init, center, sx, sy);
    scalePointAroundCenter(&lines[id].end, center, sx, sy);
}

void scalePolygon(int id, float sx, float sy) {
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++) {
        scalePointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}

void mirrorPointAroundAxis(Point *p, Point center, int direction) {
    if (direction == 0) { // Horizontal (em relação ao eixo y)
        p->x = 2 * center.x - p->x;
    } else { // Vertical (em relação ao eixo x)
        p->y = 2 * center.y - p->y;
    }
}

void mirrorPoint(int id, int direction) {
    // Para pontos, espelhamos em relação à origem
    Point origin = {0, 0};
    mirrorPointAroundAxis(&points[id], origin, direction);
}

void mirrorLine(int id, int direction) {
    Point center = calculateLineCenter(lines[id]);
    mirrorPointAroundAxis(&lines[id].init, center, direction);
    mirrorPointAroundAxis(&lines[id].end, center, direction);
}

void mirrorPolygon(int id, int direction) {
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++) {
        mirrorPointAroundAxis(&meshes[id].vertices[i], center, direction);
    }
}

void shearPointAroundCenter(Point *p, Point center, float sx, float sy) {
    // Translate point to origin
    float x = p->x - center.x;
    float y = p->y - center.y;

    // Apply shear
    float xnew = x + sx * y;
    float ynew = sy * x + y;

    // Translate point back
    p->x = xnew + center.x;
    p->y = ynew + center.y;
}

void shearPoint(int id, float sx, float sy) {
    // Para pontos, cisalhamento não faz sentido, mas implementamos mesmo assim
    Point origin = {0, 0};
    shearPointAroundCenter(&points[id], origin, sx, sy);
}

void shearLine(int id, float sx, float sy) {
    Point center = calculateLineCenter(lines[id]);
    shearPointAroundCenter(&lines[id].init, center, sx, sy);
    shearPointAroundCenter(&lines[id].end, center, sx, sy);
}

void shearPolygon(int id, float sx, float sy) {
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++) {
        shearPointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}

// Manipuladores de eventos
void mouse(int button, int state, int x, int y) {
    int normX = x;
    int normY = 600 - y; // Inverter Y para coordenadas OpenGL

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // Armazena a posição atual do mouse para transformações
            lastMousePos.x = normX;
            lastMousePos.y = normY;

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

                case SELECTION:
                    // Reseta seleções anteriores
                    IdSelectedPoint = -1;
                    IdSelectedLine = -1;
                    IdSelectedPolygon = -1;
                    isSelected = 0;

                    // Tenta selecionar na ordem: pontos, linhas, polígonos
                    IdSelectedPoint = selectPoint(normX, normY);

                    if (IdSelectedPoint >= 0) {
                        isSelected = 1;
                        printf("Ponto selecionado: ID %d\n", IdSelectedPoint);
                    } else {
                        IdSelectedLine = selectLine(normX, normY);

                        if (IdSelectedLine >= 0) {
                            isSelected = 1;
                            printf("Linha selecionada: ID %d\n", IdSelectedLine);
                        } else {
                            IdSelectedPolygon = selectPolygon(normX, normY);

                            if (IdSelectedPolygon >= 0) {
                                isSelected = 1;
                                printf("Polígono selecionado: ID %d\n", IdSelectedPolygon);
                            }
                        }
                    }

                    // Se um objeto foi selecionado, define o ponto de referência
                    if (isSelected) {
                        if (IdSelectedPoint >= 0) {
                            referencePoint = points[IdSelectedPoint];
                        } else if (IdSelectedLine >= 0) {
                            referencePoint = calculateLineCenter(lines[IdSelectedLine]);
                        } else if (IdSelectedPolygon >= 0) {
                            referencePoint = calculatePolygonCenter(meshes[IdSelectedPolygon]);
                        }
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
            // Fecha o polígono
            meshes[meshCount++] = tempMesh;
            tempMesh.numberPoints = 0;
            isDrawingPolygon = 0;
            printf("Polígono fechado e armazenado.\n");
        } else if (isSelected) {
            // Reseta a seleção com clique direito
            IdSelectedPoint = -1;
            IdSelectedLine = -1;
            IdSelectedPolygon = -1;
            isSelected = 0;
            currentTransform = NONE;
            printf("Seleção removida.\n");
        }
    } else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && isSelected) {
        // Botão do meio alterna entre modos de transformação
        currentTransform = (currentTransform + 1) % 5;

        switch (currentTransform) {
            case TRANSLATE:
                printf("Modo: Translação\n");
                break;
            case ROTATE:
                printf("Modo: Rotação\n");
                break;
            case SCALE:
                printf("Modo: Escala\n");
                break;
            case MIRROR:
                printf("Modo: Reflexão\n");
                break;
            case SHEAR:
                printf("Modo: Cisalhamento\n");
                break;
        }
    } else if (button == 3 && isSelected) {  // Scroll up
        // Aplica transformação específica ao objeto selecionado
        switch (currentTransform) {
            case SCALE:
                scaleFactorX *= 1.1f;
                scaleFactorY *= 1.1f;

                if (IdSelectedLine >= 0) {
                    scaleLine(IdSelectedLine, 1.1f, 1.1f);
                } else if (IdSelectedPolygon >= 0) {
                    scalePolygon(IdSelectedPolygon, 1.1f, 1.1f);
                }
                break;
            case ROTATE:
                rotationAngle += 5.0f;

                if (IdSelectedPoint >= 0) {
                    rotatePoint(IdSelectedPoint, 5.0f);
                } else if (IdSelectedLine >= 0) {
                    rotateLine(IdSelectedLine, 5.0f);
                } else if (IdSelectedPolygon >= 0) {
                    rotatePolygon(IdSelectedPolygon, 5.0f);
                }
                break;
        }
    } else if (button == 4 && isSelected) {  // Scroll down
        // Aplica transformação específica ao objeto selecionado
        switch (currentTransform) {
            case SCALE:
                scaleFactorX *= 0.9f;
                scaleFactorY *= 0.9f;

                if (IdSelectedLine >= 0) {
                    scaleLine(IdSelectedLine, 0.9f, 0.9f);
                } else if (IdSelectedPolygon >= 0) {
                    scalePolygon(IdSelectedPolygon, 0.9f, 0.9f);
                }
                break;
            case ROTATE:
                rotationAngle -= 5.0f;

                if (IdSelectedPoint >= 0) {
                    rotatePoint(IdSelectedPoint, -5.0f);
                } else if (IdSelectedLine >= 0) {
                    rotateLine(IdSelectedLine, -5.0f);
                } else if (IdSelectedPolygon >= 0) {
                    rotatePolygon(IdSelectedPolygon, -5.0f);
                }
                break;
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
            if (isDrawingPolygon && tempMesh.numberPoints > 0) {
                // Atualiza o último ponto adicionado enquanto movimenta o mouse
                tempMesh.vertices[tempMesh.numberPoints - 1] = (Point){normX, normY};
            }
            break;
        case SELECTION:
            if (isSelected && currentTransform == TRANSLATE) {
                float dx = normX - lastMousePos.x;
                float dy = normY - lastMousePos.y;

                if (IdSelectedPoint >= 0) {
                    translatePoint(IdSelectedPoint, dx, dy);
                } else if (IdSelectedLine >= 0) {
                    translateLine(IdSelectedLine, dx, dy);
                } else if (IdSelectedPolygon >= 0) {
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

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'p':
            currentMode = POLYGON;
            isDrawingPolygon = 1;
            tempMesh.numberPoints = 0;
            printf("Modo: Desenhar Polígono\n");
            break;
        case 'l':
            currentMode = LINE;
            printf("Modo: Desenhar Linha\n");
            break;
        case 'v':
            currentMode = VERTICE;
            printf("Modo: Desenhar Vértice\n");
            break;
        case 's':
            currentMode = SELECTION;
            printf("Modo: Seleção\n");
            break;
        case 't':
            if (isSelected) {
                currentTransform = TRANSLATE;
                printf("Transformação: Translação\n");
            }
            break;
        case 'r':
            if (isSelected) {
                currentTransform = ROTATE;
                printf("Transformação: Rotação\n");
            }
            break;
        case 'e':
            if (isSelected) {
                currentTransform = SCALE;
                printf("Transformação: Escala\n");
            }
            break;
        case 'm':
            if (isSelected) {
                currentTransform = MIRROR;
                printf("Transformação: Reflexão\n");
            }
            break;
        case 'c':
            if (isSelected) {
                currentTransform = SHEAR;
                printf("Transformação: Cisalhamento\n");
            }
            break;
        case 'x':
            if (isSelected && currentTransform == MIRROR) {
                mirrorDirection = 0; // Horizontal

                if (IdSelectedPoint >= 0) {
                    mirrorPoint(IdSelectedPoint, 0);
                } else if (IdSelectedLine >= 0) {
                    mirrorLine(IdSelectedLine, 0);
                } else if (IdSelectedPolygon >= 0) {
                    mirrorPolygon(IdSelectedPolygon, 0);
                }

                printf("Reflexão horizontal aplicada\n");
            }
            break;
        case 'y':
            if (isSelected && currentTransform == MIRROR) {
                mirrorDirection = 1; // Vertical

                if (IdSelectedPoint >= 0) {
                    mirrorPoint(IdSelectedPoint, 1);
                } else if (IdSelectedLine >= 0) {
                    mirrorLine(IdSelectedLine, 1);
                } else if (IdSelectedPolygon >= 0) {
                    mirrorPolygon(IdSelectedPolygon, 1);
                }

                printf("Reflexão vertical aplicada\n");
            }
            break;
        case 'a':
            if (isSelected && currentTransform == ROTATE) {
                rotationAngle -= 5.0f;

                if (IdSelectedPoint >= 0) {
                    rotatePoint(IdSelectedPoint, -5.0f);
                } else if (IdSelectedLine >= 0) {
                    rotateLine(IdSelectedLine, -5.0f);
                } else if (IdSelectedPolygon >= 0) {
                    rotatePolygon(IdSelectedPolygon, -5.0f);
                }
            }
            break;
        case 'd':
            if (isSelected && currentTransform == ROTATE) {
                rotationAngle += 5.0f;

                if (IdSelectedPoint >= 0) {
                    rotatePoint(IdSelectedPoint, 5.0f);
                } else if (IdSelectedLine >= 0) {
                    rotateLine(IdSelectedLine, 5.0f);
                } else if (IdSelectedPolygon >= 0) {
                    rotatePolygon(IdSelectedPolygon, 5.0f);
                }
            }
            break;
        case '+':
        case '=':
            if (isSelected && currentTransform == SCALE) {
                scaleFactorX *= 1.1f;
                scaleFactorY *= 1.1f;

                if (IdSelectedLine >= 0) {
                    scaleLine(IdSelectedLine, 1.1f, 1.1f);
                } else if (IdSelectedPolygon >= 0) {
                    scalePolygon(IdSelectedPolygon, 1.1f, 1.1f);
                }
            }
            break;
        case '-':
        case '_':
            if (isSelected && currentTransform == SCALE) {
                scaleFactorX *= 0.9f;
                scaleFactorY *= 0.9f;

                if (IdSelectedLine >= 0) {
                    scaleLine(IdSelectedLine, 0.9f, 0.9f);
                } else if (IdSelectedPolygon >= 0) {
                    scalePolygon(IdSelectedPolygon, 0.9f, 0.9f);
                }
            }
            break;
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float step = 5.0f;
    int shiftPressed = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    if (isSelected) {
        switch (key) {
            case GLUT_KEY_UP:
                if (currentTransform == TRANSLATE) {
                    if (IdSelectedPoint >= 0) {
                        translatePoint(IdSelectedPoint, 0, step);
                    } else if (IdSelectedLine >= 0) {
                        translateLine(IdSelectedLine, 0, step);
                    } else if (IdSelectedPolygon >= 0) {
                        translatePolygon(IdSelectedPolygon, 0, step);
                    }
                } else if (currentTransform == SHEAR && shiftPressed) {
                    shearFactorY += 0.1f;

                    if (IdSelectedPoint >= 0) {
                        shearPoint(IdSelectedPoint, 0, 0.1f);
                    } else if (IdSelectedLine >= 0) {
                        shearLine(IdSelectedLine, 0, 0.1f);
                    } else if (IdSelectedPolygon >= 0) {
                        shearPolygon(IdSelectedPolygon, 0, 0.1f);
                    }
                }
                break;

            case GLUT_KEY_DOWN:
                if (currentTransform == TRANSLATE) {
                    if (IdSelectedPoint >= 0) {
                        translatePoint(IdSelectedPoint, 0, -step);
                    } else if (IdSelectedLine >= 0) {
                        translateLine(IdSelectedLine, 0, -step);
                    } else if (IdSelectedPolygon >= 0) {
                        translatePolygon(IdSelectedPolygon, 0, -step);
                    }
                } else if (currentTransform == SHEAR && shiftPressed) {
                    shearFactorY -= 0.1f;

                    if (IdSelectedPoint >= 0) {
                        shearPoint(IdSelectedPoint, 0, -0.1f);
                    } else if (IdSelectedLine >= 0) {
                        shearLine(IdSelectedLine, 0, -0.1f);
                    } else if (IdSelectedPolygon >= 0) {
                        shearPolygon(IdSelectedPolygon, 0, -0.1f);
                    }
                }
                break;

            case GLUT_KEY_LEFT:
                if (currentTransform == TRANSLATE) {
                    if (IdSelectedPoint >= 0) {
                        translatePoint(IdSelectedPoint, -step, 0);
                    } else if (IdSelectedLine >= 0) {
                        translateLine(IdSelectedLine, -step, 0);
                    } else if (IdSelectedPolygon >= 0) {
                        translatePolygon(IdSelectedPolygon, -step, 0);
                    }
                } else if (currentTransform == SHEAR && shiftPressed) {
                    shearFactorX -= 0.1f;

                    if (IdSelectedPoint >= 0) {
                        shearPoint(IdSelectedPoint, -0.1f, 0);
                    } else if (IdSelectedLine >= 0) {
                        shearLine(IdSelectedLine, -0.1f, 0);
                    } else if (IdSelectedPolygon >= 0) {
                        shearPolygon(IdSelectedPolygon, -0.1f, 0);
                    }
                }
                break;

            case GLUT_KEY_RIGHT:
                if (currentTransform == TRANSLATE) {
                    if (IdSelectedPoint >= 0) {
                        translatePoint(IdSelectedPoint, step, 0);
                    } else if (IdSelectedLine >= 0) {
                        translateLine(IdSelectedLine, step, 0);
                    } else if (IdSelectedPolygon >= 0) {
                        translatePolygon(IdSelectedPolygon, step, 0);
                    }
                } else if (currentTransform == SHEAR && shiftPressed) {
                    shearFactorX += 0.1f;

                    if (IdSelectedPoint >= 0) {
                        shearPoint(IdSelectedPoint, 0.1f, 0);
                    } else if (IdSelectedLine >= 0) {
                        shearLine(IdSelectedLine, 0.1f, 0);
                    } else if (IdSelectedPolygon >= 0) {
                        shearPolygon(IdSelectedPolygon, 0.1f, 0);
                    }
                }
                break;
        }
    }

    glutPostRedisplay();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 800, 0, 600);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Paint - Simulador com Transformações");

    init();
    initLines();
    initPoints();
    initPolygons();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    printf("===== OpenGL Paint com Transformações Geométricas =====\n");
    printf("Modos de desenho:\n");
    printf("  v: Modo Vértice\n");
    printf("  l: Modo Linha\n");
    printf("  p: Modo Polígono\n");
    printf("  s: Modo Seleção\n\n");

    printf("Transformações (após selecionar um objeto):\n");
    printf("  t: Ativar translação (use as setas)\n");
    printf("  r: Ativar rotação (use A/D ou roda do mouse)\n");
    printf("  e: Ativar escala (use +/- ou roda do mouse)\n");
    printf("  m: Ativar reflexão (use x/y para espelhar)\n");
    printf("  c: Ativar cisalhamento (use shift + setas)\n\n");

    printf("Outras operações:\n");
    printf("  Botão do meio do mouse: Alternar entre transformações\n");
    printf("  Botão direito: Cancelar seleção ou fechar polígono\n");

    glutMainLoop();

    // Liberar memória alocada
    if (points) free(points);
    if (lines) free(lines);
    if (meshes) free(meshes);

    return 0;
}
