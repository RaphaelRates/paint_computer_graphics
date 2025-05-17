# Paint em OpenGL - Computação Gráfica 

Este é um projeto desenvolvido para a disciplina de **Computação Gráfica** da **Universidade Federal do Cariri (UFCA)**. O objetivo é criar uma aplicação em **OpenGL** e **C** que funcione como um editor gráfico simples, semelhante ao *Paint*.

## 🎨 Funcionalidades

### 🎭 1. Criação de Objetos Primários
- **Ponto**
- **Segmento de reta**
- **Polígono**

O usuário pode criar quantos objetos desejar.

### 🔍 2. Seleção e Exclusão de Objetos
- **Selecionar objetos individualmente** (ponto, segmento de reta e polígono)
- **Excluir objetos selecionados**

### 🔄 3. Transformações Geométricas
- **Translação (arrastar e soltar)**
- **Rotação em relação ao centro do objeto**
  - No caso do ponto, é em relação à origem do eixo de coordenadas
- **Escala em relação ao centro do objeto** (exceto para ponto)
- **Reflexão**
- **Cisalhamento**

### 🖱️ 4. Entrada de Evento
As ações podem ser executadas por meio de:
- **Clique do mouse**
- **Botão de rolagem do mouse**
- **Teclado**

### 💾 5. Salvamento e Carregamento de Objetos 
- **Salvar os objetos geométricos criados em um arquivo**
- **Carregar os objetos geométricos de um arquivo**

### 🎥 6. Ponto Extra: Animações Criativas
Implementação de uma funcionalidade extra para animar os objetos criados.

## 📁 Estrutura do Projeto

```plaintext
/
├── include/               # Arquivos de cabeçalho
│   ├── draw.h             # Header para funções de desenho
│   ├── transform.h        # Header para transformações geométricas
│   ├── arqs.h             # Header para manipulação de arquivos
|   ├── data.h             # arquivos dos structs e cabeçalhs das funfções principais
|   ├── calculate.h        # Header para funções de calculos geométricos
├── README.md              # Este arquivo
└── main.c                 # Documentação do projeto
```

## ⚙️ Como Compilar e Executar

```
Em Manutenção
```

## 🖼️ Sobre o Sistema de Desenho

O sistema utiliza **OpenGL com GLUT** para representar interativamente **pontos, linhas e polígonos** em uma janela gráfica.

---

### ✏️ Desenhando Formas

- **`drawPoint`**  
  Desenha todos os pontos na tela.  
  Se o ponto está selecionado, ele é renderizado em **vermelho**.
```c
  void drawPoint(Point *points, int pointCount, int isSelected, int IdSelectedPoint) {
      glPointSize(5.0);
      glBegin(GL_POINTS);
      for (int i = 0; i < pointCount; i++) {
          glColor3f((i == IdSelectedPoint && isSelected) ? 1.0f : 0.0f, 0.0f, 0.0f);
          glVertex2f(points[i].x, points[i].y);
      }
      glEnd();
  }
```

- **`drawLines`**  
  Desenha as linhas entre dois pontos (início e fim).  
  A linha selecionada também é exibida em **vermelho**.
```c
void drawLines(Line *lines, int lineCount, int IdSelectedLine, int isSelected) {
    glLineWidth(2.0);
    for (int i = 0; i < lineCount; i++) {
        glColor3f((i == IdSelectedLine && isSelected) ? 1.0f : 0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(lines[i].init.x, lines[i].init.y);
        glVertex2f(lines[i].end.x, lines[i].end.y);
        glEnd();
    }
}
```

- **`drawPolygon`**  
  Usa `GL_LINE_LOOP` para conectar todos os vértices de um polígono.  
  Polígonos selecionados são desenhados em **vermelho**.
```c
void drawPolygon(Mesh *meshes, int meshCount, int IdSelectedPolygon, int isSelected) {
    glLineWidth(2.0);
    for (int i = 0; i < meshCount; i++) {
        glColor3f((i == IdSelectedPolygon && isSelected) ? 1.0f : 0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < meshes[i].numberPoints; j++) {
            glVertex2f(meshes[i].vertices[j].x, meshes[i].vertices[j].y);
        }
        glEnd();
    }
}
```

---

### 👁️ Pré-visualização

