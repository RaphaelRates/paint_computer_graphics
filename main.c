//=================================================== IMPORTAÇÕES ======================================================
/**
 * Aqui estão presentes todas as importações necessárias para o
 * funcionamento do proejto PAINT AIMULATOR, possuindo alguns arquivos como:
 *
 * glut.h            -  para usar o OpenGL
 * stdio.h           -  entrada de dados como prinft
 * stdlib.h          -  gerenciamento da memória
 * math.h            -  funções matemáricas
 * data.h            -  structs e cabeçalhos das funções padrão usadas no projeto
 * arqs.c            -  funções carregamento e salvamento do desenho
 * draw.c            -  funções de desenhos de linha, ponto e poligonos, alem dos previews dos desenhos
 * tranformers.c     -  funções para tranformações de Escala, Tranlação, ROtação, Cislhamento e Reflexão 
 *                      para os elementos de desenho (pontos, linhas e poligonos).
 * calculate.c       -  Funções de calculos feitas para o projeto
 * time.h            -  biblioteca para gerir o tempo de execução
 * 
 * 
 * AUTORES DO PROJETO
 * 
 * @authors
 * Raphael Rates
 * José Denis
 * Gabriela Queiroga
 * 
 *
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <data.h>
#include <arqs.c>
#include <draw.c>
#include <transformers.c>
#include <calculate.c>
#include <time.h>

/**
 * @struct BMPHeader
 * @brief Estrutura para armazenar o cabeçalho de arquivos BMP.
 */
#pragma pack(push, 1)
typedef struct
{
    unsigned char bfType[2];   /**< @var Tipo do arquivo ("BM" para Bitmap). */
    unsigned int bfSize;       /**< @var Tamanho total do arquivo em bytes. */
    unsigned short bfReserved1; /**< @var Reservado (deve ser 0). */
    unsigned short bfReserved2; /**< @var Reservado (deve ser 0). */
    unsigned int bfOffBits;    /**< @var Offset onde os dados de imagem começam. */
} BMPHeader;

/**
 * @struct BMPInfoHeader
 * @brief Estrutura para armazenar informações sobre a imagem BMP.
 */
typedef struct
{
    unsigned int biSize;         /**< @var Tamanho da estrutura. */
    int biWidth;                 /**< @var Largura da imagem em pixels. */
    int biHeight;                /**< @var Altura da imagem em pixels. */
    unsigned short biPlanes;     /**< @var Número de planos de cor (deve ser 1). */
    unsigned short biBitCount;   /**< @var Número de bits por pixel. */
    unsigned int biCompression;  /**< @var Tipo de compressão. */
    unsigned int biSizeImage;    /**< @var Tamanho da imagem em bytes. */
    int biXPelsPerMeter;         /**< @var Resolução horizontal (pixels por metro). */
    int biYPelsPerMeter;         /**< @var Resolução vertical (pixels por metro). */
    unsigned int biClrUsed;      /**< @var Número de cores na paleta. */
    unsigned int biClrImportant; /**< @var Número de cores importantes. */
} BMPInfoHeader;
#pragma pack(pop)

//==================================================== VARIÁVEIS ============================================================

/** @var Ponteiros para estruturas dinâmicas de pontos, linhas e malhas. */
Point *points = NULL;
Mesh *meshes = NULL;
Line *lines = NULL;

/** @var Estruturas temporárias para manipulação de malhas, pontos e linhas. */
Mesh tempMesh;
Point tempPoint;
Line tempLine;

/** @var Contadores para os elementos desenhados. */
int pointCount = 0, lineCount = 0, meshCount = 0;

/** @var Flags de controle de desenho. */
int isDrawing = 0, isDrawingPolygon = 0, isSelected;
MESSAGE showMessage = NONE_MESSAGE;

