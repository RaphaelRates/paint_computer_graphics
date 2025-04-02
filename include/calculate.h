#ifndef CALCULATE_H
#define CALCULATE_H

#include "data.h"

/**
 * @brief Calcula o ponto central de uma linha.
 * @param l Estrutura da linha.
 * @return Ponto central da linha.
 */
Point calculateLineCenter(Line l);

/**
 * @brief Calcula o ponto central de um polígono (malha).
 * @param m Estrutura da malha.
 * @return Ponto central do polígono.
 */
Point calculatePolygonCenter(Mesh m);

/**
 * @brief Calcula a distância entre dois pontos.
 * @param p1 Primeiro ponto.
 * @param p2 Segundo ponto.
 * @return Distância entre os dois pontos.
 */
float distance(Point p1, Point p2);

#endif