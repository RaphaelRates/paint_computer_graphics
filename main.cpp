#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_OBJECT 400
#define MAX_SUB_OBJECTS 40

// Estrutura para representar um ponto no plano 2D
typedef struct {
    int x, y;
} Point;

// Estrutura para representar uma linha com dois pontos de início e fim
typedef struct {
    Point startPoint, endPoint;
} Line;

// Estrutura para representar um polígono com um número variável de vértices
typedef struct {
    int verticeNum;
    Point vertices[MAX_SUB_OBJECTS];
} Poligono;

// Variáveis para rastrear o estado da aplicação
int isTranslating = 0;
int isRotating = 0;
int isScaling = 0;
int isMirroring = 0;
int isShearing = 0;

// Arrays para armazenar objetos
Point pointObjects[MAX_OBJECT];
Line lineObjects[MAX_OBJECT];
Poligono polygonObjects[MAX_OBJECT];

// Contadores para o número de objetos de cada tipo
int pointNum = 0;
int lineNum = 0;
int polygonNum = 0;

// Índices dos objetos selecionados
int selectedLineIndex = -1;
int selectedPointIndex = -1;
int selectedPolygonIndex = -1;

char mode = ' ';  // Modo atual da aplicação (translação, rotação, escala, etc.)
int interaction = 0;  // Controle de interação para criação de objetos

// Função para criar um ponto e adicioná-lo ao array de objetos
void createPoint(int x, int y){
    Point p = {x, y};
    pointObjects[pointNum] = p;
    pointNum++;
    interaction = 0;
};

// Função para deletar um ponto selecionado
void delPoint(){
    pointObjects[selectedPointIndex] = pointObjects[pointNum-1];
    pointNum--;
};

// Função para identificar um ponto próximo ao clique do mouse
int pickPoint(int x, int y, int t){
    for(int i = 0; i < pointNum; i++){
        if (x <= pointObjects[i].x + t && x >= pointObjects[i].x - t){
            if (y <= pointObjects[i].y + t && y >= pointObjects[i].y - t){
                return i;
            }
        }
    }
    return -1;
};

// Função para desenhar um ponto
void drawPoint(Point p){
    glPointSize(4.5);  // Ajuste o tamanho conforme necessário
    glBegin(GL_POINTS);
        glVertex2i(p.x, p.y);
    glEnd();
};

// Função para criar uma linha e adicioná-la ao array de objetos
void createLine(int x, int y){
    Line a;

    if (interaction == 0) {
        a.startPoint.x = x;
        a.startPoint.y = y;
        lineObjects[lineNum] = a;
        interaction++;
    } else {
        a.endPoint.x = x;
        a.endPoint.y = y;
        lineObjects[lineNum].endPoint.x = x;
        lineObjects[lineNum].endPoint.y = y;
        lineNum++;
        interaction = 0;
    }
};

// Função para deletar uma linha selecionada
void delLine(){
    lineObjects[selectedLineIndex] = lineObjects[lineNum-1];
    lineNum--;
};

// Função para identificar uma linha próxima ao clique do mouse
int pickLine(int mx, int my, int t) {
    for (int i = 0; i < lineNum; i++) {
        Line currentLine = lineObjects[i];
        glLineWidth(3.0);

        // Vetor da linha
        int lineVectorX = currentLine.endPoint.x - currentLine.startPoint.x;
        int lineVectorY = currentLine.endPoint.y - currentLine.startPoint.y;

        // Vetor do ponto do mouse até o ponto inicial da linha
        int toStartVectorX = mx - currentLine.startPoint.x;
        int toStartVectorY = my - currentLine.startPoint.y;

        // Vetor do ponto do mouse até o ponto final da linha
        int toEndVectorX = mx - currentLine.endPoint.x;
        int toEndVectorY = my - currentLine.endPoint.y;

        // Calcula a distância do ponto do mouse à linha
        int crossProductToStart = abs(lineVectorX * toStartVectorY - lineVectorY * toStartVectorX);
        int crossProductToEnd = abs(lineVectorX * toEndVectorY - lineVectorY * toEndVectorX);
        int lineLengthSquared = lineVectorX * lineVectorX + lineVectorY * lineVectorY;

        // Calcula a distância perpendicular do ponto do mouse à linha
        float distanceToStart = crossProductToStart / sqrt(lineLengthSquared);
        float distanceToEnd = crossProductToEnd / sqrt(lineLengthSquared);

        // Verifica se o ponto do mouse está próximo o suficiente da linha
        if (distanceToStart + distanceToEnd <= t) {
            return i; // Retorna o índice da linha selecionada
        }
    }
    return -1; // Nenhuma linha selecionada
}