Durante o desenho de uma nova forma, é exibida uma pré-visualização:

- **`drawPreviewPoint`**  
  Mostra o ponto atual antes de ser confirmado.
```c
void drawPreviewPoint(Mode currentMode, int isDrawing, Point tempPoint) {
  if (currentMode == VERTICE && isDrawing) {
      glColor3f(0.8f, 1.0f, 0.4f);
      glPointSize(5.0);
      glBegin(GL_POINTS);
      glVertex2f(tempPoint.x, tempPoint.y);
      glEnd();
  }
}
```

- **`drawPreviewLine`**  
  Mostra dinamicamente a linha em construção, com **cor configurável**.
```c
void drawPreviewLine(Mode currentMode, int isDrawing, Point tempPoint, float r, float g, float b, Line tempLine) {
    if (currentMode == LINE && isDrawing) {
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        glVertex2f(tempLine.init.x, tempLine.init.y);
        glVertex2f(tempLine.end.x, tempLine.end.y);
        glEnd();
    }
}
```

- **`drawPreviewPolygon`**  
  Exibe o contorno do polígono durante sua criação, com os pontos temporários.
```c
void drawPreviewPolygon(int isDrawingPolygon, float r, float g, float b, Mesh tempMesh) {
    if (isDrawingPolygon) {
        glColor3f(r, g, b);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < tempMesh.numberPoints; i++) {
            glVertex2f(tempMesh.vertices[i].x, tempMesh.vertices[i].y);
        }
        glEnd();
    }
}
```

---

### 💬 Mensagens na Tela

- **`drawMessage`**  
  Exibe mensagens como:
  - `"Objeto deletado"`
  - `"Arquivo salvo"`
  - `"Modo: Linha"`, etc.  
  As mensagens mudam de **cor** de acordo com o contexto:
  - **Verde** para sucesso
  - **Vermelho** para erro
  - **Amarelo** para alertas
```c
void drawMessage(MESSAGE showMessage) {
    int *window = windowSize();
    switch (showMessage) {
        case SAVE_MESH: glColor3f(0, 0.7, 0); break;
        case LOAD_MESH: glColor3f(0.6, 0.9, 0); break;
        case DELETED:   glColor3f(0.8, 0.0, 0.1); break;
        default:        glColor3f(0.0, 0.0, 0.0); break;
    }

    glRasterPos2f((window[0] * 3) / 7, window[1] - 20);
    const char *msg;
    switch (showMessage) {
        case LINE_LOG:     msg = "Desenho para Linha"; break;
        case POLYLGON_LOG: msg = "Desenho para Poligono"; break;
        case VERTCIE_LOG:  msg = "Desenho para Vertice"; break;
        case SAVE_MESH:    msg = "Arquivo salvo com sucesso!"; break;
        case LOAD_MESH:    msg = "Arquivo carregado com sucesso!"; break;
        case DELETED:      msg = "Objeto deletado!"; break;
        case IN_CONCERT:   msg = "Funcionalidade em manutencao, sentimos muito!"; break;
        default:           msg = "PAINT SIMULATOR"; break;
    }

    for (const char *c = msg; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}
```

---

### 🔵 Destaque de Seleção

- **`drawSelectedObject`**  
  Desenha um **círculo azul claro** em volta do objeto selecionado: ponto, linha ou polígono.  
  Para isso:
  - Calcula o **centro geométrico** da linha ou do polígono
  - Cria um **círculo** com raio baseado na distância até os vértices/extremos
