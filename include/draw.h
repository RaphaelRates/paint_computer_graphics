#ifndef DRAW_H
#define DRAW_H

#include <GL/glut.h>
#include <time.h>
#include "data.h"

/**
 * @brief Desenha os pontos na tela.
 * @param points Array de pontos.
 * @param pointCount Número de pontos.
 * @param isSelected Indica se um ponto está selecionado.
 * @param IdSelectedPoint ID do ponto selecionado.
 */
void drawPoint(Point *points, int pointCount, int isSelected, int IdSelectedPoint);

/**
 * @brief Desenha as linhas na tela.
 * @param lines Array de linhas.
 * @param lineCount Número de linhas.
 * @param IdSelectedLine ID da linha selecionada.
 * @param isSelected Indica se uma linha está selecionada.
 */
void drawLines(Line *lines, int lineCount, int IdSelectedLine, int isSelected);

/**
 * @brief Desenha os polígonos na tela.
 * @param meshes Array de malhas (polígonos).
 * @param meshCount Número de polígonos.
 * @param IdSelectedPolygon ID do polígono selecionado.
 * @param isSelected Indica se um polígono está selecionado.
 */
void drawPolygon(Mesh *meshes, int meshCount, int IdSelectedPolygon, int isSelected);

/**
 * @brief Desenha a pré-visualização de um ponto.
 * @param currentMode Modo de desenho atual.
 * @param isDrawing Indica se está desenhando.
 * @param tempPoint Ponto temporário para visualização.
 */
void drawPreviewPoint(Mode currentMode, int isDrawing, Point tempPoint);

/**
 * @brief Desenha a pré-visualização de uma linha.
 * @param currentMode Modo de desenho atual.
 * @param isDrawing Indica se está desenhando.
 * @param tempPoint Ponto temporário.
 * @param colorLoading_r Componente vermelho da cor.
 * @param colorLoading_g Componente verde da cor.
 * @param colorLoading_b Componente azul da cor.
 * @param tempLine Linha temporária para visualização.
 */
void drawPreviewLine(Mode currentMode, int isDrawing, Point tempPoint, float colorLoading_r, float colorLoading_g, float colorLoading_b, Line tempLine);

/**
 * @brief Desenha a pré-visualização de um polígono.
 * @param isDrawingPolygon Indica se está desenhando um polígono.
 * @param colorLoading_r Componente vermelho da cor.
 * @param colorLoading_g Componente verde da cor.
 * @param colorLoading_b Componente azul da cor.
 * @param tempMesh Estrutura temporária de malha.
 */
void drawPreviewPolygon(int isDrawingPolygon, float colorLoading_r, float colorLoading_g, float colorLoading_b, Mesh tempMesh);

/**
 * @brief Destaca o objeto atualmente selecionado.
 * @param isSelected Indica se há um objeto selecionado.
 * @param IdSelectedPoint ID do ponto selecionado.
 * @param IdSelectedLine ID da linha selecionada.
 * @param IdSelectedPolygon ID do polígono selecionado.
 * @param points Array de pontos.
 * @param lines Array de linhas.
 * @param meshes Array de polígonos.
 */
void drawSelectedObject(int isSelected, int IdSelectedPoint, int IdSelectedLine, int IdSelectedPolygon, Point *points, Line *lines, Mesh *meshes);

/**
 * @brief Exibe mensagens na tela.
 * @param showMessage Mensagem a ser exibida.
 */
void drawMessage(MESSAGE showMessage);

/**
 * @brief Função idle para animações e atualizações.
 */
void idleFunc();

#endif