// Função para desenhar uma linha
void drawLine(Line a){
    glLineWidth(1.5);

    glBegin(GL_LINES);
        glVertex2i(a.startPoint.x, a.startPoint.y);
        glVertex2i(a.endPoint.x, a.endPoint.y);
    glEnd();
};

// Função para criar um polígono e adicioná-lo ao array de objetos
void createPolygon(int x, int y){
    if (interaction == 0){
        Poligono a;
        a.verticeNum = 0;
        polygonObjects[polygonNum] = a;
        polygonNum++;
    }
    Point p = {x, y};
    int verticeNum = polygonObjects[polygonNum-1].verticeNum;
    polygonObjects[polygonNum-1].vertices[verticeNum] = p;
    interaction++;
    polygonObjects[polygonNum-1].verticeNum++;
};

// Função para deletar um polígono selecionado
void delpolygon(){
    polygonObjects[selectedPolygonIndex] = polygonObjects[polygonNum-1];
    polygonNum--;
};

// Função para verificar se um ponto está dentro de um polígono
// Retorna 1 se o ponto estiver dentro do polígono, 0 caso contrário
int isInsidePolygon(Point p, Poligono poly) {
    int num = 0;
    int verticeNum = poly.verticeNum;

    poly.vertices[verticeNum] = poly.vertices[0];
    verticeNum++;

    for (int j = 1; j < verticeNum; j++) {
        Point p1 = poly.vertices[j - 1];
        Point p2 = poly.vertices[j];
        if(((p1.x > p.x || p2.x > p.x) && ((p1.y > p.y && p2.y < p.y) || (p1.y < p.y && p2.y > p.y)) && p1.y != p2.y )){
            float xi = p1.x + (p.y - p1.y)*(p2.x - p1.x) / (p2.y - p1.y);
            if (xi > p.x || p.x == p1.x ) {
                num++;
            }
        }
    }
    // Retorna 1 se o número de interseções for ímpar (ponto dentro do polígono), 0 caso contrário
    return (num % 2);
}

// Função para identificar um polígono próximo ao clique do mouse
// Retorna o índice do polígono selecionado ou -1 se nenhum polígono estiver selecionado
int pickPolygon(int mouseX, int mouseY) {
    Point p = { mouseX, mouseY };

    for (int i = 0; i < polygonNum; i++) {
        if (isInsidePolygon(p, polygonObjects[i])) {
            return i; // Retorna o índice do polígono selecionado
        }
    }

    return -1; // Nenhum polígono selecionado
}

// Função para desenhar um polígono
void drawPolygon(Poligono a){
    int verticeNum = a.verticeNum;

    a.vertices[verticeNum] = a.vertices[0];
    verticeNum++;

    Line l;
    for (int i = 0; i < a.verticeNum; i++){
        l.startPoint.x = a.vertices[i].x;
        l.startPoint.y = a.vertices[i].y;
        l.endPoint.x = a.vertices[i+1].x;
        l.endPoint.y = a.vertices[i+1].y;
        drawLine(l);
    }
};