```c
void drawSelectedObject(int isSelected, int IdSelectedPoint, int IdSelectedLine, int IdSelectedPolygon, Point *points, Line *lines, Mesh *meshes) {
    if (!isSelected) return;

    glColor3f(0.0f, 0.5f, 1.0f);
    glLineWidth(1.0);
    int segments = 20;

    if (IdSelectedPoint >= 0) {
        float radius = 8.0f;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * PI * i / segments;
            float x = points[IdSelectedPoint].x + radius * cosf(theta);
            float y = points[IdSelectedPoint].y + radius * sinf(theta);
            glVertex2f(x, y);
        }
        glEnd();
    }
    else if (IdSelectedLine >= 0) {
        Point center = calculateLineCenter(lines[IdSelectedLine]);
        float radius = distance(center, lines[IdSelectedLine].init) + 10.0f;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * PI * i / segments;
            float x = center.x + radius * cosf(theta);
            float y = center.y + radius * sinf(theta);
            glVertex2f(x, y);
        }
        glEnd();
    }
    else if (IdSelectedPolygon >= 0) {
        Point center = calculatePolygonCenter(meshes[IdSelectedPolygon]);
        float maxDist = 0.0f;
        for (int j = 0; j < meshes[IdSelectedPolygon].numberPoints; j++) {
            float dist = distance(center, meshes[IdSelectedPolygon].vertices[j]);
            if (dist > maxDist) maxDist = dist;
        }
        float radius = maxDist + 10.0f;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * PI * i / segments;
            float x = center.x + radius * cosf(theta);
            float y = center.y + radius * sinf(theta);
            glVertex2f(x, y);
        }
        glEnd();
    }
}
```

## 📐 Funções de Cálculo Geométrico

As funções abaixo são responsáveis por cálculos essenciais para manipulação dos objetos gráficos:

```c
/**
 * @brief Calcula o ponto central de uma linha.
 * @param l Estrutura da linha.
 * @return Ponto central da linha.
 */
Point calculateLineCenter(Line l) {
    Point center;
    center.x = (l.init.x + l.end.x) / 2.0f;
    center.y = (l.init.y + l.end.y) / 2.0f;
    return center;
}

/**
 * @brief Calcula o ponto central de um polígono (malha).
 * @param m Estrutura da malha.
 * @return Ponto central do polígono.
 */
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

/**
 * @brief Calcula a distância entre dois pontos.
 * @param p1 Primeiro ponto.
 * @param p2 Segundo ponto.
 * @return Distância entre os dois pontos.
 */
float distance(Point p1, Point p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}
```
## 📄 Exemplo de Implementação: Salvamento e Carregamento de Objetos

A seguir, um exemplo de implementação das funções para salvar e carregar objetos geométricos em arquivo (`arqs.h` e `arqs.c`):

```c
#ifndef ARQS_H
#define ARQS_H

#include <stdio.h>
#include <stdlib.h>
#include "data.h"

/**
 * @brief Salva os objetos desenhados em um arquivo.
 * @param filename Nome do arquivo de destino.
 * @param pointCount Número de pontos.
 * @param lineCount Número de linhas.
 * @param meshCount Número de malhas.
 * @param points Ponteiro para os pontos.
 * @param lines Ponteiro para as linhas.
 * @param meshes Ponteiro para as malhas.
 * @return Retorna 1 se bem-sucedido, 0 caso contrário.
 */
int saveObjectsToFile(const char *filename, int pointCount, int lineCount, int meshCount, Point *points, Line *lines, Mesh *meshes);

/**
 * @brief Carrega os objetos desenhados a partir de um arquivo.
 * @param filename Nome do arquivo de origem.
 * @param pointCount Ponteiro para armazenar o número de pontos.
 * @param lineCount Ponteiro para armazenar o número de linhas.
 * @param meshCount Ponteiro para armazenar o número de malhas.
 * @param points Ponteiro para armazenar os pontos carregados.
 * @param lines Ponteiro para armazenar as linhas carregadas.
 * @param meshes Ponteiro para armazenar as malhas carregadas.
 * @param tempMesh Estrutura temporária de malha.
 * @return Retorna 1 se bem-sucedido, 0 caso contrário.
 */
int loadObjectsFromFile(const char *filename, int *pointCount, int *lineCount, int *meshCount, Point **points, Line **lines, Mesh **meshes, Mesh *tempMesh);

#endif
```

