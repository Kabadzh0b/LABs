#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include<conio.h>
#include <stdbool.h>

#define RADIUS 35
#define SMALL_OFFSET 20
#define BIG_OFFSET 40
#define OFFSET_X 16
#define OFFSET_Y 16
#define DTX 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Declare our buttons
HWND buttonDFS, buttonBFS, buttonNextBFS, buttonNextDFS;
bool waitingButtonBFS = true, waitingButtonDFS = true;

char ProgName[] = "Lab 5";

void freeMatrix(int** matrix, int N){
    if (matrix != NULL) {
        for (int i = 0; i < N; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
}

void arrow(float fi, int px, int py, HDC hdc){//draw arrow
    int leftX, leftY, rightX, rightY;
    leftX = px - 15 * cos(fi + 0.3);
    rightX = px - 15 * cos(fi - 0.3);
    leftY = py - 15 * sin(fi + 0.3);
    rightY = py - 15 * sin(fi - 0.3);
    MoveToEx(hdc, px, py, NULL);
    LineTo(hdc, leftX, leftY);
    MoveToEx(hdc, px, py, NULL);
    LineTo(hdc, rightX, rightY);
}

void drawArrow(int fromX, int fromY, int tox, int toy, int r, HDC hdc){//count angle and call function for drawing arrow
    float angleRad = atan2((toy - fromY), (tox - fromX));
    float px = tox - r*cos(angleRad);
    float py = toy - r*sin(angleRad);
    arrow(angleRad, px, py, hdc);
}

float** randMatrix(int N) {//generate random matrix with value from 0 to 2.0
    const short SEED = 2123;
    srand(SEED);
    float** arr = (float**)malloc(N * sizeof(float*));
    for(int i = 0; i < N; i++){
        arr[i] = (float*)malloc(N * sizeof(float));
        for(int j = 0; j < N; j++){
            arr[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0;
        }
    }

    return arr;
}

float** mulmr(float c, float** mat, int n) {//round matrix from randMatrix() to 0 1 matrix
    float** res = (float**)malloc(n * sizeof(float*));
    for(int i = 0; i < n; i++){
        res[i] = (float*)malloc(n * sizeof(float));
        for(int j = 0; j < n; j++){
            res[i][j] = floor(mat[i][j] * c);
        }
    }

    return res;
}

char** symbolArray(int N){//return symbol char array of pointer with elements from 1 to N
    char** array = malloc(N * sizeof(char*));
    for(int i = 0; i < N; i++) {
        array[i] = malloc(3 * sizeof(char)); // allocate memory for each element
        sprintf(array[i], "%d", i+1); // use sprintf to convert int to string
    }
    return array;
}

float** makeSymmetric(float** mat, int N){//receive 0 1 matrix A and make it symmetric for 1 (undirected graph)
    float** arr = (float**)malloc(N * sizeof(float*));
    for(int i = 0; i < N; i++){
        arr[i] = (float*)malloc(N * sizeof(float));
        for(int j = 0; j < N; j++){
            if(mat[i][j] == 1 || mat[j][i] == 1){
                arr[i][j] = 1;
            } else {
                arr[i][j] = 0;
            }
        }
    }
    return arr;
}

void printIntMatrix(int N, int** mat){//Print received matrix
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

void arrayX(int N, int* array){//count X coordinates for graph
    float edge = N / 4.0;
    int edgeCeil = ceil(edge);
    int stepRigth = 400;//step to go away from left border of window, to let space for buttons

    for(int i = 0; i < edgeCeil+1; i++){
        array[i] = 100 + 100*i + stepRigth;
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

    for(int i = 0; i < edgeCeil+1; i++){
        array[i] = 100;
    }

    for(int i = edgeCeil+1; i < edgeCeil*2+1; i++){
        array[i] = array[i-1]+100;
    }

    for(int i = edgeCeil*2+1; i < edgeCeil*3+1; i++){
        array[i] = array[i-1];
    }

    for(int i = edgeCeil*3+1; i < edgeCeil*4 && i < N; i++){
        array[i] = array[i-1]-100;
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
//from received matrix of dependencies, coordinates, names and number of vertex draw directed graph
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

void drawCircle(HDC hdc, int x, int y, int radius, char state, int vertexNumber) {
    HBRUSH hBrush;
    COLORREF color;
    char vertexLabel[10];

    switch (state) {
        case 'a':
            color = RGB(255, 0, 0); // Red for active vertices
            sprintf(vertexLabel, "%d(a)", vertexNumber + 1); // Vertex numbers from 1 to N
            break;
        case 'c':
            color = RGB(0, 0, 255); // Blue for closed vertices
            sprintf(vertexLabel, "%d(c)", vertexNumber + 1); // Vertex numbers from 1 to N
            break;
        case 'v':
            color = RGB(0, 255, 0);
            sprintf(vertexLabel, "%d(v)", vertexNumber + 1);
            break;
        default:
            color = RGB(255, 255, 255); // White for other vertices
            sprintf(vertexLabel, "%d", vertexNumber + 1); // Vertex numbers from 1 to N
            break;
    }

    hBrush = CreateSolidBrush(color);
    SelectObject(hdc, hBrush);

    int left = x - radius;
    int top = y - radius;
    int right = x + radius;
    int bottom = y + radius;

    Ellipse(hdc, left, top, right, bottom);
    FloodFill(hdc, x, y, color);

    SIZE textSize;
    GetTextExtentPoint32(hdc, vertexLabel, strlen(vertexLabel), &textSize);
    int textX = x - textSize.cx / 2;
    int textY = y - textSize.cy / 2;
    TextOut(hdc, textX, textY, vertexLabel, strlen(vertexLabel));
    DeleteObject(hBrush);
}

void drawSameNXCondition(int i, int j, HDC hdc, int* nx, int* ny, int circleRadius){
    if (i > j) {
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j] + RADIUS, ny[i] - (ny[i] - ny[j]) / 2);
        MoveToEx(hdc, nx[j] + RADIUS, ny[i] - (ny[i] - ny[j]) / 2, NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow(nx[j] + RADIUS, ny[i] - (ny[i] - ny[j]) / 2, nx[j], ny[j], circleRadius, hdc);
        drawCircle(hdc, nx[j], ny[j], circleRadius, 'v', j);
        drawCircle(hdc, nx[i], ny[i], circleRadius, 'a', i);
    }
    else {
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j] - RADIUS, ny[i] - (ny[i] - ny[j]) / 2);
        MoveToEx(hdc, nx[j] - RADIUS, ny[i] - (ny[i] - ny[j]) / 2, NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow(nx[j] - RADIUS, ny[i] - (ny[i] - ny[j]) / 2, nx[j], ny[j], circleRadius, hdc);
        drawCircle(hdc, nx[j], ny[j], circleRadius, 'v', j);
        drawCircle(hdc, nx[i], ny[i], circleRadius, 'a', i);
    }
}

void drawNotSameNXCondition(int i, int j, HDC hdc, int* nx, int* ny, int circleRadius){
    if (i > j) {
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j] + (nx[i] - nx[j]) / 2, ny[i] + RADIUS);
        MoveToEx(hdc, nx[j] + (nx[i] - nx[j]) / 2, ny[i] + RADIUS, NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow(nx[j] + (nx[i] - nx[j]) / 2, ny[i] + RADIUS, nx[j], ny[j], circleRadius, hdc);
        drawCircle(hdc, nx[j], ny[j], circleRadius, 'v', j);
        drawCircle(hdc, nx[i], ny[i], circleRadius, 'a', i);
    }
    else {
        MoveToEx(hdc, nx[i], ny[i], NULL);
        LineTo(hdc, nx[j] + (nx[i] - nx[j]) / 2, ny[i] - RADIUS);
        MoveToEx(hdc, nx[j] + (nx[i] - nx[j]) / 2, ny[i] - RADIUS, NULL);
        LineTo(hdc, nx[j], ny[j]);
        drawArrow(nx[j] + (nx[i] - nx[j]) / 2, ny[i] - RADIUS, nx[j], ny[j], circleRadius, hdc);
        drawCircle(hdc, nx[j], ny[j], circleRadius, 'v', j);
        drawCircle(hdc, nx[i], ny[i], circleRadius, 'a', i);
    }
}

void drawCommonLinesTransition(int i, int j, HDC hdc, int* nx, int* ny, int circleRadius){
    MoveToEx(hdc, nx[i], ny[i], NULL);
    LineTo(hdc, nx[j], ny[j]);
    drawArrow(nx[i], ny[i], nx[j], ny[j], circleRadius, hdc);
    MoveToEx(hdc, nx[i], ny[i], NULL);
    drawCircle(hdc, nx[j], ny[j], circleRadius, 'v', j);
    drawCircle(hdc, nx[i], ny[i], circleRadius, 'a', i);
}

void drawTransition(HDC hdc, int N, int i, int j, int nx[], int ny[], int last, int startVertex, int counter, int visited[]) {
    int edgeCeil = ceil(N / 4.0);
    int circleRadius = 16;
    COLORREF lineColor = RGB(255, 0, 0);
    int lineWidth = 3; // Weight of lines
    HPEN hPen = CreatePen(PS_SOLID, lineWidth, lineColor);
    SelectObject(hdc, hPen);

    if (i != startVertex) {
        drawCircle(hdc, nx[last], ny[last], circleRadius, 'v', last);
    }

    bool condition = ((abs(i - j) >= 2 && abs(i - j) <= edgeCeil) || abs(i - j) >= 3 * edgeCeil) && (nx[i] == nx[j] || ny[i] == ny[j]);

    if (condition) {
        if (nx[i] == nx[j]) {
            drawSameNXCondition(i,j,hdc,nx,ny,circleRadius);
        }
        else {
            drawNotSameNXCondition(i,j,hdc,nx,ny,circleRadius);
        }
    }
    else {
        drawCommonLinesTransition(i,j,hdc,nx,ny,circleRadius);
    }

    if (i != last) {
        drawCircle(hdc, nx[last], ny[last], circleRadius, 'c', last);
    }

    DeleteObject(hPen);
}

void bfs(float** adjacencyMatrix, int numVertices, int firstNode, HDC hdc, int nx[], int ny[], int* visited, int** treeMatrix) {

    int* queue = (int*)malloc(numVertices * sizeof(int));
    int* visitedFrom = (int*)malloc(numVertices * sizeof(int));

    int front = 0;
    int last = firstNode;
    int counter = 0;
    int index = 0;

    queue[index] = firstNode;
    visited[firstNode] = 1;
    visitedFrom[firstNode] = firstNode;

    while (front <= index) {
        counter++;
        int currentVertex = queue[front++];
        int fromVertex = visitedFrom[currentVertex];

        treeMatrix[fromVertex][currentVertex] = 1;
        printf("We came to vertex: %d. From vertex: %d\n", (currentVertex + 1), (fromVertex + 1));
        drawTransition(hdc, numVertices, fromVertex, currentVertex, nx, ny, last, firstNode, counter, queue);

        last = fromVertex;
        waitingButtonBFS = true;

        while (waitingButtonBFS) {
            MSG message;
            GetMessage(&message, NULL, 0, 0);
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        for (int i = 0; i < numVertices; i++) {
            if (adjacencyMatrix[currentVertex][i] != 0 && visited[i] == 0) {
                queue[++index] = i;
                visited[i] = 1;
                visitedFrom[i] = currentVertex;
            }
        }
    }

    for(int i = 0; i < numVertices; i++){
        if(visited[i] == 1){
            drawCircle(hdc, nx[i], ny[i], 16, 'c', i);
        }
    }

    for(int i = 0; i < numVertices; i++){
        if(visited[i] != 1){
            bfs(adjacencyMatrix, numVertices, i, hdc, nx, ny, visited, treeMatrix);
        }
    }

    printf("\nCorrespondent matrix of vertexes and gotten numeration\n");
    printf("Start numeration/ gotten numeration from algorithm\n");
    for(int i = 0; i < numVertices; i++){
        printf("\t%d / %d \n", (i+1), (queue[i]+1));
    }

    free(queue);
    free(visitedFrom);
}

void dfs(float** adjMatrix, int n, int startVertex, HDC hdc, int nx[], int ny[], int* visited, int** treeMatrix) {
    int* stack = malloc((n * 3) * sizeof(int));
    int* queue = malloc(n * sizeof(int));
    int top = -1;
    int last = startVertex;
    int counter = 0;
    int* transitionFrom = malloc(n * sizeof(int));
    for(int i = 0; i < n; i++){
        transitionFrom[i] = startVertex;
    }

    stack[++top] = startVertex;

    while (top != -1) {
        counter++;
        int currentVertex = stack[top--];

        if (visited[currentVertex]) {
            continue;
        }

        visited[currentVertex] = 1;
        printf("Visited: %d ", (1 + currentVertex));
        treeMatrix[transitionFrom[currentVertex]][currentVertex] = 1;

        if (top >= 0) {
            printf("Transition from: %d\n", (1 + transitionFrom[currentVertex]));
            queue[counter-1] = (currentVertex);
        } else {
            printf("(start vertex)\n");
        }

        drawTransition(hdc, n, transitionFrom[currentVertex], currentVertex, nx, ny, last, startVertex, counter, transitionFrom);
        last = transitionFrom[currentVertex];

        waitingButtonDFS = true;

        while(waitingButtonDFS){
            MSG msg;
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for (int i = n - 1; i >= 0; i--) {
            if (adjMatrix[currentVertex][i] != 0 && !visited[i]) {
                stack[++top] = i;
                transitionFrom[i] = currentVertex;
            }

        }
    }

    for(int i = 0; i < n; i++){
        if(visited[i] == 1){
            drawCircle(hdc, nx[i], ny[i], 16, 'c', i);
        }
    }

    for(int i = 0; i < n; i++){
        if(visited[i] != 1){
            dfs(adjMatrix, n, i, hdc, nx, ny, visited, treeMatrix);
        }
    }

    printf("\nCorrespondent matrix of vertexes and gotten numeration\n");
    printf("Start numeration/ gotten numeration from algorithm\n");
    for(int i = 0; i < n; i++){
        printf("\t%d / %d \n", (i+1), (queue[i]+1));
    }


    free(queue);
    free(stack);
    free(transitionFrom);
}

//main function from which we call all needed function onclick of buttons. Also this function response all of calculations and let hem in argument of functions
void mainFunc(int option, HWND hWnd, HDC hdc){
    const int N = 12;//Number of our vertex
    int nx[N], ny[N];

    int* visited = (int*)malloc(N * sizeof(int));
    int** treeMatrix = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        visited[i] = 0;
        treeMatrix[i] = (int*)malloc(N * sizeof(int));
        for(int j = 0; j < N; j++){
            treeMatrix[i][j] = 0;
        }
    }

    arrayX(N, nx);
    arrayY(N, ny);

    char** nn = symbolArray(N);
    float** randMatrix = randMatrix(N);
    float** roundedMatrix = mulmr(0.825, randMatrix, N);//Fill our matrix
    float** symmetricMatrix = makeSymmetric(roundedMatrix, N);

    switch(option){
        case 1:
            drawGraph(hWnd, hdc, N, nx, ny, nn, roundedMatrix);
            bfs(roundedMatrix, N, 0, hdc, nx, ny, visited, treeMatrix);
            printf("\nMatrix of graph tree BFS:\n");
            printIntMatrix(N, treeMatrix);
            break;
        case 2:
            drawGraph(hWnd, hdc, N, nx, ny, nn, roundedMatrix);
            dfs(roundedMatrix, N, 0, hdc, nx, ny, visited, treeMatrix);
            printf("\nMatrix of graph tree DFS:\n");
            printIntMatrix(N, treeMatrix);
            break;
    }

    free(nn);
    free(visited);
    freeMatrix(treeMatrix, N);
    freeMatrix(randMatrix, N);
    freeMatrix(roundedMatrix, N);
    freeMatrix(symmetricMatrix, N);
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

    w.lpszClassName = ProgName;
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

    hWnd = CreateWindow(ProgName, "Lab 5. Lesko Dmytro IM-21", WS_OVERLAPPEDWINDOW, 100, 100, 1200, 700, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, (HINSTANCE)NULL);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    PAINTSTRUCT ps;
    switch(messg){
        case WM_CREATE:

            buttonBFS = CreateWindow("BUTTON",
                                     "Start BFS algorithm",
                                     WS_VISIBLE | WS_CHILD | WS_BORDER,
                                     20, 20, 150, 30,
                                     hWnd, (HMENU) 1, NULL, NULL);
            buttonDFS = CreateWindow("BUTTON",
                                     "Start DFS algorithm",
                                     WS_VISIBLE | WS_CHILD | WS_BORDER,
                                     20, 50, 150, 30,
                                     hWnd, (HMENU) 2, NULL, NULL);
            buttonNextBFS = CreateWindow("BUTTON",
                                         "Go to next vertex BFS",
                                         WS_VISIBLE | WS_CHILD | WS_BORDER,
                                         320, 20, 150, 30,
                                         hWnd, (HMENU) 3, NULL, NULL);
            buttonNextDFS = CreateWindow("BUTTON",
                                         "Go to next vertex DFS",
                                         WS_VISIBLE | WS_CHILD | WS_BORDER,
                                         320, 50, 150, 30,
                                         hWnd, (HMENU) 4, NULL, NULL);
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
                    waitingButtonBFS = false;
                    break;
                case 4:
                    waitingButtonDFS = false;
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
            return DefWindowProc(hWnd, messg, wParam, lParam);
    }

    return 0;
}