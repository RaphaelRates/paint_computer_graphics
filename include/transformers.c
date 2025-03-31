
#include "transformers.h"
#include <GL/glut.h>

void drawTransformInfo(int isSelected, int IdSelectedPoint, int IdSelectedLine, int IdSelectedPolygon, TransformMode currentTransform, float rotationAngle, float scaleFactorX, float scaleFactorY, float shearFactorX, float shearFactorY)
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

int selectPoint(int x, int y, int pointCount, Point *points)
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

int selectLine(int x, int y, int lineCount, Line *lines)
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

int selectPolygon(int x, int y, int meshCount, Mesh *meshes)
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

void translatePoint(int id, float dx, float dy, Point *points)
{
    points[id].x += dx;
    points[id].y += dy;
}

void translateLine(int id, float dx, float dy, Line *lines)
{
    lines[id].init.x += dx;
    lines[id].init.y += dy;
    lines[id].end.x += dx;
    lines[id].end.y += dy;
}

void translatePolygon(int id, float dx, float dy, Mesh *meshes)
{
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        meshes[id].vertices[i].x += dx;
        meshes[id].vertices[i].y += dy;
    }
}

void rotatePoint(int id, float angle, Point *points)
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

void rotateLine(int id, float angle, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    rotatePointAroundCenter(&lines[id].init, center, angle);
    rotatePointAroundCenter(&lines[id].end, center, angle);
}

void rotatePolygon(int id, float angle, Mesh *meshes)
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

void scaleLine(int id, float sx, float sy, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    scalePointAroundCenter(&lines[id].init, center, sx, sy);
    scalePointAroundCenter(&lines[id].end, center, sx, sy);
}

void scalePolygon(int id, float sx, float sy, Mesh *meshes)
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

void mirrorPoint(int id, int direction, Point *points)
{
    Point origin = {0, 0};
    mirrorPointAroundAxis(&points[id], origin, direction);
}

void mirrorLine(int id, int direction, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    mirrorPointAroundAxis(&lines[id].init, center, direction);
    mirrorPointAroundAxis(&lines[id].end, center, direction);
}

void mirrorPolygon(int id, int direction, Mesh *meshes)
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

void shearPoint(int id, float sx, float sy, Point *points)
{
    Point origin = {0, 0};
    shearPointAroundCenter(&points[id], origin, sx, sy);
}

void shearLine(int id, float sx, float sy, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    shearPointAroundCenter(&lines[id].init, center, sx, sy);
    shearPointAroundCenter(&lines[id].end, center, sx, sy);
}

void shearPolygon(int id, float sx, float sy, Mesh *meshes)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        shearPointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}