// Função para desenhar todos os objetos na tela
void drawObjects(){
    for (int i = 0; i < pointNum; i++) {
        if (i == selectedPointIndex) {
            glColor3f(1.0, 0.0, 0.0); // Objeto selecionado em vermelho
        } else {
            glColor3f(0.0, 0.0, 0.0);
        }
        drawPoint(pointObjects[i]);
    };
    for (int i = 0; i < lineNum; i++) {
        if (i == selectedLineIndex) {
            glColor3f(1.0, 0.0, 0.0); // Objeto selecionado em vermelho
        } else {
            glColor3f(0.0, 0.0, 0.0);
        }
        drawLine(lineObjects[i]);
    };
    for (int i = 0; i < polygonNum; i++){
        if (i == selectedPolygonIndex) {
            glColor3f(1.0, 0.0, 0.0); // Objeto selecionado em vermelho
        } else {
            glColor3f(0.0, 0.0, 0.0);
        }
        drawPolygon(polygonObjects[i]);
    }
};

// Função para aplicar uma matriz de transformação a um ponto
void applyMatrixToPoint(Point *point, float matrix[3][3]) {
    int x = point->x;
    int y = point->y;

    point->x = round(x * matrix[0][0] + y * matrix[0][1] + matrix[0][2]);
    point->y = round(x * matrix[1][0] + y * matrix[1][1] + matrix[1][2]);
}

// Função de translação
void translateSelectedObject(float dx, float dy) {
    float translationMatrix[3][3] = {
        {1.0, 0.0, dx},
        {0.0, 1.0, dy},
        {0.0, 0.0, 1.0}
    };

    if (selectedPointIndex != -1) {
        Point *selectedPoint = &pointObjects[selectedPointIndex];
        applyMatrixToPoint(selectedPoint, translationMatrix);
    } else if (selectedLineIndex != -1) {
        Line *selectedLine = &lineObjects[selectedLineIndex];
        applyMatrixToPoint(&selectedLine->startPoint, translationMatrix);
        applyMatrixToPoint(&selectedLine->endPoint, translationMatrix);
    } else if (selectedPolygonIndex != -1) {
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            applyMatrixToPoint(&selectedPolygon->vertices[i], translationMatrix);
        }
    }
}

// Função auxiliar para a rotação de um ponto em relação ao centro
void rotatePoint(Point *point, float angleDegrees, int centerX, int centerY) {
    float angleRadians = angleDegrees * (M_PI / 180.0);
    float cosTheta = cos(angleRadians);
    float sinTheta = sin(angleRadians);

    int translatedX = point->x - centerX;
    int translatedY = point->y - centerY;

    point->x = round(translatedX * cosTheta - translatedY * sinTheta) + centerX;
    point->y = round(translatedX * sinTheta + translatedY * cosTheta) + centerY;
}

// Função de rotação para uma linha
void rotateLine(Line *line, float angleDegrees) {
    Point *startPoint = &line->startPoint;
    Point *endPoint = &line->endPoint;

    // Calcular o centro da linha
    int centerX = (startPoint->x + endPoint->x) / 2;
    int centerY = (startPoint->y + endPoint->y) / 2;

    // Aplicar rotação aos pontos
    rotatePoint(startPoint, angleDegrees, centerX, centerY);
    rotatePoint(endPoint, angleDegrees, centerX, centerY);
}

// Função de rotação para um polígono
void rotatePolygon(Poligono *polygon, float angleDegrees) {
    int centerX = 0;
    int centerY = 0;

    // Calcular o centro do polígono
    for (int i = 0; i < polygon->verticeNum; i++) {
        centerX += polygon->vertices[i].x;
        centerY += polygon->vertices[i].y;
    }

    centerX /= polygon->verticeNum;
    centerY /= polygon->verticeNum;

    // Aplicar rotação a cada vértice
    for (int i = 0; i < polygon->verticeNum; i++) {
        rotatePoint(&polygon->vertices[i], angleDegrees, centerX, centerY);
    }
}

// Função de rotação em relação ao centro do objeto selecionado
void rotateSelectedObject(float angleDegrees) {
    if (selectedLineIndex != -1) {
        // Rotação para linha
        Line *selectedLine = &lineObjects[selectedLineIndex];
        rotateLine(selectedLine, angleDegrees);
    } else if (selectedPolygonIndex != -1) {
        // Rotação para polígono
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        rotatePolygon(selectedPolygon, angleDegrees);
    }
}

