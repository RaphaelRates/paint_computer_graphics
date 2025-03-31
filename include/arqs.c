
#include <arqs.h>

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

// 🔹 Função para carregar objetos do arquivo
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
        case 1: { // 🔹 Carregar pontos
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
        case 2: { // 🔹 Carregar linhas
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
        case 3: { // 🔹 Carregar polígonos
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
