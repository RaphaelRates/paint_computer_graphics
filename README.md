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

## 📦 Estruturas e Variáveis Globais

Abaixo estão as principais estruturas, variáveis globais e importações utilizadas no projeto, essenciais para o funcionamento do editor gráfico.

### 📥 Importações

O projeto utiliza as seguintes bibliotecas e arquivos:

- **OpenGL/GLUT** (`glut.h`): Renderização gráfica.
- **stdio.h**: Entrada e saída padrão (ex: `printf`).
- **stdlib.h**: Gerenciamento de memória.
- **math.h**: Funções matemáticas.
- **time.h**: Controle de tempo e animações.
- **Arquivos do projeto**:
    - `data.h`: Definições de structs e funções principais.
    - `arqs.c`: Funções de salvamento/carregamento.
    - `draw.c`: Funções de desenho e pré-visualização.
    - `transformers.c`: Transformações geométricas (escala, translação, rotação, cisalhamento, reflexão).
    - `calculate.c`: Funções auxiliares de cálculo.

### 🗂️ Estruturas de Dados

```c
// Estruturas para manipulação de arquivos BMP (exportação futura)
#pragma pack(push, 1)
typedef struct {
        unsigned char bfType[2];
        unsigned int bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        unsigned int bfOffBits;
} BMPHeader;

typedef struct {
        unsigned int biSize;
        int biWidth, biHeight;
        unsigned short biPlanes, biBitCount;
        unsigned int biCompression, biSizeImage;
        int biXPelsPerMeter, biYPelsPerMeter;
        unsigned int biClrUsed, biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)
```

### 🧩 Variáveis Globais

- **Elementos gráficos**:
    - `Point *points`, `Line *lines`, `Mesh *meshes`: Vetores dinâmicos para pontos, linhas e polígonos.
    - `Mesh tempMesh`, `Point tempPoint`, `Line tempLine`: Estruturas temporárias para desenho.

- **Contadores**:
    - `int pointCount, lineCount, meshCount`: Quantidade de cada elemento desenhado.

- **Seleção e Transformação**:
    - `int IdSelectedPoint, IdSelectedLine, IdSelectedPolygon`: IDs dos elementos selecionados.
    - `int isSelected`: Flag de seleção.
    - `TransformMode currentTransform`: Modo de transformação atual.
    - `float rotationAngle, scaleFactorX, scaleFactorY, shearFactorX, shearFactorY`: Parâmetros de transformação.
    - `int mirrorDirection`: Direção da reflexão.

- **Controle de desenho e entrada**:
    - `int isDrawing, isDrawingPolygon`: Flags de desenho.
    - `Mode currentMode`: Modo de desenho atual.
    - `int joystickActive, joystickButton, buttonMask, joystickX, joystickY`: Controle de joystick.
    - `int lastMouseX, lastMouseY, MousseActvive`: Controle de mouse.

- **Mensagens e feedback**:
    - `MESSAGE showMessage`: Mensagem exibida na tela.

- **Animação extra (gato)**:
    - `CatAnimationState catState`: Estado da animação do gato.
    - `float catX, catY, catSize, catAngle, catScaleFactor, catAnimationTime, catEarWiggle, catTailAngle`: Parâmetros de animação.
    - `int catBlinkCounter, catIsBlinking`: Controle de piscada.
    - `clock_t lastFrameTime`: Controle de tempo para animação.

---

Essas definições garantem a base para manipulação eficiente dos objetos gráficos, controle de entrada do usuário e animações criativas no editor.

## 🧰 Funções Utilitárias e Gerenciamento de Objetos

Abaixo estão exemplos de funções utilitárias para gerenciamento de janela, inicialização de estruturas e exclusão de objetos selecionados no editor gráfico.

```c
// Retorna o tamanho da janela (80% da tela)
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

// Inicializa o vetor de linhas
void initLines()
{
    if (lines == NULL)
        lines = (Line *)malloc(MAX_SHAPES * sizeof(Line));
}

// Inicializa o vetor de pontos
void initPoints()
{
    if (points == NULL)
        points = (Point *)malloc(MAX_SHAPES * sizeof(Point));
}

// Inicializa o vetor de polígonos
void initPolygons()
{
    if (meshes == NULL)
        meshes = (Mesh *)malloc(MAX_SHAPES * sizeof(Mesh));
}

// Remove o objeto selecionado (ponto, linha ou polígono)
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
```