//
// Função auxiliar para a escala de um ponto em relação ao centro
void scalePoint(Point *point, float scaleX, float scaleY, int centerX, int centerY) {
    // Translação do ponto para o centro da escala
    int translatedX = point->x - centerX;
    int translatedY = point->y - centerY;

    // Aplica a escala aos pontos
    point->x = round(translatedX * scaleX) + centerX;
    point->y = round(translatedY * scaleY) + centerY;
}

// Função de escala para uma linha
void scaleLine(Line *line, float scaleX, float scaleY) {
    Point *startPoint = &line->startPoint;
    Point *endPoint = &line->endPoint;

    // Calcular o centro da linha
    int centerX = (startPoint->x + endPoint->x) / 2;
    int centerY = (startPoint->y + endPoint->y) / 2;

    // Aplicar escala aos pontos
    scalePoint(startPoint, scaleX, scaleY, centerX, centerY);
    scalePoint(endPoint, scaleX, scaleY, centerX, centerY);
}

// Função de escala para um polígono
void scalePolygon(Poligono *polygon, float scaleX, float scaleY) {
    int centerX = 0;
    int centerY = 0;

    // Calcular o centro do polígono
    for (int i = 0; i < polygon->verticeNum; i++) {
        centerX += polygon->vertices[i].x;
        centerY += polygon->vertices[i].y;
    }

    centerX /= polygon->verticeNum;
    centerY /= polygon->verticeNum;

    // Aplicar escala a cada vértice
    for (int i = 0; i < polygon->verticeNum; i++) {
        scalePoint(&polygon->vertices[i], scaleX, scaleY, centerX, centerY);
    }
}

// Função de escala em relação ao centro do objeto selecionado
void scaleSelectedObject(float scaleX, float scaleY) {
    if (selectedLineIndex != -1) {
        // Escala para linha
        Line *selectedLine = &lineObjects[selectedLineIndex];
        scaleLine(selectedLine, scaleX, scaleY);
    } else if (selectedPolygonIndex != -1) {
        // Escala para polígono
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        scalePolygon(selectedPolygon, scaleX, scaleY);
    }
}

// Função auxiliar para aplicar a transformação de espelhamento
void applyTransformation(Point *point, float matrix[3][3]) {
    int x = point->x;
    int y = point->y;

    // Aplica a transformação ao ponto
    point->x = round(x * matrix[0][0] + y * matrix[0][1] + matrix[0][2]);
    point->y = round(x * matrix[1][0] + y * matrix[1][1] + matrix[1][2]);
}

// Função de espelhamento em relação ao eixo X
void mirrorSelectedObjectX() {
    float matrix[3][3] = {
        {1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    float e;

    if (selectedLineIndex != -1) {
        // Espelhamento para linha
        Line *selectedLine = &lineObjects[selectedLineIndex];
        e = hypot(0, lineObjects[selectedLineIndex].endPoint.y - lineObjects[selectedLineIndex].startPoint.y);

        // Ajusta a posição do espelhamento
        if (lineObjects[selectedLineIndex].startPoint.y < lineObjects[selectedLineIndex].endPoint.y) {
            matrix[1][2] = ((e / 2) + lineObjects[selectedLineIndex].startPoint.y) * 2;
        } else {
            matrix[1][2] = ((e / 2) + lineObjects[selectedLineIndex].endPoint.y) * 2;
        }

        // Aplica a transformação aos pontos
        applyTransformation(&selectedLine->startPoint, matrix);
        applyTransformation(&selectedLine->endPoint, matrix);
    } else if (selectedPolygonIndex != -1) {
        // Espelhamento para polígono
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        Point biggerY = selectedPolygon->vertices[0];
        Point smallerY = selectedPolygon->vertices[0];

        // Encontrar os pontos extremos em relação ao eixo Y
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            if (selectedPolygon->vertices[i].y > biggerY.y) {
                biggerY = selectedPolygon->vertices[i];
            } else if (selectedPolygon->vertices[i].y < smallerY.y) {
                smallerY = selectedPolygon->vertices[i];
            }
        }

        // Calcular a altura do polígono
        e = hypot(0, biggerY.y - smallerY.y);
        matrix[1][2] = ((e / 2) + smallerY.y) * 2;

        // Aplica a transformação a cada vértice
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            applyTransformation(&selectedPolygon->vertices[i], matrix);
        }
    }
}

