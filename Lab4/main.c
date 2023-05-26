#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include<conio.h>
#define RADIUS 35
#define SMALL_OFFSET 20
#define BIG_OFFSET 40
#define OFFSET_X 16
#define OFFSET_Y 16
#define DTX 5


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Declare our buttons
__attribute__((unused)) HWND buttonDrawDirect, buttonDrawUnd, buttonPower, buttonRegCheck, buttonFindIsolated, buttonDraw2, buttonWays, buttonReachable, buttonStronglyConnectedMat,
        buttonComponent, buttonCondensat;

char ProgramName[] = "Lab 4";

void freeMemory(float** matrix, int N){
    if (matrix != NULL) {
        for (int i = 0; i < N; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
}

void arrow(float fi, int pointX, int pointY, HDC hdc){
    int leftX, leftY, rightX, rightY;
    const int LENGTH = 15;
    leftX = pointX - LENGTH * cos(fi + 0.3);
    rightX = pointX - LENGTH * cos(fi - 0.3);
    leftY = pointY - LENGTH * sin(fi + 0.3);
    rightY = pointY - LENGTH * sin(fi - 0.3);
    MoveToEx(hdc, pointX, pointY, NULL);
    LineTo(hdc, leftX, leftY);
    MoveToEx(hdc, pointX, pointY, NULL);
    LineTo(hdc, rightX, rightY);
}

void drawArrow(int fromX, int fromY, int toX, int toY, int radius, HDC hdc){
    float angleRad = atan2((toY - fromY), (toX - fromX));
    float pointX = toX - radius * cos(angleRad);
    float pointY = toY - radius * sin(angleRad);
    arrow(angleRad, pointX, pointY, hdc);
}

float** randomMatrix(int N) {
    srand(2123); // seed: 21 group 23 number in group
    float** arr = (float**)malloc(N * sizeof(float*));
    for(int i = 0; i < N; i++){
        arr[i] = (float*)malloc(N * sizeof(float));
        for(int j = 0; j < N; j++){
            arr[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0;
        }
    }

    return arr;
}

float** multiplyMatrix(float coefficient, float** matrix, int n) { //or mulmr
    float** result = (float**)malloc(n * sizeof(float*));
    for(int i = 0; i < n; i++){
        result[i] = (float*)malloc(n * sizeof(float));
        for(int j = 0; j < n; j++){
            result[i][j] = floorf(matrix[i][j] * coefficient);
        }
    }

    return result;
}

char** symbolArray(int N){
    char** array = malloc(N * sizeof(char*));
    for(int i = 0; i < N; i++) {
        array[i] = malloc(3 * sizeof(char));
        sprintf(array[i], "%d", i+1);
    }
    return array;
}

float** makeSymmetric(float** mat, int N){
    float** result = (float**)malloc(N * sizeof(float*));
    for(int i = 0; i < N; i++){
        result[i] = (float*)malloc(N * sizeof(float));
        for(int j = 0; j < N; j++){
            if(mat[i][j] == 1 || mat[j][i] == 1){
                result[i][j] = 1;
            } else {
                result[i][j] = 0;
            }
        }
    }
    return result;
}

void printMatrix(int N, float** matrix){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.0f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void printIntArray(int N, int* array){
    for(int i = 0; i < N; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
}

int checkForRegularity(int N, int* array){
    for(int i = 0; i < N; i++){
        if(array[0] == array[i]) continue;
        else return 0;
    }
    return 1;
}

void regularityPrint(int flag, int* array){
    if(flag == 0){
        printf("NO\n");
    } else if(flag == 1){
        printf("YES\nPower of regularity is: %d\n", array[1]);
    }
}

void arrayX(int N, int* array){
    float edge = N / 4.0;
    int edgeCeil = ceil(edge);
    int rightStepForButtons = 400;

    for(int i = 0; i < edgeCeil+1; i++){
        array[i] = 100 + 100*i + rightStepForButtons;
    }

    for(int i = edgeCeil+1; i < edgeCeil*2+1; i++){
        array[i] = array[i-1];
    }

    for(int i = edgeCeil*2+1; i < edgeCeil*3+1; i++){
        array[i] = array[i-1]-100;
    }

    for(int i = edgeCeil*3+1; i < edgeCeil*4 && i < N; i++){
        array[i] = array[0];
    }
}

void arrayY(int N, int* array){//count Y coordinates for graph
    float edge = N / 4.0;
    int edgeCeil = ceil(edge);

    const int LENGTH = 100;

    for(int i = 0; i < edgeCeil+1; i++){
        array[i] = LENGTH;
    }

    for(int i = edgeCeil+1; i < edgeCeil*2+1; i++){
        array[i] = array[i-1]+LENGTH;
    }

    for(int i = edgeCeil*2+1; i < edgeCeil*3+1; i++){
        array[i] = array[i-1];
    }

    for(int i = edgeCeil*3+1; i < edgeCeil*4 && i < N; i++){
        array[i] = array[i-1]-LENGTH;
    }
}

void drawUndirectedEllipses(int i, int j, int nx[], int ny[], HDC hdc, int edgeCeil){
    int dir = (int) ceil((i+1)/(float) edgeCeil);
    if(dir%2 == 0){
        if(dir > edgeCeil){
            Ellipse(hdc, nx[i]-BIG_OFFSET, ny[i]-SMALL_OFFSET, nx[i], ny[i]+SMALL_OFFSET);
        } else{
            Ellipse(hdc, nx[i]+BIG_OFFSET, ny[i]-SMALL_OFFSET, nx[i], ny[i]+SMALL_OFFSET);
        }
    } else{
        if(dir >= edgeCeil){
            Ellipse(hdc, nx[i]-SMALL_OFFSET, ny[i]+BIG_OFFSET, nx[i]+SMALL_OFFSET, ny[i]);
        } else{
            Ellipse(hdc, nx[i]-SMALL_OFFSET, ny[i]-BIG_OFFSET, nx[i]+SMALL_OFFSET, ny[i]);
        }
    }
}

void drawUndirectedOneRowLines(int i, int j, int nx[], int ny[], HDC hdc, int edgeCeil){
    if(nx[i] == nx[j]){
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j]+RADIUS, ny[i]-(ny[i]-ny[j])/2);
        MoveToEx(hdc, nx[j]+RADIUS, ny[i]-(ny[i]-ny[j])/2, NULL);
        LineTo(hdc, nx[j], ny[j]);
    } else{
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-RADIUS);
        MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-RADIUS, NULL);
        LineTo(hdc, nx[j], ny[j]);
    }
}

void drawUndirectedCommonLines(int i, int j, HDC hdc, int nx[], int ny[]){
    MoveToEx(hdc, nx[i], ny[i], NULL);
    LineTo(hdc, nx[j], ny[j]);
}

void drawUndirectedGraph(HWND hWnd, HDC hdc, const int N, char** nn, int nx[], int ny[], float** Matrix){
    int edgeCeil = ceil(N / 4.0);
    HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));
    SelectObject(hdc, KPen);

    printf("Undirected Matrix matrix:\n");
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(Matrix[j][i] == 1 || Matrix[i][j] == 1){
                printf("1 ");
            } else printf("%.0f ", Matrix[i][j]);
        }
        printf("\n");
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(Matrix[i][j] == 1){
                if(i == j){
                    drawUndirectedEllipses(i, j, nx, ny, hdc, edgeCeil);
                }
                else if(((abs(i-j) >=2 && abs(i-j) <= edgeCeil) || abs(i-j) >= 3*edgeCeil) && (nx[i] == nx[j] || ny[i] == ny[j])){
                    drawUndirectedOneRowLines(i,j,nx,ny,hdc,edgeCeil);
                }
                else{
                    drawUndirectedCommonLines(i,j,hdc,nx,ny);
                }
            }
        }
    }
    HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));

    SelectObject(hdc, BPen);
    for(int i = 0; i < N; i++){
        Ellipse(hdc, nx[i] - OFFSET_X, ny[i] - OFFSET_Y, nx[i] + OFFSET_X, ny[i] + OFFSET_Y);
        TextOut(hdc, nx[i] - DTX, ny[i] - OFFSET_Y / 2, nn[i], 2);
    }
}