/** @var Controle de joystick e mouse. */
int joystickActive = 0;
int joystickButton = 0;
int buttonMask = 0;
int joystickX = 400, joystickY = 400;
int lastMouseX = -1, lastMouseY = -1;
int MousseActvive = 1; /**< @note "MouseActive" está escrito errado. O correto seria "MouseActive". */

/** @var IDs de seleção de elementos gráficos. */
int IdSelectedLine = -1;
int IdSelectedPoint = -1;
int IdSelectedPolygon = -1;

/** @var Flags para transformações geométricas. */
int isTranslating = 0;
int isRotating = 0;
int isScaling = 0;
int isMirroring = 0;
int isShearing = 0;

/** @var Ponto de referência para transformações. */
Point referencePoint;
Point lastMousePos;

/** @var Parâmetros para transformações geométricas. */
float rotationAngle = 0.0f;
float scaleFactorX = 1.0f;
float scaleFactorY = 1.0f;
int mirrorDirection = 0;
float shearFactorX = 0.0f;
float shearFactorY = 0.0f;

/** @var Modos de transformação e desenho. */
TransformMode currentTransform = NONE_TRANSFORMER;
Mode currentMode = NONE_MESH;
int joystickMode = 0;
float colorLoading_r = 0.1f, colorLoading_g = 0.5f, colorLoading_b = 0.3f;

/** @struct CatAnimationState
 *  @brief Estrutura para controlar o estado de animação do gato.
 */
CatAnimationState catState = CAT_IDLE;
float catX = 0.0f;
float catY = 0.0f;
float catSize = 30.0f;
float catAngle = 0.0f;
float catScaleFactor = 1.0f;
float catAnimationTime = 0.0f;
clock_t lastFrameTime;
float catEarWiggle = 0.0f;
float catTailAngle = 30.0f;
int catBlinkCounter = 0;
int catIsBlinking = 0;