// Função de espelhamento em relação ao eixo Y
void mirrorSelectedObjectY() {
    float matrix[3][3] = {
        {-1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    float e;

    if (selectedLineIndex != -1) {
        // Espelhamento para linha
        Line *selectedLine = &lineObjects[selectedLineIndex];
        e = hypot(lineObjects[selectedLineIndex].endPoint.x - lineObjects[selectedLineIndex].startPoint.x, 0);

        // Ajusta a posição do espelhamento
        if (lineObjects[selectedLineIndex].startPoint.x < lineObjects[selectedLineIndex].endPoint.x) {
            matrix[0][2] = ((e / 2) + lineObjects[selectedLineIndex].startPoint.x) * 2;
        } else {
            matrix[0][2] = ((e / 2) + lineObjects[selectedLineIndex].endPoint.x) * 2;
        }

        // Aplica a transformação aos pontos
        applyTransformation(&selectedLine->startPoint, matrix);
        applyTransformation(&selectedLine->endPoint, matrix);
    } else if (selectedPolygonIndex != -1) {
        // Espelhamento para polígono
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        Point biggerX = selectedPolygon->vertices[0];
        Point smallerX = selectedPolygon->vertices[0];

        // Encontrar os pontos extremos em relação ao eixo X
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            if (selectedPolygon->vertices[i].x > biggerX.x) {
                biggerX = selectedPolygon->vertices[i];
            } else if (selectedPolygon->vertices[i].x < smallerX.x) {
                smallerX = selectedPolygon->vertices[i];
            }
        }

        // Calcular a largura do polígono
        e = hypot(biggerX.x - smallerX.x, 0);
        matrix[0][2] = ((e / 2) + smallerX.x) * 2;

        // Aplica a transformação a cada vértice
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            applyTransformation(&selectedPolygon->vertices[i], matrix);
        }
    }
}

// Função de cisalhamento em relação ao eixo X
void shearSelectedObjectX(float sh) {
    float matrix[3][3] = {
        {1.0, sh, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };

    if (selectedLineIndex != -1) {
        // Cisalhamento para linha
        Line *selectedLine = &lineObjects[selectedLineIndex];

        // Aplica a transformação ao ponto inicial ou final, dependendo da orientação da linha
        if (lineObjects[selectedLineIndex].endPoint.y < lineObjects[selectedLineIndex].startPoint.y) {
            applyTransformation(&selectedLine->startPoint, matrix);
        } else {
            applyTransformation(&selectedLine->endPoint, matrix);
        }
    } else if (selectedPolygonIndex != -1) {
        // Cisalhamento para polígono
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        Point biggerY, smallerY;
        biggerY.y = -1;
        smallerY.y = 10000000;

        // Encontrar os pontos extremos em relação ao eixo Y
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            if (selectedPolygon->vertices[i].y > biggerY.y) {
                biggerY = selectedPolygon->vertices[i];
            } else if (selectedPolygon->vertices[i].y < smallerY.y) {
                smallerY = selectedPolygon->vertices[i];
            }
        }

        // Aplica o cisalhamento apenas aos vértices acima do ponto médio vertical
        float e = hypot(0, biggerY.y - smallerY.y);
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            if (smallerY.y + e / 2 < selectedPolygon->vertices[i].y) {
                applyTransformation(&selectedPolygon->vertices[i], matrix);
            }
        }
    }
}