void drawEllipses(int i, int j, HDC hdc, int nx[], int ny[], int dx, int edgeCeil){
    if(i == j){
        int dir = (int) ceil((i + 1) / (float) edgeCeil);
        if(dir % 2 == 0){
            if(dir > edgeCeil){
                Ellipse(hdc, nx[i]-BIG_OFFSET, ny[i]-SMALL_OFFSET, nx[i], ny[i]+SMALL_OFFSET);
                drawArrow(nx[i]-RADIUS, ny[i]+BIG_OFFSET, nx[j], ny[j], dx, hdc);
            } else{
                Ellipse(hdc, nx[i]+BIG_OFFSET, ny[i]-SMALL_OFFSET, nx[i], ny[i]+SMALL_OFFSET);
                drawArrow(nx[i]+RADIUS, ny[i]-BIG_OFFSET, nx[j], ny[j], dx, hdc);
            }
        } else{
            if(dir >= edgeCeil){
                Ellipse(hdc, nx[i]-SMALL_OFFSET, ny[i]+BIG_OFFSET, nx[i]+SMALL_OFFSET, ny[i]);
                drawArrow(nx[i]+BIG_OFFSET, ny[i]+BIG_OFFSET, nx[j], ny[j], dx, hdc);
            } else{
                Ellipse(hdc, nx[i]-SMALL_OFFSET, ny[i]-BIG_OFFSET, nx[i]+SMALL_OFFSET, ny[i]);
                drawArrow(nx[i]-BIG_OFFSET, ny[i]-BIG_OFFSET, nx[j], ny[j], dx, hdc);
            }
        }
    }
}

