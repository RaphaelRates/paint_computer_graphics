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

