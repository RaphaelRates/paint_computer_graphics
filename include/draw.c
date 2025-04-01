
#include <data.h>
#include <draw.h>
#include <calculate.h>

void drawPoint(Point *points, int pointCount,int isSelected,int IdSelectedPoint)
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

void drawLines(Line *lines, int lineCount, int IdSelectedLine, int isSelected)
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

void drawPolygon(Mesh *meshes, int meshCount, int IdSelectedPolygon, int isSelected)
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

void drawPreviewPoint(Mode currentMode, int isDrawing, Point tempPoint)
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

void drawPreviewLine(Mode currentMode, int isDrawing, Point tempPoint, float colorLoading_r, float colorLoading_g, float colorLoading_b, Line tempLine)
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

void drawPreviewPolygon(int isDrawingPolygon, float colorLoading_r, float colorLoading_g, float colorLoading_b, Mesh tempMesh)
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

void drawMessage(MESSAGE showMessage)
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
        msg = "Funcionalidade em manutencao, sentimos muito!";
        break;
    default:
        msg = "PAINT SIMULATOR";
        break;
    }

    for (const char *c = msg; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawSelectedObject(int isSelected, int IdSelectedPoint,int IdSelectedLine, int IdSelectedPolygon, Point *points,Line *lines, Mesh *meshes)
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