void drawLinesOnSameRow(int i, int j, HDC hdc, int nx[], int ny[], int dx){
    if(nx[i] == nx[j]){
        if(i > j){
            MoveToEx(hdc, nx[i], ny[i], NULL);
            LineTo(hdc, nx[j]+RADIUS, ny[i]-(ny[i]-ny[j])/2);
            MoveToEx(hdc, nx[j]+RADIUS, ny[i]-(ny[i]-ny[j])/2, NULL);
            LineTo(hdc, nx[j], ny[j]);
            drawArrow(nx[j]+RADIUS, ny[i]-(ny[i]-ny[j])/2, nx[j], ny[j], dx, hdc);
        } else{
            MoveToEx(hdc, nx[i], ny[i], NULL);
            LineTo(hdc, nx[j]-RADIUS, ny[i]-(ny[i]-ny[j])/2);
            MoveToEx(hdc, nx[j]-RADIUS, ny[i]-(ny[i]-ny[j])/2, NULL);
            LineTo(hdc, nx[j], ny[j]);
            drawArrow(nx[j]-RADIUS, ny[i]-(ny[i]-ny[j])/2, nx[j], ny[j], dx, hdc);
        }
    } else{
        if(i > j){
            MoveToEx(hdc, nx[i], ny[i], NULL);
            LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]+RADIUS);
            MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]+RADIUS, NULL);
            LineTo(hdc, nx[j], ny[j]);
            drawArrow(nx[j]+(nx[i]-nx[j])/2, ny[i]+RADIUS, nx[j], ny[j], dx, hdc);

        } else{
            MoveToEx(hdc, nx[i], ny[i], NULL);
            LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-RADIUS);
            MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-RADIUS, NULL);
            LineTo(hdc, nx[j], ny[j]);
            drawArrow(nx[j]+(nx[i]-nx[j])/2, ny[i]-RADIUS, nx[j], ny[j], dx, hdc);
        }
    }
}

void drawCommonLines(int i, int j, HDC hdc, int nx[], int ny[], int dx, float** A){
    if(i > j && A[j][i] == 1){
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, (nx[i]+nx[j])/2, (ny[i]+ny[j])/2+SMALL_OFFSET);
        MoveToEx(hdc, (nx[i]+nx[j])/2, (ny[i]+ny[j])/2+SMALL_OFFSET, NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow((nx[i]+nx[j])/2, (ny[i]+ny[j])/2+SMALL_OFFSET, nx[j], ny[j], dx, hdc);
    } else if(ny[i] == ny[j]){
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, (nx[i]+nx[j])/2+SMALL_OFFSET, (ny[i]+ny[j])/2);
        MoveToEx(hdc, (nx[i]+nx[j])/2+SMALL_OFFSET, (ny[i]+ny[j])/2, NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow((nx[i]+nx[j])/2+SMALL_OFFSET, (ny[i]+ny[j])/2, nx[j], ny[j], dx, hdc);
    } else{
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow(nx[i], ny[i], nx[j], ny[j], dx, hdc);
    }
}