Essas funções garantem a correta alocação de memória para os objetos gráficos e permitem a exclusão eficiente de elementos selecionados pelo usuário.
## 🐱 Animação Extra: Gatinho Animado

O projeto inclui uma animação criativa de um gatinho, que responde a diferentes transformações (translação, rotação, escala, reflexão, cisalhamento) e possui efeitos visuais como piscadas, movimento de cauda e orelhas.

### ✨ Funções Principais

#### Desenho do Gatinho

```c
void drawCat()
{
    glPushMatrix();
    int *window = windowSize();
    if (catX == 0 && catY == 0)
    {
        catX = window[0] - 70;
        catY = window[1] - 70;
    }
    glTranslatef(catX, catY, 1.0f);
    glScalef(1.0f, 1.0f, 1.0f);

    // Transformações conforme o estado do gato
    if (catState == CAT_ROTATE)
        glRotatef(catAngle, 0.0f, 0.0f, 1.0f);

    if (catState == CAT_SCALE)
        glScalef(catScaleFactor, catScaleFactor, 1.0f);

    if (catState == CAT_MIRROR)
        glScalef(mirrorDirection == 0 ? -1.0f : 1.0f, mirrorDirection == 1 ? -1.0f : 1.0f, 1.0f);

    if (catState == CAT_SHEAR)
    {
        float m[16] = {
            1.0f, shearFactorY * 0.2f, 0.0f, 0.0f,
            shearFactorX * 0.2f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
        glMultMatrixf(m);
    }

    // Corpo do gato (círculo)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++)
    {
        float angle = 2.0f * PI * i / 20;
        glVertex2f(catSize * cosf(angle), catSize * sinf(angle));
    }
    glEnd();

    // Focinho (círculo menor)
    glColor3f(1.0f, 0.8f, 0.8f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++)
    {
        float angle = 2.0f * PI * i / 20;
        glVertex2f(catSize * 0.4f * cosf(angle), catSize * 0.4f * sinf(angle) - catSize * 0.1f);
    }
    glEnd();

    // Orelhas (triângulos)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_TRIANGLES);
    // Esquerda
    glVertex2f(-catSize * 0.5f, catSize * 0.5f);
    glVertex2f(-catSize * 0.7f - catEarWiggle, catSize * 0.9f + catEarWiggle);
    glVertex2f(-catSize * 0.3f, catSize * 0.9f);
    // Direita
    glVertex2f(catSize * 0.5f, catSize * 0.5f);
    glVertex2f(catSize * 0.3f, catSize * 0.9f);
    glVertex2f(catSize * 0.7f + catEarWiggle, catSize * 0.9f + catEarWiggle);
    glEnd();

    // Olhos (abertos ou piscando)
    if (catIsBlinking)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-catSize * 0.3f, catSize * 0.2f);
        glVertex2f(-catSize * 0.1f, catSize * 0.2f);
        glVertex2f(catSize * 0.1f, catSize * 0.2f);
        glVertex2f(catSize * 0.3f, catSize * 0.2f);
        glEnd();
    }
    else
    {
        // Olhos abertos (círculos azuis e pupilas pretas)
        glColor3f(0.2f, 0.6f, 0.8f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; i++)
        {
            float angle = 2.0f * PI * i / 20;
            glVertex2f(-catSize * 0.2f + catSize * 0.1f * cosf(angle),
                       catSize * 0.2f + catSize * 0.1f * sinf(angle));
        }
        glEnd();
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; i++)
        {
            float angle = 2.0f * PI * i / 20;
            glVertex2f(catSize * 0.2f + catSize * 0.1f * cosf(angle),
                       catSize * 0.2f + catSize * 0.1f * sinf(angle));
        }
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; i++)
        {
            float angle = 2.0f * PI * i / 20;
            glVertex2f(-catSize * 0.2f + catSize * 0.05f * cosf(angle),
                       catSize * 0.2f + catSize * 0.05f * sinf(angle));
        }
        glEnd();
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; i++)
        {
            float angle = 2.0f * PI * i / 20;
            glVertex2f(catSize * 0.2f + catSize * 0.05f * cosf(angle),
                       catSize * 0.2f + catSize * 0.05f * sinf(angle));
        }
        glEnd();
    }

    // Nariz
    glColor3f(0.9f, 0.5f, 0.5f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-catSize * 0.05f, catSize * 0.0f);
    glVertex2f(catSize * 0.05f, catSize * 0.0f);
    glVertex2f(0.0f, catSize * -0.05f);
    glEnd();

    // Bigodes
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Esquerda
    glVertex2f(-catSize * 0.2f, 0.0f); glVertex2f(-catSize * 0.6f, catSize * 0.1f);
    glVertex2f(-catSize * 0.2f, -catSize * 0.05f); glVertex2f(-catSize * 0.6f, -catSize * 0.05f);
    glVertex2f(-catSize * 0.2f, -catSize * 0.1f); glVertex2f(-catSize * 0.6f, -catSize * 0.2f);
    // Direita
    glVertex2f(catSize * 0.2f, 0.0f); glVertex2f(catSize * 0.6f, catSize * 0.1f);
    glVertex2f(catSize * 0.2f, -catSize * 0.05f); glVertex2f(catSize * 0.6f, -catSize * 0.05f);
    glVertex2f(catSize * 0.2f, -catSize * 0.1f); glVertex2f(catSize * 0.6f, -catSize * 0.2f);
    glEnd();

    // Rabo (linha animada)
    glLineWidth(2.0f);
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++)
    {
        float t = (float)i / 10.0f;
        float x = -catSize + t * catSize * 0.4f;
        float y = -catSize * 0.7f + sinf(t * PI * 2 + catTailAngle * PI / 180.0f) * catSize * 0.3f;
        glVertex2f(x, y);
    }
    glEnd();

    // Boca (diferente para cada estado)
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    if (catState == CAT_IDLE)
    {
        for (int i = 0; i <= 10; i++)
        {
            float t = (float)i / 10.0f;
            float x = (t - 0.5f) * catSize * 0.4f;
            float y = -catSize * 0.15f - sinf(t * PI) * catSize * 0.03f;
            glVertex2f(x, y);
        }
    }
    else if (catState == CAT_TRANSLATE)
    {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; i++)
        {
            float angle = PI + PI * i / 20;
            if (angle > PI && angle < 2 * PI)
                glVertex2f(catSize * 0.1f * cosf(angle),
                           -catSize * 0.15f + catSize * 0.1f * sinf(angle));
        }
    }
    else if (catState == CAT_ROTATE)
    {
        for (int i = 0; i <= 20; i++)
        {
            float angle = PI + PI * i / 20;
            float x = catSize * 0.1f * cosf(angle);
            float y = -catSize * 0.15f + catSize * 0.05f * sinf(angle);
            glVertex2f(x, y);
        }
    }
    else if (catState == CAT_SCALE)
    {
        for (int i = 0; i <= 10; i++)
        {
            float t = (float)i / 10.0f;
            float x = (t - 0.5f) * catSize * 0.5f;
            float y = -catSize * 0.15f - sinf(t * PI) * catSize * 0.1f;
            glVertex2f(x, y);
        }
    }
    else if (catState == CAT_MIRROR)
    {
        for (int i = 0; i <= 10; i++)
        {
            float t = (float)i / 10.0f;
            float x = (t - 0.5f) * catSize * 0.4f;
            float y = -catSize * 0.15f + cosf((t - 0.5f) * PI * 2) * catSize * 0.03f;
            glVertex2f(x, y);
        }
    }
    else if (catState == CAT_SHEAR)
    {
        for (int i = 0; i <= 10; i++)
        {
            float t = (float)i / 10.0f;
            float x = (t - 0.5f) * catSize * 0.4f;
            float y = -catSize * 0.15f + sinf(t * PI * 3) * catSize * 0.03f;
            glVertex2f(x, y);
        }
    }
    glEnd();

    glPopMatrix();
}
```

