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

---


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