void drawGraph(HWND hWnd, HDC hdc, int N, int nx[], int ny[], char** nn, float** A){
    int edgeCeil = ceil(N / 4.0);

    printf("Adjacency matrix:\n");
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.0f ", A[i][j]);
        }
        printf("\n");
    }
    int sumOfLines = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(A[i][j] == 1){
                sumOfLines++;
                drawEllipses(i, j, hdc, nx, ny, OFFSET_X, edgeCeil);
                if(((abs(i-j) >=2 && abs(i-j) <= edgeCeil) || abs(i-j) >= 3*edgeCeil) && (nx[i] == nx[j] || ny[i] == ny[j])){
                    drawLinesOnSameRow(i, j, hdc, nx, ny, OFFSET_X);
                }
                else if(i!=j){
                    drawCommonLines(i, j, hdc, nx, ny, OFFSET_X, A);
                }
            }
        }
    }

    printf("Number of all lines: %d\n", sumOfLines);

    HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
    HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));

    SelectObject(hdc, BPen);
    for(int i = 0;i < N; i++){
        Ellipse(hdc, nx[i] - OFFSET_X, ny[i] - OFFSET_Y, nx[i] + OFFSET_X, ny[i] + OFFSET_Y);
        TextOut(hdc, nx[i] - DTX, ny[i] - OFFSET_Y / 2, nn[i], 2);
    }
}

void drawIsolatedPendantVertexes(HWND hWnd, HDC hdc, int N, int nx[], int ny[], char** nn, int* isPen){
    HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HPEN yellowPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));

    for(int i = 0; i < N; i++){
        if(isPen[i] == 0){
            SelectObject(hdc, redPen);
            Ellipse(hdc, nx[i] - OFFSET_X, ny[i] - OFFSET_Y, nx[i] + OFFSET_X, ny[i] + OFFSET_Y);
            TextOut(hdc, nx[i] - DTX, ny[i] - OFFSET_Y / 2, nn[i], 2);
        } else if(isPen[i] == 1){
            SelectObject(hdc, yellowPen);
            Ellipse(hdc, nx[i] - OFFSET_X, ny[i] - OFFSET_Y, nx[i] + OFFSET_X, ny[i] + OFFSET_Y);
            TextOut(hdc, nx[i] - DTX, ny[i] - OFFSET_Y / 2, nn[i], 2);
        }
    }
}

int* findPowerOfUndirectedGraph(int N, float** matrix){//return array of powers of undirected graph vertexes
    int* result = malloc(N * sizeof(int));
    int count;

    for(int i = 0; i < N; i++){
        count = 0;
        for(int j = 0; j < N; j++){
            if(matrix[i][j] == 1.0) count++;
        }
        result[i] = count;
    }

    return result;
}

int* foundOutcomeDegrees(int N, float** mat) {
    int* result = (int*) malloc(N * sizeof(int));
    int count = 0;

    for (int i = 0; i < N; i++) {
        count = 0;
        for (int j = 0; j < N; j++) {
            if (mat[i][j] == 1) count++;
        }
        result[i] = count;
    }

    return result;
}

int* foundIncomeDegrees(int N, float** mat) {//return array of half power of incoming to vertex dependencies
    int* result = (int*) malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        result[i] = 0;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (mat[i][j] == 1) {
                result[j]++;
            }
        }
    }

    return result;
}

int* undirectedIsolatedPendant(int N, int* array){
    int* result = (int*) malloc(N * sizeof(int));

    for(int i = 0; i < N; i++){
        if(array[i] == 0){
            result[i] = 0;
        } else if(array[i] == 1){
            result[i] = 1;
        } else result[i] = 2;
    }

    return result;
}

void printIsolatedPendant(int N, int* array){

    printf("\nIsolated vertexes:\n");
    for(int i = 0; i < N; i++){
        if(array[i] == 0){
            printf("%d  ", (i+1));
        }
    }
    printf("\n\nPendant vertexes:\n");
    for(int i = 0; i < N; i++){
        if(array[i] == 1){
            printf("%d  ", (i+1));
        }
    }
}

