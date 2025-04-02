#ifndef TRANSFORMERS_H
#define TRANSFORMERS_H

#include "data.h"
#include "arqs.h"

/**
 * @brief Desenha informações sobre a transformação aplicada.
 * @param isSelected Indica se há um objeto selecionado.
 * @param IdSelectedPoint ID do ponto selecionado.
 * @param IdSelectedLine ID da linha selecionada.
 * @param IdSelectedPolygon ID do polígono selecionado.
 * @param currentTransform Modo de transformação atual.
 * @param rotationAngle Ângulo de rotação.
 * @param scaleFactorX Fator de escala no eixo X.
 * @param scaleFactorY Fator de escala no eixo Y.
 * @param shearFactorX Fator de cisalhamento no eixo X.
 * @param shearFactorY Fator de cisalhamento no eixo Y.
 */
void drawTransformInfo(int isSelected, int IdSelectedPoint, int IdSelectedLine, int IdSelectedPolygon, TransformMode currentTransform, float rotationAngle, float scaleFactorX, float scaleFactorY, float shearFactorX, float shearFactorY);

/**
 * @brief Calcula a distância entre dois pontos.
 * @param p1 Primeiro ponto.
 * @param p2 Segundo ponto.
 * @return A distância entre os dois pontos.
 */
float distance(Point p1, Point p2);

/**
 * @brief Calcula a distância de um ponto a uma linha.
 * @param p Ponto de referência.
 * @param l Linha para cálculo da distância.
 * @return A menor distância do ponto à linha.
 */
float pointToLineDistance(Point p, Line l);

/**
 * @brief Verifica se um ponto está dentro de um polígono.
 * @param p Ponto a ser verificado.
 * @param m Polígono onde será feita a verificação.
 * @return 1 se o ponto estiver dentro, 0 caso contrário.
 */
int isPointInPolygon(Point p, Mesh m);

/**
 * @brief Seleciona um ponto com base nas coordenadas (x, y).
 * @param x Coordenada X do clique.
 * @param y Coordenada Y do clique.
 * @param pointCount Número total de pontos.
 * @param points Array de pontos.
 * @return O ID do ponto selecionado ou -1 se nenhum ponto for selecionado.
 */
int selectPoint(int x, int y, int pointCount, Point *points);

/**
 * @brief Seleciona uma linha com base nas coordenadas (x, y).
 * @param x Coordenada X do clique.
 * @param y Coordenada Y do clique.
 * @param lineCount Número total de linhas.
 * @param lines Array de linhas.
 * @return O ID da linha selecionada ou -1 se nenhuma linha for selecionada.
 */
int selectLine(int x, int y, int lineCount, Line *lines);

/**
 * @brief Seleciona um polígono com base nas coordenadas (x, y).
 * @param x Coordenada X do clique.
 * @param y Coordenada Y do clique.
 * @param meshCount Número total de polígonos.
 * @param meshes Array de polígonos.
 * @return O ID do polígono selecionado ou -1 se nenhum polígono for selecionado.
 */
int selectPolygon(int x, int y, int meshCount, Mesh *meshes);

/**
 * @brief Aplica translação a um ponto.
 * @param id ID do ponto.
 * @param dx Deslocamento em X.
 * @param dy Deslocamento em Y.
 * @param points Array de pontos.
 */
void translatePoint(int id, float dx, float dy, Point *points);

/**
 * @brief Aplica translação a uma linha.
 * @param id ID da linha.
 * @param dx Deslocamento em X.
 * @param dy Deslocamento em Y.
 * @param lines Array de linhas.
 */
void translateLine(int id, float dx, float dy, Line *lines);

/**
 * @brief Aplica translação a um polígono.
 * @param id ID do polígono.
 * @param dx Deslocamento em X.
 * @param dy Deslocamento em Y.
 * @param meshes Array de polígonos.
 */
void translatePolygon(int id, float dx, float dy, Mesh *meshes);

// As demais funções seguem a mesma lógica, documentando seus parâmetros corretamente.

#endif // TRANSFORMERS_H