#### Atualização da Animação

```c
void updateCatAnimation()
{
    clock_t currentTime = clock();
    float deltaTime = (float)(currentTime - lastFrameTime) / CLOCKS_PER_SEC;
    lastFrameTime = currentTime;

    catAnimationTime += deltaTime;
    int *window = windowSize();

    switch (catState)
    {
    case CAT_IDLE:
        catScaleFactor = 1.0f + sinf(catAnimationTime * 2.0f) * 0.02f;
        catEarWiggle = sinf(catAnimationTime * 3.0f) * 2.0f;
        catTailAngle = 30.0f + sinf(catAnimationTime) * 10.0f;
        break;
    case CAT_TRANSLATE:
        catY = (float)(window[1] - 70) + sinf(catAnimationTime * 8.0f) * 10.0f;
        catEarWiggle = sinf(catAnimationTime * 8.0f) * 5.0f;
        catTailAngle = 60.0f + sinf(catAnimationTime * 5.0f) * 20.0f;
        break;
    case CAT_ROTATE:
        catAngle = sinf(catAnimationTime * 2.0f) * 15.0f;
        catTailAngle = 30.0f + sinf(catAnimationTime * 3.0f) * 40.0f;
        break;
    case CAT_SCALE:
        catScaleFactor = 1.0f + sinf(catAnimationTime * 5.0f) * 0.2f;
        catTailAngle = 50.0f + cosf(catAnimationTime * 8.0f) * 30.0f;
        break;
    case CAT_MIRROR:
        catEarWiggle = sinf(catAnimationTime * 10.0f) * 3.0f;
        catTailAngle = 90.0f + cosf(catAnimationTime * 12.0f) * 30.0f;
        break;
    case CAT_SHEAR:
        catEarWiggle = sinf(catAnimationTime * 6.0f) * 8.0f;
        catTailAngle = 120.0f + sinf(catAnimationTime * 10.0f) * 40.0f;
        break;
    }

    // Piscar aleatório
    catBlinkCounter++;
    if (catBlinkCounter > 100)
    {
        int shouldBlink = rand() % 10;
        if (shouldBlink == 0)
        {
            catIsBlinking = 1;
            catBlinkCounter = 0;
        }
    }
    else if (catBlinkCounter > 5 && catIsBlinking)
    {
        catIsBlinking = 0;
    }

    glutPostRedisplay();
}
```