float** multiplyMatrices(float** mat1, float** mat2, int N) {
    float** result = (float**)malloc(N * sizeof(float*));

    for (int i = 0; i < N; i++) {
        result[i] = (float*)malloc(N * sizeof(float));
        for (int j = 0; j < N; j++) {
            result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    return result;
}

float** powerMatrix(float** matrix, int N, int power) {
    float** result = (float**)malloc(N * sizeof(float*));

    for (int i = 0; i < N; i++) {
        result[i] = (float*)malloc(N * sizeof(float));
        for (int j = 0; j < N; j++) {
            result[i][j] = matrix[i][j];
        }
    }

    for (int i = 1; i < power; i++) {
        float** temp = multiplyMatrices(result, matrix, N);
        for (int j = 0; j < N; j++) {
            free(result[j]);
        }
        free(result);
        result = temp;
    }

    return result;
}

float** transposeMatrix(float** matrix, int rows, int columns) {
    float** result = (float**)malloc(columns * sizeof(float*));
    for (int i = 0; i < columns; i++) {
        result[i] = (float*)malloc(rows * sizeof(float));
        for (int j = 0; j < rows; j++) {
            result[i][j] = matrix[j][i];
        }
    }
    return result;
}

float** multiplyMatricesStraight(float** mat1, float** mat2, int N) {
    float** result = (float**)malloc(N * sizeof(float*));

    for (int i = 0; i < N; i++) {
        result[i] = (float*)malloc(N * sizeof(float));
        for (int j = 0; j < N; j++) {
            result[i][j] = mat1[i][j] * mat2[i][j];
        }
    }

    return result;
}

void findPathsOfLengthTwo(float** adjacencyMatrix, int N) {

    printf("Paths of length two:\n");

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (adjacencyMatrix[i][j] == 1) {
                for (int k = 0; k < N; k++) {
                    if (adjacencyMatrix[j][k] == 1) {
                        printf("%d -> %d -> %d\n", (i+1), (j+1), (k+1));
                    }
                }
            }
        }
    }
}

void findPathsOfLengthThree(float** adjacencyMatrix, int N) {

    printf("Paths of length three:\n");

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (adjacencyMatrix[i][j] == 1 && adjacencyMatrix[j][k] == 1) {
                    for (int m = 0; m < N; m++) {
                        if (adjacencyMatrix[k][m] == 1) {
                            printf("%d -> %d -> %d -> %d\n", (i+1), (j+1), (k+1), (m+1));
                        }
                    }
                }
            }
        }
    }
}

float** findReachabilityMatrix(float** adjacencyMatrix, int N) {
    float** reachabilityMatrix = (float**)malloc(N * sizeof(float*));

    for (int i = 0; i < N; i++) {
        reachabilityMatrix[i] = (float*)malloc(N * sizeof(float));
        for (int j = 0; j < N; j++) {
            if (adjacencyMatrix[i][j] > 0 || i == j)
                reachabilityMatrix[i][j] = 1;
            else
                reachabilityMatrix[i][j] = 0;
        }
    }

    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                reachabilityMatrix[i][j] = reachabilityMatrix[i][j] || (reachabilityMatrix[i][k] && reachabilityMatrix[k][j]);
            }
        }
    }

    return reachabilityMatrix;
}

float** findStronglyConnectedMatrix(float** adjacencyMatrix, int N) {
    float** trans = transposeMatrix(adjacencyMatrix, N, N);
    float** result = multiplyMatricesStraight(adjacencyMatrix, trans, N);

    freeMemory(trans, N);

    return result;
}

void dfs(int v, float** adjacencyMatrix, int* visited, int N) {
    visited[v] = 1;
    printf("%d ", v + 1);

    for (int i = 0; i < N; i++) {
        if (adjacencyMatrix[v][i] != 0 && !visited[i]) {
            dfs(i, adjacencyMatrix, visited, N);
        }
    }
}

void printConnectedComponents(float** adjacencyMatrix, int N) {
    int* visited = (int*)calloc(N, sizeof(int));

    for (int i = 0; i < N; i++) {
        if (!visited[i]) {
            printf("Component: ");
            dfs(i, adjacencyMatrix, visited, N);
            printf("\n");
        }
    }

    free(visited);
}