//============================================================ FUNÇÕES BASE =============================================================

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

    if (catState == CAT_ROTATE)
    {
        glRotatef(catAngle, 0.0f, 0.0f, 1.0f);
    }

    if (catState == CAT_SCALE)
    {
        glScalef(catScaleFactor, catScaleFactor, 1.0f);
    }

    if (catState == CAT_MIRROR)
    {
        if (mirrorDirection == 0)
        {
            glScalef(-1.0f, 1.0f, 1.0f);
        }
        else
        {
            glScalef(1.0f, -1.0f, 1.0f);
        }
    }

    if (catState == CAT_SHEAR)
    {
        float m[16] = {
            1.0f, shearFactorY * 0.2f, 0.0f, 0.0f,
            shearFactorX * 0.2f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
        glMultMatrixf(m);
    }

    // Desenhar o corpo do gatinho (c�rculo)
    glColor3f(0.8f, 0.8f, 0.8f); // Cinza claro
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++)
    {
        float angle = 2.0f * PI * i / 20;
        glVertex2f(catSize * cosf(angle), catSize * sinf(angle));
    }
    glEnd();

    // Desenhar o rosto
    glColor3f(1.0f, 0.8f, 0.8f); // Rosa claro para o focinho
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++)
    {
        float angle = 2.0f * PI * i / 20;
        glVertex2f(catSize * 0.4f * cosf(angle), catSize * 0.4f * sinf(angle) - catSize * 0.1f);
    }
    glEnd();

    // Orelhas (tri�ngulos)
    glColor3f(0.8f, 0.8f, 0.8f); // Cinza claro
    glBegin(GL_TRIANGLES);
    // Orelha esquerda
    glVertex2f(-catSize * 0.5f, catSize * 0.5f);
    glVertex2f(-catSize * 0.7f - catEarWiggle, catSize * 0.9f + catEarWiggle);
    glVertex2f(-catSize * 0.3f, catSize * 0.9f);

    // Orelha direita
    glVertex2f(catSize * 0.5f, catSize * 0.5f);
    glVertex2f(catSize * 0.3f, catSize * 0.9f);
    glVertex2f(catSize * 0.7f + catEarWiggle, catSize * 0.9f + catEarWiggle);
    glEnd();

    // Olhos (c�rculos)
    if (catIsBlinking)
    {
        // Olhos fechados (linhas)
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        // Olho esquerdo
        glVertex2f(-catSize * 0.3f, catSize * 0.2f);
        glVertex2f(-catSize * 0.1f, catSize * 0.2f);
        // Olho direito
        glVertex2f(catSize * 0.1f, catSize * 0.2f);
        glVertex2f(catSize * 0.3f, catSize * 0.2f);
        glEnd();
    }
    else
    {
        // Olhos abertos
        glColor3f(0.2f, 0.6f, 0.8f); // Azul para os olhos
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

        // Pupilas
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
    glColor3f(0.9f, 0.5f, 0.5f); // Rosa para o nariz
    glBegin(GL_TRIANGLES);
    glVertex2f(-catSize * 0.05f, catSize * 0.0f);
    glVertex2f(catSize * 0.05f, catSize * 0.0f);
    glVertex2f(0.0f, catSize * -0.05f);
    glEnd();

    // Bigodes (linhas)
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Bigodes esquerdos
    glVertex2f(-catSize * 0.2f, 0.0f);
    glVertex2f(-catSize * 0.6f, catSize * 0.1f);
    glVertex2f(-catSize * 0.2f, -catSize * 0.05f);
    glVertex2f(-catSize * 0.6f, -catSize * 0.05f);
    glVertex2f(-catSize * 0.2f, -catSize * 0.1f);
    glVertex2f(-catSize * 0.6f, -catSize * 0.2f);

    // Bigodes direitos
    glVertex2f(catSize * 0.2f, 0.0f);
    glVertex2f(catSize * 0.6f, catSize * 0.1f);
    glVertex2f(catSize * 0.2f, -catSize * 0.05f);
    glVertex2f(catSize * 0.6f, -catSize * 0.05f);
    glVertex2f(catSize * 0.2f, -catSize * 0.1f);
    glVertex2f(catSize * 0.6f, -catSize * 0.2f);
    glEnd();

    // Rabo
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

    // Boca
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    if (catState == CAT_IDLE)
    {
        // Sorriso suave
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
        // Boca aberta de surpresa
        glBegin(GL_POLYGON);
        for (int i = 0; i < 20; i++)
        {
            float angle = PI + PI * i / 20;
            if (angle > PI && angle < 2 * PI)
            {
                glVertex2f(catSize * 0.1f * cosf(angle),
                           -catSize * 0.15f + catSize * 0.1f * sinf(angle));
            }
        }
    }
    else if (catState == CAT_ROTATE)
    {
        // Boca em "o" de surpresa
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
        // Sorriso largo
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
        // Boca de confus�o
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
        // Boca de esfor�o
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

void updateCatAnimation()
{
    // Calcular delta de tempo
    clock_t currentTime = clock();
    float deltaTime = (float)(currentTime - lastFrameTime) / CLOCKS_PER_SEC;
    lastFrameTime = currentTime;

    catAnimationTime += deltaTime;
    int *window = windowSize();

    // Atualizar vari�veis de anima��o baseadas no estado
    switch (catState)
    {
    case CAT_IDLE:
        // Anima��o de respira��o suave
        catScaleFactor = 1.0f + sinf(catAnimationTime * 2.0f) * 0.02f;
        catEarWiggle = sinf(catAnimationTime * 3.0f) * 2.0f;
        catTailAngle = 30.0f + sinf(catAnimationTime) * 10.0f;
        break;

    case CAT_TRANSLATE:
        // Movimento saltitante
        catY = (float)(window[1] - 70) + sinf(catAnimationTime * 8.0f) * 10.0f;
        catEarWiggle = sinf(catAnimationTime * 8.0f) * 5.0f;
        catTailAngle = 60.0f + sinf(catAnimationTime * 5.0f) * 20.0f;
        break;

    case CAT_ROTATE:
        // Rota��o lenta
        catAngle = sinf(catAnimationTime * 2.0f) * 15.0f;
        catTailAngle = 30.0f + sinf(catAnimationTime * 3.0f) * 40.0f;
        break;

    case CAT_SCALE:
        // Pulsar
        catScaleFactor = 1.0f + sinf(catAnimationTime * 5.0f) * 0.2f;
        catTailAngle = 50.0f + cosf(catAnimationTime * 8.0f) * 30.0f;
        break;

    case CAT_MIRROR:
        // Efeito de confus�o
        catEarWiggle = sinf(catAnimationTime * 10.0f) * 3.0f;
        catTailAngle = 90.0f + cosf(catAnimationTime * 12.0f) * 30.0f;
        break;

    case CAT_SHEAR:
        // Efeito de distor��o
        catEarWiggle = sinf(catAnimationTime * 6.0f) * 8.0f;
        catTailAngle = 120.0f + sinf(catAnimationTime * 10.0f) * 40.0f;
        break;
    }

    // Piscar aleatoriamente
    catBlinkCounter++;
    if (catBlinkCounter > 100)
    {                                  // Aproximadamente a cada 3 segundos
        int shouldBlink = rand() % 10; // 10% de chance de piscar
        if (shouldBlink == 0)
        {
            catIsBlinking = 1;
            catBlinkCounter = 0;
        }
    }
    else if (catBlinkCounter > 5 && catIsBlinking)
    { // Dura��o do piscar
        catIsBlinking = 0;
    }

    glutPostRedisplay();
}

void setCatState(CatAnimationState state)
{
    catState = state;
    catAnimationTime = 0.0f;
    int *window = windowSize(); // Reinicia o tempo da anima��o

    // Efeitos de inicializa��o para diferentes estados
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

void idleFunc()
{
    updateCatAnimation();
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

void hideMessage(int value)
{
    showMessage = NONE_MESSAGE;
    glutPostRedisplay();
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
        setCatState(CAT_IDLE);
        glutTimerFunc(2000, hideMessage, 0);
        printf("Modo: Desenhar Poligono\n");
        break;
    case 'l':
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        joystickActive = 0;
        isDrawing = 1;
        currentMode = LINE;
        showMessage = LINE_LOG;
        setCatState(CAT_IDLE);
        glutTimerFunc(2000, hideMessage, 0);
        printf("Modo: Desenhar Linha\n");
        break;
    case 'v':
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        currentMode = VERTICE;
        showMessage = VERTCIE_LOG;
        setCatState(CAT_IDLE);
        glutTimerFunc(2000, hideMessage, 0);
        printf("Modo: Desenhar Vertice\n");
        break;
    case 's':
        glutSetCursor(GLUT_CURSOR_CYCLE);
        joystickActive = 0;
        currentMode = SELECTION;
        setCatState(CAT_IDLE);
        printf("Modo: Selecao\n");
        break;
    case 't':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_CYCLE);
            joystickActive = 0;
            setCatState(CAT_TRANSLATE);
            currentTransform = TRANSLATE;
            printf("Transformacao: Translacao\n");
        }
        break;
    case 'r':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_UP_DOWN);
            joystickActive = 0;
            setCatState(CAT_ROTATE);
            currentTransform = ROTATE;
            printf("Transformacao: Rotacao\n");
        }
        break;
    case 'e':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_SPRAY);
            joystickActive = 0;
            setCatState(CAT_SCALE);
            currentTransform = SCALE;
            printf("Transformacao: Escala\n");
        }
        break;
    case 'm':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            joystickActive = 0;
            setCatState(CAT_MIRROR);
            currentTransform = MIRROR;
            printf("Transformacao: Reflexao\n");
        }
        break;
    case 'c':
        if (isSelected)
        {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            joystickActive = 0;
            setCatState(CAT_SHEAR);
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
        setCatState(CAT_IDLE);

        break;
    case 'x':
        if (isSelected && currentTransform == MIRROR)
        {
            glutSetCursor(GLUT_CURSOR_WAIT);
            joystickActive = 0;
            mirrorDirection = 0;
            if (IdSelectedPoint >= 0)
                mirrorPoint(IdSelectedPoint, 0, points);
            else if (IdSelectedLine >= 0)
                mirrorLine(IdSelectedLine, 0, lines);
            else if (IdSelectedPolygon >= 0)
                mirrorPolygon(IdSelectedPolygon, 0, meshes);
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
                mirrorPoint(IdSelectedPoint, 1, points);
            else if (IdSelectedLine >= 0)
                mirrorLine(IdSelectedLine, 1, lines);
            else if (IdSelectedPolygon >= 0)
                mirrorPolygon(IdSelectedPolygon, 1, meshes);
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
                rotatePoint(IdSelectedPoint, -5.0f, points);
            else if (IdSelectedLine >= 0)
                rotateLine(IdSelectedLine, -5.0f, lines);
            else if (IdSelectedPolygon >= 0)
                rotatePolygon(IdSelectedPolygon, -5.0f, meshes);
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
                rotatePoint(IdSelectedPoint, 5.0f, points);
            else if (IdSelectedLine >= 0)
                rotateLine(IdSelectedLine, 5.0f, lines);
            else if (IdSelectedPolygon >= 0)
                rotatePolygon(IdSelectedPolygon, 5.0f, meshes);
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
                scaleLine(IdSelectedLine, 1.1f, 1.1f, lines);
            else if (IdSelectedPolygon >= 0)
                scalePolygon(IdSelectedPolygon, 1.1f, 1.1f, meshes);
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
                scaleLine(IdSelectedLine, 0.9f, 0.9f, lines);
            else if (IdSelectedPolygon >= 0)
                scalePolygon(IdSelectedPolygon, 0.9f, 0.9f, meshes);
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        break;
    case 'D':
        glutSetCursor(GLUT_CURSOR_WAIT);
        joystickActive = 0;
        showMessage = DELETED;
        setCatState(CAT_IDLE);
        deleteSelectedObject();
        glutTimerFunc(2000, hideMessage, 0);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 'S':
        glutSetCursor(GLUT_CURSOR_WAIT);
        joystickActive = 0;
        setCatState(CAT_IDLE);
        saveObjectsToFile("objetos.txt", pointCount, lineCount, meshCount, points, lines, meshes);
        saveScreenshotBMP("projeto.bmp", window[0], window[1]);
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
        setCatState(CAT_IDLE);
        loadObjectsFromFile("objetos.txt", &pointCount, &lineCount, &meshCount, &points, &lines, &meshes, &tempMesh);
        glutTimerFunc(2000, hideMessage, 0);
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

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
    drawPreviewPoint(currentMode, isDrawing, tempPoint);
    drawPreviewLine(currentMode, isDrawing, tempPoint, colorLoading_r, colorLoading_g, colorLoading_b, tempLine);
    drawPreviewPolygon(isDrawingPolygon, colorLoading_r, colorLoading_g, colorLoading_b, tempMesh);
    drawPoint(points, pointCount, isSelected, IdSelectedPoint);

    drawLines(lines, lineCount, IdSelectedLine, isSelected);
    drawPolygon(meshes, meshCount, IdSelectedPolygon, isSelected);
    drawSelectedObject(isSelected, IdSelectedPoint, IdSelectedLine, IdSelectedPolygon, points, lines, meshes);
    drawTransformInfo(isSelected, IdSelectedPoint, IdSelectedLine, IdSelectedPolygon, currentTransform, rotationAngle, scaleFactorX, scaleFactorY, shearFactorX, shearFactorY);
    drawMessage(showMessage);

    drawCat();

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
    loadObjectsFromFile("objetos.txt", &pointCount, &lineCount, &meshCount, &points, &lines, &meshes, &tempMesh);
    glutJoystickFunc(joystick, 10);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idleFunc);
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