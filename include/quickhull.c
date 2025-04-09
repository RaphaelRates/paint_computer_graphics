
#include <stdio.h>
#include <stdlib.h>
#include "quickhull.h"

int lado(Point a, Point b, Point p) {
    int val = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
}

int distancia(Point a, Point b, Point p) {
    return abs((b.x - a.x) * (a.y - p.y) - (a.x - p.x) * (b.y - a.y));
}

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

void quickHull(Point pontos[], int n) {
    if (n < 3) {
        printf("nao e possivel formar fecho convexo com menos de 3 pontos\n");
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