```c
#include "arqs.h"

int saveObjectsToFile(const char *filename, int pointCount, int lineCount, int meshCount, Point *points, Line *lines, Mesh *meshes) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return 0;
    }

    fprintf(file, "[PONTOS]\n");
    fprintf(file, "Cont: %d\n", pointCount);
    for (int i = 0; i < pointCount; i++) {
        fprintf(file, "P%d: %.f, %.f\n", i, points[i].x, points[i].y);
    }
    fprintf(file, "\n");

    fprintf(file, "[LINHAS]\n");
    fprintf(file, "Cont: %d\n", lineCount);
    for (int i = 0; i < lineCount; i++) {
        fprintf(file, "L%d: (%.f, %.f) -> (%.f, %.f)\n",
                i, lines[i].init.x, lines[i].init.y,
                lines[i].end.x, lines[i].end.y);
    }
    fprintf(file, "\n");

    fprintf(file, "[POLIGONOS]\n");
    fprintf(file, "Cont: %d\n", meshCount);
    for (int i = 0; i < meshCount; i++) {
        fprintf(file, "Poligono %d - Pontas: %d\n", i, meshes[i].numberPoints);
        for (int j = 0; j < meshes[i].numberPoints; j++) {
            fprintf(file, "  V%d: %.f, %.f\n",
                    j, meshes[i].vertices[j].x, meshes[i].vertices[j].y);
        }
    }

    fclose(file);
    printf("Os objetos foram salvos no arquivo %s.\n", filename);
    return 1;
}

int loadObjectsFromFile(const char *filename, int *pointCount, int *lineCount, int *meshCount, Point **points, Line **lines, Mesh **meshes, Mesh *tempMesh) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir ao carregar objetos do arquivo.\n");
        return 0;
    }

    *pointCount = 0;
    *lineCount = 0;
    *meshCount = 0;
    tempMesh->numberPoints = 0;

    char line[256];
    int currentSection = 0;
    int currentPolygonIndex = -1;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        if (strcmp(line, "[PONTOS]") == 0) {
            currentSection = 1;
            continue;
        } else if (strcmp(line, "[LINHAS]") == 0) {
            currentSection = 2;
            continue;
        } else if (strcmp(line, "[POLIGONOS]") == 0) {
            currentSection = 3;
            continue;
        }

        switch (currentSection) {
        case 1: { // Carregar pontos
            if (strstr(line, "Cont:") != NULL) continue;

            float x, y;
            if (sscanf(line, "P%d: %f, %f", &(int){0}, &x, &y) == 3) {
                if (*pointCount < MAX_SHAPES) {
                    (*points)[*pointCount] = (Point){x, y};
                    (*pointCount)++;
                }
            }
            break;
        }
        case 2: { // Carregar linhas
            if (strstr(line, "Cont:") != NULL) continue;

            Point init, end;
            if (sscanf(line, "L%d: (%f, %f) -> (%f, %f)",
                       &(int){0}, &init.x, &init.y, &end.x, &end.y) == 5) {
                if (*lineCount < MAX_SHAPES) {
                    (*lines)[*lineCount] = (Line){init, end};
                    (*lineCount)++;
                }
            }
            break;
        }
        case 3: { // Carregar polígonos
            if (strstr(line, "Cont:") != NULL) continue;

            if (strstr(line, "Poligono") != NULL) {
                if (currentPolygonIndex >= 0 && (*meshes)[currentPolygonIndex].numberPoints > 0) {
                    (*meshCount)++;
                }

                currentPolygonIndex++;
                if (currentPolygonIndex >= MAX_SHAPES) break;

                int numPoints;
                if (sscanf(line, "Poligono %d - Pontas: %d", &(int){0}, &numPoints) == 2) {
                    (*meshes)[currentPolygonIndex].numberPoints = 0;
                }
                continue;
            }

            float x, y;
            if (sscanf(line, "  V%d: %f, %f", &(int){0}, &x, &y) == 3) {
                if (currentPolygonIndex >= 0 &&
                    currentPolygonIndex < MAX_SHAPES &&
                    (*meshes)[currentPolygonIndex].numberPoints < MAX_POLYGON_POINTS) {
                    (*meshes)[currentPolygonIndex].vertices[(*meshes)[currentPolygonIndex].numberPoints++] = (Point){x, y};
                }
            }
            break;
        }
        }
    }

    if (currentPolygonIndex >= 0 && (*meshes)[currentPolygonIndex].numberPoints > 0) {
        (*meshCount)++;
    }

    fclose(file);
    printf("Os objetos foram carregados!\n");
    return 1;
}
```

Essas funções permitem salvar e restaurar todos os objetos criados pelo usuário, facilitando a persistência do trabalho no editor gráfico.


## 🏎️ Algoritmo QuickHull para Fecho Convexo