#### Controle de Estado da Animação

```c
void setCatState(CatAnimationState state)
{
    catState = state;
    catAnimationTime = 0.0f;
    int *window = windowSize();

    switch (state)
    {
    case CAT_IDLE:
        catScaleFactor = 1.0f;
        catAngle = 0.0f;
        break;
    case CAT_TRANSLATE:
        catY = window[1] - 70;
        break;
    case CAT_ROTATE:
        catAngle = 15.0f;
        break;
    case CAT_SCALE:
        catScaleFactor = 1.2f;
        break;
    case CAT_MIRROR:
        catEarWiggle = 5.0f;
        break;
    case CAT_SHEAR:
        catTailAngle = 90.0f;
        break;
    }
}
```

#### Função Idle para Atualização Contínua

```c
void idleFunc()
{
    updateCatAnimation();
}
```

## 🎹 Atalhos de Teclado

Abaixo estão os principais atalhos de teclado para utilizar o editor gráfico:

| Tecla         | Função                                                                 |
|---------------|------------------------------------------------------------------------|
| **v**         | Modo vértice (desenhar pontos)                                         |
| **l**         | Modo linha (desenhar segmentos de reta)                                |
| **p**         | Modo polígono (desenhar polígonos)                                     |
| **s**         | Modo seleção                                                           |
| **t**         | Ativar translação (após selecionar um objeto)                          |
| **r**         | Ativar rotação (após selecionar um objeto)                             |
| **e**         | Ativar escala (após selecionar um objeto)                              |
| **m**         | Ativar reflexão (após selecionar um objeto)                            |
| **c**         | Ativar cisalhamento (após selecionar um objeto)                        |
| **x**         | Reflexão horizontal (quando em modo reflexão)                          |
| **y**         | Reflexão vertical (quando em modo reflexão)                            |
| **a / d**     | Rotacionar objeto selecionado (anti-horário/horário)                   |
| **+ / -**     | Aumentar/diminuir escala do objeto selecionado                         |
| **D**         | Deletar objeto selecionado                                             |
| **S**         | Salvar objetos e screenshot                                            |
| **L**         | Carregar objetos do arquivo                                            |
| **h**         | Exibir ajuda no terminal                                               |
| **ESC**       | Sair do programa                                                       |

Outras operações:
- **Botão do meio do mouse**: Alternar entre transformações
- **Botão direito do mouse**: Cancelar seleção ou fechar polígono

---

---

## 🎮 Função de Desenho do Ícone do Joystick

Abaixo está a função responsável por desenhar um ícone de joystick na tela, indicando visualmente o modo de seleção ou outro estado do editor:

```c
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

    if (currentMode == SELECTION)
    {
        glColor3f(0.4, 0.0, 1.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i++)
        {
            float angle = i * PI / 180.0;
            float dx = (radius + 2) * cos(angle);
            float dy = (radius + 2) * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();

        glColor3f(1.0, 0.0, 0.0);
        glPointSize(5.0);
        glBegin(GL_POINTS);
        for (int i = 0; i < 360; i += 30)
        {
            float angle = i * PI / 180.0;
            float dx = radius * cos(angle);
            float dy = radius * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
    }
    else
    {
        glColor3f(0.0, 0.8, 0.0);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i++)
        {
            float angle = i * PI / 180.0;
            float dx = radius * cos(angle);
            float dy = radius * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
    }
}
```

**Descrição:**
- Desenha um círculo externo cinza como base do ícone.
- Se o modo atual for `SELECTION`, destaca o círculo com uma borda roxa e pontos vermelhos ao redor, indicando seleção ativa.
- Caso contrário, preenche o círculo de verde, indicando outro estado.

Utilize essa função para exibir o status do joystick ou modo de seleção no seu editor gráfico.


## 🎮 Função de Manipulação do Joystick

Abaixo está a função responsável por tratar os eventos do joystick, permitindo desenhar, selecionar e transformar objetos no editor gráfico:

```c
void joystick(unsigned int buttons, int x, int y, int z)
{
    int previousButtonMask = buttonMask;
    buttonMask = buttons;

    static int lastJoystickX = 0, lastJoystickY = 0;
    static int lastMirrorDirection = -1;
    int *window = windowSize();
    int moveX = x / 80;
    int moveY = -y / 80;

    int widthScreen = window[0], heightScreen = window[1];

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

    float dx = joystickX - lastJoystickX;
    float dy = joystickY - lastJoystickY;
    lastJoystickX = joystickX;
    lastJoystickY = joystickY;

    printf("dx: %2f dy: %2f | joystickX: %d joystickY: %d | lastX: %d lastY: %d , button: %d\n",
           dx, dy, joystickX, joystickY, lastJoystickX, lastJoystickY, buttons);

    if (isSelected && currentTransform == TRANSLATE)
    {
        if (IdSelectedPoint >= 0)
            translatePoint(IdSelectedPoint, dx, dy, points);
        else if (IdSelectedLine >= 0)
            translateLine(IdSelectedLine, dx, dy, lines);
        else if (IdSelectedPolygon >= 0)
            translatePolygon(IdSelectedPolygon, dx, dy, meshes);
    }
    else if (isSelected && currentTransform == SCALE)
    {
        float scaleFactor = 1.0f;

        if (dx > 0 || dy > 0)
            scaleFactor = 1.04f;
        else if (dx < 0 || dy < 0)
            scaleFactor = 0.97f;

        scaleFactorX *= scaleFactor;
        scaleFactorY *= scaleFactor;

        if (IdSelectedLine >= 0)
            scaleLine(IdSelectedLine, scaleFactor, scaleFactor, lines);
        else if (IdSelectedPolygon >= 0)
            scalePolygon(IdSelectedPolygon, scaleFactor, scaleFactor, meshes);
    }
    else if (isSelected && currentTransform == ROTATE)
    {
        rotationAngle -= dx / 2;

        if (IdSelectedPoint >= 0)
        {
            rotatePoint(IdSelectedPoint, -dx / 2, points);
        }
        else if (IdSelectedLine >= 0)
        {
            rotateLine(IdSelectedLine, -dx / 2, lines);
        }
        else if (IdSelectedPolygon >= 0)
        {
            rotatePolygon(IdSelectedPolygon, -dx / 2, meshes);
        }
    }
    else if (isSelected && currentTransform == MIRROR && (dx != 0 || dy != 0))
    {
        int newMirrorDirection = (dx != 0) ? 0 : 1;
        if (newMirrorDirection != mirrorDirection)
        {
            mirrorDirection = newMirrorDirection;
            if (IdSelectedPoint >= 0)
                mirrorPoint(IdSelectedPoint, mirrorDirection, points);
            else if (IdSelectedLine >= 0)
                mirrorLine(IdSelectedLine, mirrorDirection, lines);
            else if (IdSelectedPolygon >= 0)
                mirrorPolygon(IdSelectedPolygon, mirrorDirection, meshes);
        }
    }
    else if (isSelected && currentTransform == SHEAR)
    {
        float shearX = 0.0f, shearY = 0.0f;

        if (dx != 0)
            shearX = (dx > 0) ? 0.05f : -0.05f;
        if (dy != 0)
            shearY = (dy > 0) ? 0.05f : -0.05f;

        shearFactorX += shearX;
        shearFactorY += shearY;

        if (IdSelectedPoint >= 0)
            shearPoint(IdSelectedPoint, shearX, shearY, points);
        else if (IdSelectedLine >= 0)
            shearLine(IdSelectedLine, shearX, shearY, lines);
        else if (IdSelectedPolygon >= 0)
            shearPolygon(IdSelectedPolygon, shearX, shearY, meshes);
    }

    switch (buttons)
    {
    case JOYSTICK_DOWN:
        if ((currentMode == LINE) && isDrawing && joystickActive)
        {
            tempLine.end = (Point){joystickX, joystickY};
            lines[lineCount++] = tempLine;
            isDrawing = 0;
        }
        break;

    case JOYSTICK_X: // X
        if (buttonMask != previousButtonMask)
        {
            showMessage = LOAD_MESH;
            loadObjectsFromFile("objetos.txt", &pointCount, &lineCount, &meshCount, &points, &lines, &meshes, &tempMesh);
            glutTimerFunc(2000, hideMessage, 0);
        }
        break;

    case JOYSTICK_CIRCLE: // O
        if (buttonMask != previousButtonMask)
        {
            showMessage = DELETED;
            deleteSelectedObject();
            glutTimerFunc(2000, hideMessage, 0);
        }
        break;

    case JOYSTICK_QUAD: // Quadrado
        if (currentMode == VERTICE && buttonMask != previousButtonMask)
        {
            points[pointCount++] = (Point){joystickX, joystickY};
        }
        else if (currentMode == LINE && !isDrawing && buttonMask != previousButtonMask)
        {
            tempLine.init = (Point){joystickX, joystickY};
            isDrawing = 1;
        }
        else if (currentMode == LINE && buttons == JOYSTICK_QUAD)
        {
            tempLine.end = (Point){joystickX, joystickY};
        }
        else if (currentMode == POLYGON)
        {
            if (!isDrawingPolygon)
            {
                isDrawingPolygon = 1;
                tempMesh.numberPoints = 0;
            }
            if (tempMesh.numberPoints < MAX_POLYGON_POINTS && buttonMask != previousButtonMask)
            {
                tempMesh.vertices[tempMesh.numberPoints++] = (Point){joystickX, joystickY};
            }
        }
        else if (currentMode == SELECTION && !isDrawing && buttonMask != previousButtonMask)
        {
            IdSelectedPoint = -1;
            IdSelectedLine = -1;
            IdSelectedPolygon = -1;
            isSelected = 0;
            IdSelectedPoint = selectPoint(joystickX, joystickY, pointCount, points);

            if (IdSelectedPoint >= 0)
            {
                isSelected = 1;
            }
            else
            {
                IdSelectedLine = selectLine(joystickX, joystickY, lineCount, lines);

                if (IdSelectedLine >= 0)
                {
                    isSelected = 1;
                    printf("Linha selecionada: ID %d\n", IdSelectedLine);
                }
                else
                {
                    IdSelectedPolygon = selectPolygon(joystickX, joystickY, meshCount, meshes);

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
            saveObjectsToFile("objetos.txt", pointCount, lineCount, meshCount, points, lines, meshes);
            showMessage = SAVE_MESH;
            saveScreenshotBMP("projeto.bmp", widthScreen, heightScreen);
            glutTimerFunc(2000, hideMessage, 0);
        }
        break;

    case JOYSTICK_L1: // L1
        if (buttonMask != previousButtonMask)
        {
            (currentMode == SELECTION) ? currentMode = VERTICE : currentMode++;
            if (currentMode == POLYGON)
            {
                showMessage = POLYLGON_LOG;
                isDrawingPolygon = 1;
                tempMesh.numberPoints = 0;
                glutTimerFunc(2000, hideMessage, 0);
            }
            else if (currentMode == VERTICE)
            {
                showMessage = VERTCIE_LOG;
                isDrawingPolygon = 0;
                glutTimerFunc(2000, hideMessage, 0);
            }
            else if (currentMode == LINE)
            {
                showMessage = LINE_LOG;
                isDrawingPolygon = 0;
                glutTimerFunc(2000, hideMessage, 0);
            }
        }
        break;

    case JOYSTICK_R1: // R1
        if (buttonMask != previousButtonMask)
        {
            if (currentTransform == SHEAR)
                currentTransform = NONE_TRANSFORMER;
            else
                currentTransform++;
            setCatState(currentTransform);
        }
        break;

    case JOYSTICK_L2:
        break; // L2

    case JOYSTICK_R2:
        if (currentMode == POLYGON && isDrawingPolygon)
        {
            meshes[meshCount++] = tempMesh;
            tempMesh.numberPoints = 0;
            isDrawingPolygon = 0;
            printf("Poligono fechado e armazenado.\n");
        }
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
                setCatState(CAT_IDLE);
                IdSelectedPoint = -1;
                IdSelectedLine = -1;
                IdSelectedPolygon = -1;
                isSelected = 0;
                currentTransform = NONE_TRANSFORMER;
                printf("Selecao removida.\n");
            }
        }
        break; // L3

    case JOYSTICK_R3:
        if (currentMode == POLYGON && isDrawingPolygon)
        {
            meshes[meshCount++] = tempMesh;
            tempMesh.numberPoints = 0;
            isDrawingPolygon = 0;
        }
        break; // R3

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
    glutPostRedisplay();
}
```