// Função de cisalhamento em relação ao eixo Y
void shearSelectedObjectY(float sh) {
    float matrix[3][3] = {
        {1.0, 0.0, 0.0},
        {sh, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };

    if (selectedLineIndex != -1) {
        // Cisalhamento para linha
        Line *selectedLine = &lineObjects[selectedLineIndex];

        // Aplica a transformação ao ponto inicial ou final, dependendo da orientação da linha
        if (lineObjects[selectedLineIndex].endPoint.x < lineObjects[selectedLineIndex].startPoint.x) {
            applyTransformation(&selectedLine->startPoint, matrix);
        } else {
            applyTransformation(&selectedLine->endPoint, matrix);
        }
    } else if (selectedPolygonIndex != -1) {
        // Cisalhamento para polígono
        Poligono *selectedPolygon = &polygonObjects[selectedPolygonIndex];
        Point smallerX;
        Point biggerX;
        biggerX.x = -1;
        smallerX.x = 1000000;

        // Encontrar os pontos extremos em relação ao eixo X
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            if (selectedPolygon->vertices[i].x > biggerX.x) {
                biggerX = selectedPolygon->vertices[i];
            } else if (selectedPolygon->vertices[i].x < smallerX.x) {
                smallerX = selectedPolygon->vertices[i];
            }
        }

        // Aplica o cisalhamento apenas aos vértices à direita do ponto médio horizontal
        float e = hypot(biggerX.x - smallerX.x, 0);
        for (int i = 0; i < selectedPolygon->verticeNum; i++) {
            if (smallerX.x + e / 2 < selectedPolygon->vertices[i].x) {
                applyTransformation(&selectedPolygon->vertices[i], matrix);
            }
        }
    }
}
// ---- Funções de auxílio ----

// Obtém o tamanho da janela
int* windowSize() {
    // Pega o tamanho da tela do dispositivo
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    // Calcula o tamanho da janela (por exemplo, 80% da largura e altura da tela)
    // Para ficar do tamanho da tela, basta substituir por 1
    int windowWidth = (int)(0.8 * screenWidth);
    int windowHeight = (int)(0.8 * screenHeight);

    int* window = new int[2];
    window[0] = windowWidth;
    window[1] = windowHeight;
    return window;
}

// Inicializa as configurações iniciais
void init(void) {
    int* window = windowSize();

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, window[0], 0.0, window[1]);
    glViewport(0, 0, window[0], window[1]);
}

// Função de exibição
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    drawObjects();
    glutSwapBuffers();
}

// Salva os objetos em um arquivo
void saveObjects() {
    FILE *file;
    file = fopen("/home/felipefr/Langs/C/CG/projeto-paint-cg/paint/objects.txt", "w");

    if (file == NULL) {
        printf("Error opening the file\n");
    }

    fprintf(file, "%d\n", pointNum);
    for (int i = 0; i < pointNum; i++) {
        fprintf(file, "%d %d\n", pointObjects[i].x, pointObjects[i].y);
    };
    fprintf(file, "%d\n", lineNum);
    for (int i = 0; i < lineNum; i++) {
        fprintf(file, "%d %d %d %d\n", lineObjects[i].startPoint.x, lineObjects[i].startPoint.y, lineObjects[i].endPoint.x, lineObjects[i].endPoint.y);
    };
    fprintf(file, "%d\n", polygonNum);
    for (int i = 0; i < polygonNum; i++) {
        fprintf(file, "%d ", polygonObjects[i].verticeNum);
        for (int j = 0; j < polygonObjects[i].verticeNum; j++) {
            fprintf(file, "%d %d ", polygonObjects[i].vertices[j].x, polygonObjects[i].vertices[j].y);
        };
        fprintf(file, "\n");
    };
    fclose(file);
    printf("Objetos salvos\n");
}