void dfs2(float** adjacencyMatrix, int N, int vertex, int* visited, int* componentLabels, int* componentIndices, int component) {
    visited[vertex] = 1;
    componentLabels[vertex] = component;

    for (int i = 0; i < N; i++) {
        if (adjacencyMatrix[vertex][i] != 0 && !visited[i]) {
            dfs2(adjacencyMatrix, N, i, visited, componentLabels, componentIndices, component);
        }
    }
}

float** getCondensedGraph(float** adjacencyMatrix, int N, int* componentCount) {
    int* visited = (int*)calloc(N, sizeof(int));
    int* componentLabels = (int*)malloc(N * sizeof(int));
    int* componentIndices = (int*)calloc(N, sizeof(int));
    int component = 0;

    for (int i = 0; i < N; i++) {
        if (!visited[i]) {
            component++;
            dfs2(adjacencyMatrix, N, i, visited, componentLabels, componentIndices, component);
        }
    }

    int numComponents = component;
    float** condensedGraph = (float**)malloc(numComponents * sizeof(float*));
    for (int i = 0; i < numComponents; i++) {
        condensedGraph[i] = (float*)calloc(numComponents, sizeof(float));
    }

    for (int i = 0; i < N; i++) {
        int componentLabel = componentLabels[i];
        for (int j = 0; j < N; j++) {
            if (adjacencyMatrix[i][j] != 0 && componentLabel != componentLabels[j]) {
                condensedGraph[componentLabel - 1][componentLabels[j] - 1] = 1.0;
            }
        }
    }

    free(visited);
    free(componentLabels);
    free(componentIndices);

    *componentCount = numComponents;
    return condensedGraph;
}