**Descrição:**
- Permite desenhar pontos, linhas e polígonos usando o joystick.
- Suporta seleção e transformação (translação, rotação, escala, reflexão, cisalhamento) de objetos.
- Permite salvar/carregar objetos, deletar, alternar modos e manipular animações.
- Os botões do joystick são mapeados para diferentes ações no editor gráfico.

Utilize essa função para integrar o controle via joystick ao seu editor de gráficos em OpenGL.
## 🖱️ Funções de Manipulação de Janela e Mouse

### Função `reshape`

Responsável por ajustar o viewport e a projeção quando a janela é redimensionada.

```c
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
```

---

### Função `mouse`

Gerencia os eventos do mouse para desenhar, selecionar e transformar objetos.

```c
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
                IdSelectedPoint = selectPoint(normX, normY, pointCount, points);

                if (IdSelectedPoint >= 0)
                {
                    isSelected = 1;
                    printf("Ponto selecionado: ID %d\n", IdSelectedPoint);
                }
                else
                {
                    IdSelectedLine = selectLine(normX, normY, lineCount, lines);

                    if (IdSelectedLine >= 0)
                    {
                        isSelected = 1;
                        printf("Linha selecionada: ID %d\n", IdSelectedLine);
                    }
                    else
                    {
                        IdSelectedPolygon = selectPolygon(normX, normY, meshCount, meshes);

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
            printf("Selecao removida.\n");
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
                scaleLine(IdSelectedLine, 1.1f, 1.1f, lines);
            }
            else if (IdSelectedPolygon >= 0)
            {
                scalePolygon(IdSelectedPolygon, 1.1f, 1.1f, meshes);
            }
            break;
        case ROTATE:
            rotationAngle += 5.0f;

            if (IdSelectedPoint >= 0)
            {
                rotatePoint(IdSelectedPoint, 5.0f, points);
            }
            else if (IdSelectedLine >= 0)
            {
                rotateLine(IdSelectedLine, 5.0f, lines);
            }
            else if (IdSelectedPolygon >= 0)
            {
                rotatePolygon(IdSelectedPolygon, 5.0f, meshes);
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
                scaleLine(IdSelectedLine, 0.9f, 0.9f, lines);
            }
            else if (IdSelectedPolygon >= 0)
            {
                scalePolygon(IdSelectedPolygon, 0.9f, 0.9f, meshes);
            }
            break;
        case ROTATE:
            rotationAngle -= 5.0f;

            if (IdSelectedPoint >= 0)
            {
                rotatePoint(IdSelectedPoint, -5.0f, points);
            }
            else if (IdSelectedLine >= 0)
            {
                rotateLine(IdSelectedLine, -5.0f, lines);
            }
            else if (IdSelectedPolygon >= 0)
            {
                rotatePolygon(IdSelectedPolygon, -5.0f, meshes);
            }
            break;
        }
    }

    glutPostRedisplay();
}
```

---

Essas funções permitem manipular a janela e interagir com os objetos gráficos usando o mouse, incluindo criação, seleção e transformação de pontos, linhas e polígonos.

## 📸 Função para Captura de Tela (Screenshot BMP)