// Carrega os objetos de um arquivo
void loadObjects() {
    FILE *file;
    file = fopen("/home/felipefr/Langs/C/CG/projeto-paint-cg/paint/objects.txt", "rb");

    if (file == NULL) {
        printf("Error opening the file\n");
        return;
    }

    pointNum = lineNum = polygonNum = 0;

    int num;
    int aux = 0;

    if (fscanf(file, "%d", &num)) {
        aux = num;
    }

    for (int i = 0; aux != pointNum; i++) {
        fscanf(file, "%d", &num);
        if (i % 2) {
            pointObjects[pointNum].y = num;
            pointNum++;
        } else {
            pointObjects[pointNum].x = num;
        }
    }

    aux = 0;
    if (fscanf(file, "%d", &num)) {
        aux = num;
    }
    for (int i = 0, j = 0; aux != lineNum; i++) {
        fscanf(file, "%d", &num);

        if (j % 2 != 1) {
            if (i % 2) {
                lineObjects[lineNum].startPoint.y = num;
                j++;
            } else {
                lineObjects[lineNum].startPoint.x = num;
            }
        } else {
            if (i % 2) {
                lineObjects[lineNum].endPoint.y = num;
                j++;
                lineNum++;

            } else {
                lineObjects[lineNum].endPoint.x = num;
            }
        }
    }

    aux = 0;
    if (fscanf(file, "%d", &num)) {
        aux = num;
    }
    int verticeQtd = 0;
    polygonObjects[0].verticeNum = 0;

    for (int i = 0; aux != polygonNum; i++) {
        fscanf(file, "%d", &num);
        verticeQtd = num;
        polygonObjects[polygonNum].verticeNum = 0;
        for (int j = 0; verticeQtd != polygonObjects[polygonNum].verticeNum; j++) {
            fscanf(file, "%d", &num);

            int verticeNum = polygonObjects[polygonNum].verticeNum;

            if (j % 2) {
                polygonObjects[polygonNum].vertices[verticeNum].y = num;
                polygonObjects[polygonNum].verticeNum++;

            } else {
                polygonObjects[polygonNum].vertices[verticeNum].x = num;
            }
        }

        polygonNum++;
    }

    fclose(file);
    printf("Objetos carregados\n");
}

// Redimensiona a janela
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

// Manipula eventos do mouse
void mouseEvent(int button, int state, int x, int y) {
    int* window = windowSize();
    if (button == GLUT_LEFT_BUTTON) {
        int mouseX = x;
        int mouseY = window[1] - y;
        if (state == GLUT_DOWN) {
            selectedPointIndex = selectedLineIndex = selectedPolygonIndex = -1;
            switch (mode) {
                case 'p':
                    createPoint(mouseX, mouseY);
                    break;

                case 'l':
                    createLine(mouseX, mouseY);
                    break;

                case 'o':
                    createPolygon(mouseX, mouseY);
                    break;

                case 'z':
                    selectedPointIndex = pickPoint(mouseX, mouseY, 10);
                    if (selectedPointIndex != -1) {
                        break;
                    } else {
                        selectedPointIndex = -1;
                        selectedLineIndex = pickLine(mouseX, mouseY, 10);
                    }

                    if (selectedLineIndex != -1) {
                        break;
                    } else {
                        selectedLineIndex = -1;
                        selectedPolygonIndex = pickPolygon(mouseX, mouseY);
                    }

                    if (selectedPolygonIndex != -1) {
                        break;
                    } else {
                        selectedPolygonIndex = -1;
                    }
                    break;

                default:
                    break;
            };
            glutPostRedisplay();
        }
    }
}