//main function from which we call all needed function onclick of buttons. Also this function response all of calculations and let hem in argument of functions
void mainFunc(int option, HWND hWnd, HDC hdc){
    const int N = 12;
    int nx[N], ny[N];
    int flag;
    int components;

    arrayX(N, nx);
    arrayY(N, ny);

    char** nn = symbolArray(N);
    float** T = randomMatrix(N);
    float** A = multiplyMatrix(0.66, T, N);//Fill our matrix
    float** A2 = multiplyMatrix(0.71, T, N);
    float** A2Power2 = powerMatrix(A2, N, 2);
    float** A2Power3 = powerMatrix(A2, N, 3);
    float** symA = makeSymmetric(A, N);
    float** symA2 = makeSymmetric(A2, N);
    float** reachabilityMatrix = findReachabilityMatrix(A2, N);
    float** strongConMat = findStronglyConnectedMatrix(reachabilityMatrix, N);
    float** condensate = getCondensedGraph(A2, N, &components);
    int* undirectedPower = findPowerOfUndirectedGraph(N, symA);
    int* outgoingDeg = foundOutcomeDegrees(N, A);
    int* incomingDeg = foundIncomeDegrees(N, A);
    int* IsolatedPendant = undirectedIsolatedPendant(N, undirectedPower);

    switch(option){
        case 1:
            drawGraph(hWnd, hdc, N, nx, ny, nn, A);
            break;
        case 2:
            drawUndirectedGraph(hWnd, hdc, N, nn, nx, ny, A);
            break;
        case 3:
            printf("Directed graph: \n");
            printMatrix(N, A);
            printf("Half degree of incoming according to above matrix:\n");
            printIntArray(N, incomingDeg);
            printf("Half degree of outcoming according to above matrix:\n");
            printIntArray(N, outgoingDeg);
            printf("\n");
            printf("Undirected graph:\n");
            printMatrix(N, symA);
            printf("Power of vertexes in undirected graph according to above matrix:\n");
            printIntArray(N, undirectedPower);
            break;
        case 4:
            printf("Is undirected graph is regular: ");
            flag = checkForRegularity(N, undirectedPower);
            regularityPrint(flag, undirectedPower);
            printf("Is directed graph is regular for incoming: ");
            flag = checkForRegularity(N, incomingDeg);
            regularityPrint(flag, incomingDeg);
            printf("Is directed graph is regular for outcoming: ");
            flag = checkForRegularity(N, outgoingDeg);
            regularityPrint(flag, outgoingDeg);
            break;
        case 5:
            drawUndirectedGraph(hWnd, hdc, N, nn, nx, ny, A);
            drawIsolatedPendantVertexes(hWnd, hdc, N, nx, ny, nn, IsolatedPendant);
            printIsolatedPendant(N, IsolatedPendant);
            break;
        case 6:
            drawGraph(hWnd, hdc, N, nx, ny, nn, A2);
            free(undirectedPower);
            free(outgoingDeg);
            free(incomingDeg);
            undirectedPower = findPowerOfUndirectedGraph(N, symA2);
            outgoingDeg = foundOutcomeDegrees(N, A2);
            incomingDeg = foundIncomeDegrees(N, A2);
            printf("Half degree of incoming according to above matrix:\n");
            printIntArray(N, incomingDeg);
            printf("Half degree of outcoming according to above matrix:\n");
            printIntArray(N, outgoingDeg);
            printf("\n");
            printf("Undirected graph:\n");
            printMatrix(N, symA2);
            printf("Power of vertexes in undirected graph according to above matrix:\n");
            printIntArray(N, undirectedPower);
            break;
        case 7:
            printf("(A2)^2:\n");
            printMatrix(N, A2Power2);
            printf("(A2)^3:\n");
            printMatrix(N, A2Power3);
            drawGraph(hWnd, hdc, N, nx, ny, nn, A2);
            findPathsOfLengthTwo(A2, N);
            findPathsOfLengthThree(A2, N);
            break;
        case 8:
            printf("Reachability matrix:\n");
            printMatrix(N, reachabilityMatrix);
            drawGraph(hWnd, hdc, N, nx, ny, nn, A2);
            break;
        case 9:
            printf("Strongly connected matrix:\n");
            printMatrix(N, strongConMat);
            drawGraph(hWnd, hdc, N, nx, ny, nn, A2);
            break;
        case 10:
            drawGraph(hWnd, hdc, N, nx, ny, nn, A2);
            printf("\n");
            printConnectedComponents(A2, N);
            break;
        case 11:
            printConnectedComponents(A2, N);
            drawGraph(hWnd, hdc, components, nx, ny, nn, condensate);
            break;
        default:
            break;
    }

    free(nn);
    free(outgoingDeg);
    free(incomingDeg);
    free(undirectedPower);
    free(IsolatedPendant);
    freeMemory(T, N);
    freeMemory(A, N);
    freeMemory(A2, N);
    freeMemory(A2Power2, N);
    freeMemory(A2Power3, N);
    freeMemory(symA, N);
    freeMemory(symA2, N);
    freeMemory(reachabilityMatrix, N);
    freeMemory(strongConMat, N);
    freeMemory(condensate, components);
}