O **QuickHull** é um algoritmo eficiente para encontrar o fecho convexo de um conjunto de pontos no plano. Abaixo está um exemplo de implementação em C, dividido em arquivo de cabeçalho (`quickhull.h`) e implementação (`quickhull.c`).

### quickhull.h

```c
#ifndef QUICKHULL_H
#define QUICKHULL_H

#include "data.h"

/**
 * @brief Executa o algoritmo QuickHull para encontrar o fecho convexo.
 * @param pontos Vetor de pontos.
 * @param n Número de pontos.
 */
void quickHull(Point pontos[], int n);

#endif
```

### quickhull.c

```c
#include <stdio.h>
#include <stdlib.h>
#include "quickhull.h"

/**
 * @brief Determina de que lado do segmento ab o ponto p está.
 * @return 1 se à esquerda, -1 se à direita, 0 se colinear.
 */
int lado(Point a, Point b, Point p) {
    int val = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
}

/**
 * @brief Calcula a distância do ponto p ao segmento ab.
 */
int distancia(Point a, Point b, Point p) {
    return abs((b.x - a.x) * (a.y - p.y) - (a.x - p.x) * (b.y - a.y));
}

/**
 * @brief Função recursiva principal do QuickHull.
 */
void quickHullRec(Point pontos[], int n, Point a, Point b, int ladoRef) {
    int indice = -1;
    int maxDist = 0;

    for (int i = 0; i < n; i++) {
        int dist = distancia(a, b, pontos[i]);
        if (lado(a, b, pontos[i]) == ladoRef && dist > maxDist) {
            indice = i;
            maxDist = dist;
        }
    }

    if (indice == -1) {
        printf("(%d, %d)\n", a.x, a.y);
        printf("(%d, %d)\n", b.x, b.y);
        return;
    }

    quickHullRec(pontos, n, pontos[indice], a, -lado(a, pontos[indice], b));
    quickHullRec(pontos, n, pontos[indice], b, -lado(b, pontos[indice], a));
}

/**
 * @brief Função principal para encontrar o fecho convexo usando QuickHull.
 */
void quickHull(Point pontos[], int n) {
    if (n < 3) {
        printf("Nao e possivel formar fecho convexo com menos de 3 pontos\n");
        return;
    }

    int min = 0, max = 0;
    for (int i = 1; i < n; i++) {
        if (pontos[i].x < pontos[min].x)
            min = i;
        if (pontos[i].x > pontos[max].x)
            max = i;
    }

    quickHullRec(pontos, n, pontos[min], pontos[max], 1);
    quickHullRec(pontos, n, pontos[min], pontos[max], -1);
}
```

### 📌 Como Usar

- Inclua `quickhull.h` no seu projeto.
- Chame `quickHull(pontos, n)` passando o vetor de pontos e sua quantidade.
- O fecho convexo será impresso no terminal como pares de coordenadas.

---

Essa implementação pode ser adaptada para armazenar os pontos do fecho convexo em um vetor, caso deseje utilizá-los graficamente no seu editor.



## ✨ Código Completo: Funções de Transformação e Seleção

A seguir está a implementação das funções de transformação geométrica (translação, rotação, escala, reflexão, cisalhamento) e seleção de objetos (ponto, linha, polígono), além de utilitários para cálculo de distância e detecção de ponto em polígono. Essas funções são essenciais para manipulação interativa dos objetos gráficos no editor.