// Manipula eventos do teclado
void keyboardEvent(unsigned char key, int x, int y) {
    int* window = windowSize();
    switch (key) {
        case 'p':
            mode = key;
            printf("%c\n", mode);
            break;
        case 'o':
            mode = 'o';
            printf("o\n");
            break;
        case 'l':
            mode = key;
            printf("l\n");
            break;
        case 's':
            saveObjects();
            break;
        case 'x':
            printf("x\n");
            break;
        case 'z':
            mode = key;
            printf("z\n");
            break;
        case 'a':
            loadObjects();
            printf("a\n");
            glutPostRedisplay();
            break;
        case 'd':
            if (selectedPointIndex != -1) {
                delPoint();
            } else if (selectedLineIndex != -1) {
                delLine();
            } else if (selectedPolygonIndex != -1) {
                delpolygon();
            }
            glutPostRedisplay();
            break;

        case 't':
            if (selectedPointIndex != -1 || selectedLineIndex != -1 || selectedPolygonIndex != -1) {
                isTranslating = !isTranslating;
                isMirroring = isScaling = isShearing = isRotating = 0;  // Corrigido: removido o '0' extra
            }
            break;

        case 'r':
            if (selectedPointIndex != -1 || selectedLineIndex != -1 || selectedPolygonIndex != -1) {
                isRotating = !isRotating;
                isMirroring = isScaling = isShearing = isTranslating = 0;
            }
            break;

        case 'e':
            if (selectedPointIndex != -1 || selectedLineIndex != -1 || selectedPolygonIndex != -1) {
                isScaling = !isScaling;
                isMirroring = isRotating = isShearing = isTranslating = 0;
            }
            break;
        case 'm':
            if (selectedPointIndex != -1 || selectedLineIndex != -1 || selectedPolygonIndex != -1) {
                isMirroring = !isMirroring;
                isRotating = isTranslating = isShearing =  isScaling = 0;
            }
            break;

        case 'c':
            if (selectedPointIndex != -1 || selectedLineIndex != -1 || selectedPolygonIndex != -1) {
                isShearing = !isShearing;
                isRotating = isTranslating = isMirroring =  isScaling = 0;
            }
            break;
        default:
            break;
    };
    interaction = 0;
}

// Manipula eventos de teclado especial
void specialKeyboardEvent(int key, int x, int y) {
    if ((isTranslating || isRotating || isScaling || isMirroring || isShearing) && (selectedPointIndex != -1 || selectedLineIndex != -1 || selectedPolygonIndex != -1)) {
        switch (key) {

            case GLUT_KEY_UP:
                if (isScaling) {
                    scaleSelectedObject(1.1, 1.1);
                } else if (isTranslating) {
                    translateSelectedObject(0, 10);
                } else if (isRotating) {
                    rotateSelectedObject(15.0);
                } else if (isMirroring) {
                    mirrorSelectedObjectX();
                } else if (isShearing) {
                    shearSelectedObjectY(0.02);
                }
                break;

            case GLUT_KEY_DOWN:
                if (isScaling) {
                    scaleSelectedObject(0.9, 0.9);
                } else if (isTranslating) {
                    translateSelectedObject(0, -10);
                } else if (isRotating) {
                    rotateSelectedObject(-15.0);
                } else if (isMirroring) {
                    mirrorSelectedObjectX();
                } else if (isShearing) {
                    shearSelectedObjectY(-0.02);
                }
                break;

            case GLUT_KEY_LEFT:
                if (isScaling) {
                    scaleSelectedObject(0.9, 1.0);
                } else if (isTranslating) {
                    translateSelectedObject(-10, 0);
                } else if (isRotating) {
                    rotateSelectedObject(15.0);
                } else if (isMirroring) {
                    mirrorSelectedObjectY();
                } else if (isShearing) {
                    shearSelectedObjectX(-0.02);
                }
                break;

            case GLUT_KEY_RIGHT:
                if (isScaling) {
                    scaleSelectedObject(1.1, 1.0);
                } else if (isTranslating) {
                    translateSelectedObject(10, 0);
                } else if (isRotating) {
                    rotateSelectedObject(-15.0);
                } else if (isMirroring) {
                    mirrorSelectedObjectY();
                } else if (isShearing) {
                    shearSelectedObjectX(0.02);
                }
                break;
        }
        glutPostRedisplay();
    }
}

// Função principal
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    int* window = windowSize();
    glutCreateWindow("Paint");
    glutReshapeWindow(window[0], window[1]);
    glutInitWindowPosition(200, 0);

    init();
    glutSpecialFunc(specialKeyboardEvent);
    glutMouseFunc(mouseEvent);
    glutKeyboardFunc(keyboardEvent);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}