//function that refresh console and our app window from previous action. Also after cleaning call mainFunc() with options
void windowUpdate(HWND hWnd, HDC hdc, PAINTSTRUCT ps, int option){
    InvalidateRect(hWnd, NULL, TRUE);
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // replace RGB(255, 255, 255) with desired background color
    UpdateWindow(hWnd);//Update our app window to make possible to draw new graph
    system("cls");//clear console
    hdc = BeginPaint(hWnd, &ps);
    FillRect(hdc, &ps.rcPaint, hBrush);
    mainFunc(option, hWnd, hdc);//Call main function that call needed functions
    EndPaint(hWnd, &ps);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow){
    WNDCLASS w;

    w.lpszClassName = ProgramName;
    w.hInstance = hInstance;
    w.lpfnWndProc = WndProc;
    w.hCursor = LoadCursor(NULL, IDC_ARROW);
    w.hIcon = 0;
    w.lpszMenuName = 0;
    w.hbrBackground = WHITE_BRUSH;
    w.style = CS_HREDRAW|CS_VREDRAW;
    w.cbClsExtra = 0;
    w.cbWndExtra = 0;

    if(!RegisterClass(&w)) return 0;

    HWND hWnd;
    MSG lpMsg;

    hWnd = CreateWindow(ProgramName, "Lab 4", WS_OVERLAPPEDWINDOW, 100, 100, 1200, 700, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, (HINSTANCE)NULL);
    ShowWindow(hWnd, nCmdShow);
    int b;
    while((b = GetMessage(&lpMsg, hWnd, 0, 0))!= 0) {
        if(b == -1)	{
            return lpMsg.wParam;
        }
        else {
            TranslateMessage(&lpMsg);
            DispatchMessage(&lpMsg);
        }
    }

    return(lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    PAINTSTRUCT ps;
    switch(message){
        case WM_CREATE:

            buttonDrawDirect = CreateWindow("BUTTON",
                                            "Draw directed graph",
                                            WS_VISIBLE | WS_CHILD | WS_BORDER,
                                            20, 20, 150, 30,
                                            hWnd, (HMENU) 1, NULL, NULL);
            buttonDrawUnd = CreateWindow("BUTTON",
                                         "Draw undirected graph",
                                         WS_VISIBLE | WS_CHILD | WS_BORDER,
                                         20, 50, 150, 30,
                                         hWnd, (HMENU) 2, NULL, NULL);
            buttonPower = CreateWindow("BUTTON",
                                       "Power graph",
                                       WS_VISIBLE | WS_CHILD | WS_BORDER,
                                       20, 80, 150, 30,
                                       hWnd, (HMENU) 3, NULL, NULL);
            buttonRegCheck = CreateWindow("BUTTON",
                                          "Check for regular graph",
                                          WS_VISIBLE | WS_CHILD | WS_BORDER,
                                          20, 110, 180, 30,
                                          hWnd, (HMENU) 4, NULL, NULL);
            buttonFindIsolated = CreateWindow("BUTTON",
                                              "Show isolated and pendant",
                                              WS_VISIBLE | WS_CHILD | WS_BORDER,
                                              20, 140, 200, 30,
                                              hWnd, (HMENU) 5, NULL, NULL);
            buttonDraw2 = CreateWindow("BUTTON",
                                       "Draw A2 and show power",
                                       WS_VISIBLE | WS_CHILD | WS_BORDER,
                                       20, 170, 200, 30,
                                       hWnd, (HMENU) 6, NULL, NULL);
            buttonWays = CreateWindow("BUTTON",
                                      "Show all 2 and 3 steps ways",
                                      WS_VISIBLE | WS_CHILD | WS_BORDER,
                                      20, 200, 200, 30,
                                      hWnd, (HMENU) 7, NULL, NULL);
            buttonReachable = CreateWindow("BUTTON",
                                           "A2 matrix of reachability",
                                           WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           20, 230, 200, 30,
                                           hWnd, (HMENU) 8, NULL, NULL);
            buttonStronglyConnectedMat = CreateWindow("BUTTON",
                                                      "Strongly connected matrix",
                                                      WS_VISIBLE | WS_CHILD | WS_BORDER,
                                                      20, 260, 200, 30,
                                                      hWnd, (HMENU) 9, NULL, NULL);
            buttonComponent = CreateWindow("BUTTON",
                                           "Components of str. connection",
                                           WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           20, 290, 200, 30,
                                           hWnd, (HMENU) 10, NULL, NULL);
            buttonCondensat = CreateWindow("BUTTON",
                                           "Graph of Condensate",
                                           WS_VISIBLE | WS_CHILD | WS_BORDER,
                                           20, 320, 200, 30,
                                           hWnd, (HMENU) 11, NULL, NULL);
            break;
        case WM_COMMAND:

            switch(LOWORD(wParam)){
                case 1:
                    windowUpdate(hWnd, hdc, ps, 1);
                    break;
                case 2:
                    windowUpdate(hWnd, hdc, ps, 2);
                    break;
                case 3:
                    windowUpdate(hWnd, hdc, ps, 3);
                    break;
                case 4:
                    windowUpdate(hWnd, hdc, ps, 4);
                    break;
                case 5:
                    windowUpdate(hWnd, hdc, ps, 5);
                    break;
                case 6:
                    windowUpdate(hWnd, hdc, ps, 6);
                    break;
                case 7:
                    windowUpdate(hWnd, hdc, ps, 7);
                    break;
                case 8:
                    windowUpdate(hWnd, hdc, ps, 8);
                    break;
                case 9:
                    windowUpdate(hWnd, hdc, ps, 9);
                    break;
                case 10:
                    windowUpdate(hWnd, hdc, ps, 10);
                    break;
                case 11:
                    windowUpdate(hWnd, hdc, ps, 11);
                    break;
            }
            break;
        case WM_PAINT:
            UpdateWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}