```c
#include "transformers.h"
#include <GL/glut.h>

// Exibe informações sobre a transformação atual do objeto selecionado
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

        for (char *c = info; *c != '\0'; c++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }
}

// Calcula a distância de um ponto a uma linha
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
    t = (t < 0) ? 0 : (t > 1) ? 1 : t;

    float projx = x1 + t * (x2 - x1);
    float projy = y1 + t * (y2 - y1);

    return sqrt((x0 - projx) * (x0 - projx) + (y0 - projy) * (y0 - projy));
}

// Testa se um ponto está dentro de um polígono (algoritmo do raio)
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

// Seleciona o ponto mais próximo do clique
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

// Seleciona a linha mais próxima do clique
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

// Seleciona o polígono que contém o clique ou o mais próximo
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

// Translação de ponto
void translatePoint(int id, float dx, float dy, Point *points)
{
    points[id].x += dx;
    points[id].y += dy;
}

// Translação de linha
void translateLine(int id, float dx, float dy, Line *lines)
{
    lines[id].init.x += dx;
    lines[id].init.y += dy;
    lines[id].end.x += dx;
    lines[id].end.y += dy;
}

// Translação de polígono
void translatePolygon(int id, float dx, float dy, Mesh *meshes)
{
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        meshes[id].vertices[i].x += dx;
        meshes[id].vertices[i].y += dy;
    }
}

// Rotação de ponto em torno da origem
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

// Rotação de ponto em torno de um centro
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

// Rotação de linha em torno do centro
void rotateLine(int id, float angle, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    rotatePointAroundCenter(&lines[id].init, center, angle);
    rotatePointAroundCenter(&lines[id].end, center, angle);
}

// Rotação de polígono em torno do centro
void rotatePolygon(int id, float angle, Mesh *meshes)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        rotatePointAroundCenter(&meshes[id].vertices[i], center, angle);
    }
}

// Escala de ponto em torno de um centro
void scalePointAroundCenter(Point *p, Point center, float sx, float sy)
{
    float x = p->x - center.x;
    float y = p->y - center.y;

    x *= sx;
    y *= sy;

    p->x = x + center.x;
    p->y = y + center.y;
}

// Escala de linha
void scaleLine(int id, float sx, float sy, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    scalePointAroundCenter(&lines[id].init, center, sx, sy);
    scalePointAroundCenter(&lines[id].end, center, sx, sy);
}

// Escala de polígono
void scalePolygon(int id, float sx, float sy, Mesh *meshes)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        scalePointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}

// Reflexão de ponto em relação a um eixo (0 = X, 1 = Y)
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

// Reflexão de ponto em relação à origem
void mirrorPoint(int id, int direction, Point *points)
{
    Point origin = {0, 0};
    mirrorPointAroundAxis(&points[id], origin, direction);
}

// Reflexão de linha em relação ao centro
void mirrorLine(int id, int direction, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    mirrorPointAroundAxis(&lines[id].init, center, direction);
    mirrorPointAroundAxis(&lines[id].end, center, direction);
}

// Reflexão de polígono em relação ao centro
void mirrorPolygon(int id, int direction, Mesh *meshes)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        mirrorPointAroundAxis(&meshes[id].vertices[i], center, direction);
    }
}

// Cisalhamento de ponto em torno de um centro
void shearPointAroundCenter(Point *p, Point center, float sx, float sy)
{
    float x = p->x - center.x;
    float y = p->y - center.y;

    float xnew = x + sx * y;
    float ynew = sy * x + y;

    p->x = xnew + center.x;
    p->y = ynew + center.y;
}

// Cisalhamento de ponto em relação à origem
void shearPoint(int id, float sx, float sy, Point *points)
{
    Point origin = {0, 0};
    shearPointAroundCenter(&points[id], origin, sx, sy);
}

// Cisalhamento de linha
void shearLine(int id, float sx, float sy, Line *lines)
{
    Point center = calculateLineCenter(lines[id]);
    shearPointAroundCenter(&lines[id].init, center, sx, sy);
    shearPointAroundCenter(&lines[id].end, center, sx, sy);
}

// Cisalhamento de polígono
void shearPolygon(int id, float sx, float sy, Mesh *meshes)
{
    Point center = calculatePolygonCenter(meshes[id]);
    for (int i = 0; i < meshes[id].numberPoints; i++)
    {
        shearPointAroundCenter(&meshes[id].vertices[i], center, sx, sy);
    }
}
```

Essas funções permitem ao usuário selecionar e transformar objetos gráficos de forma interativa, proporcionando uma experiência completa de edição no ambiente OpenGL.



## 🛠️ Tecnologias Utilizadas
- **Linguagem**: C
- **Bibliotecas**: OpenGL, GLUT,
- **Bibliotecas desenvolvidas**: data.h,  arqs.h, trasformers.h, draw.h, calculate.h
- **Compilador**: GCC

## 👥 Integrantes da Equipe
- **José Denis**
- **Raphael Sousa Rabelo Rates**
- **Gabriela Queiroga**

---
Projeto desenvolvido para a disciplina de **Computação Gráfica** - Universidade Federal do Cariri (UFCA).