Abaixo está a função responsável por capturar a tela do editor gráfico e salvar como uma imagem no formato BMP. Ela utiliza OpenGL para ler os pixels da janela e grava o arquivo no disco.

```c
void saveScreenshotBMP(const char *filename, int width, int height)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }

    BMPHeader bmpHeader = {{'B', 'M'}, 54 + 3 * width * height, 0, 0, 54};
    BMPInfoHeader bmpInfoHeader = {
        40, width, -height, 1, 24, 0, 3 * width * height, 2835, 2835, 0, 0};

    fwrite(&bmpHeader, sizeof(BMPHeader), 1, file);
    fwrite(&bmpInfoHeader, sizeof(BMPInfoHeader), 1, file);

    GLubyte *pixels = (GLubyte *)malloc(3 * width * height);
    if (!pixels)
    {
        printf("Erro ao alocar memoria.\n");
        fclose(file);
        return;
    }

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    fwrite(pixels, 3, width * height, file);

    free(pixels);
    fclose(file);
    printf("Captura de tela salva como %s\n", filename);
}
```

**Descrição:**
- Abre um arquivo BMP para escrita binária.
- Preenche os cabeçalhos BMP (`BMPHeader` e `BMPInfoHeader`).
- Lê os pixels da tela usando `glReadPixels`.
- Escreve os dados de imagem no arquivo.
- Libera a memória e fecha o arquivo.

Utilize esta função para salvar uma captura do seu trabalho no editor gráfico. O arquivo será salvo no caminho e nome especificados em `filename`.



## 🖱️ Funções de Manipulação de Movimento e Teclas Especiais

A seguir estão as funções responsáveis por manipular o movimento do mouse (`motion`) e as teclas especiais do teclado (`specialKeys`). Elas permitem desenhar, mover e transformar objetos interativamente no editor gráfico.

### Função `motion`

Esta função é chamada quando o mouse é movido com um botão pressionado. Ela atualiza a posição de linhas em desenho, vértices de polígonos ou move objetos selecionados durante a translação.

```c
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
                translatePoint(IdSelectedPoint, dx, dy, points);
            }
            else if (IdSelectedLine >= 0)
            {
                translateLine(IdSelectedLine, dx, dy, lines);
            }
            else if (IdSelectedPolygon >= 0)
            {
                translatePolygon(IdSelectedPolygon, dx, dy, meshes);
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
```

---

### Função `specialKeys`

Esta função trata as teclas especiais do teclado (setas) para mover ou cisalhar objetos selecionados. O comportamento depende do modo de transformação ativo e se a tecla Shift está pressionada.

```c
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
                    translatePoint(IdSelectedPoint, 0, step, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, 0, step, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, 0, step, meshes);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorY += 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, 0, 0.1f, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, 0, 0.1f, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, 0, 0.1f, meshes);
                }
            }
            break;

        case GLUT_KEY_DOWN:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, 0, -step, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, 0, -step, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, 0, -step, meshes);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorY -= 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, 0, -0.1f, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, 0, -0.1f, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, 0, -0.1f, meshes);
                }
            }
            break;

        case GLUT_KEY_LEFT:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, -step, 0, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, -step, 0, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, -step, 0, meshes);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorX -= 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, -0.1f, 0, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, -0.1f, 0, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, -0.1f, 0, meshes);
                }
            }
            break;

        case GLUT_KEY_RIGHT:
            if (currentTransform == TRANSLATE)
            {
                if (IdSelectedPoint >= 0)
                {
                    translatePoint(IdSelectedPoint, step, 0, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    translateLine(IdSelectedLine, step, 0, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    translatePolygon(IdSelectedPolygon, step, 0, meshes);
                }
            }
            else if (currentTransform == SHEAR && shiftPressed)
            {
                shearFactorX += 0.1f;

                if (IdSelectedPoint >= 0)
                {
                    shearPoint(IdSelectedPoint, 0.1f, 0, points);
                }
                else if (IdSelectedLine >= 0)
                {
                    shearLine(IdSelectedLine, 0.1f, 0, lines);
                }
                else if (IdSelectedPolygon >= 0)
                {
                    shearPolygon(IdSelectedPolygon, 0.1f, 0, meshes);
                }
            }
            break;
        }
    }
    glutPostRedisplay();
}
```

---

Essas funções proporcionam uma experiência interativa para manipulação dos objetos gráficos, permitindo desenhar, mover e transformar elementos usando o mouse e o teclado.



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

