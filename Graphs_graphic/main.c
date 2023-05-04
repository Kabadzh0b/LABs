#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

// n1 = 2
// n2 = 1
// n3 = 2
// n4 = 3
// Число вершин = 10 + n3 = 12
// Розміщення вершин - прямокутником(квадратом) n4

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char ProgName[] = "Lab 3 ASD";

void arrow(float fi, int pointX, int pointY, HDC hdc){
    int leftX, leftY, rightX, rightY;
    leftX = pointX - 15 * cos(fi + 0.3);
    rightX = pointX - 15 * cos(fi - 0.3);
    leftY = pointY - 15 * sin(fi + 0.3);
    rightY = pointY - 15 * sin(fi - 0.3);
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

float** multiplyMatrix(float c, float** mat, int n) { //or mulmr
    float** result = (float**)malloc(n * sizeof(float*));
    for(int i = 0; i < n; i++){
        result[i] = (float*)malloc(n * sizeof(float));
        for(int j = 0; j < n; j++){
            result[i][j] = floor(mat[i][j] * c);
        }
    }
    return result;
}

float** randomizeMatrix(int n) { //or randm
    srand(2123); //n1,n2,n3,n4
    float** arr = (float**)malloc(n * sizeof(float*));
    for(int i = 0; i < n; i++){
        arr[i] = (float*)malloc(n * sizeof(float));
        for(int j = 0; j < n; j++){
            arr[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0;
        }
    }
    return arr;
}

char** createSymbolArray(int N){
    char** array = malloc(N * sizeof(char*));
    for(int i = 0; i < N; i++) {
        array[i] = malloc(3 * sizeof(char));
        sprintf(array[i], "%d", i+1);
    }
    return array;
}

void fillArrayX(int N, int* array){
    float edge = N / 4.0;
    int edgeCeil = ceil(edge);

    for(int i = 0; i < edgeCeil+1; i++){
        array[i] = 100 + 100*i;
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

void fillArrayY(int N, int* array){
    float edge = N / 4.0;
    const int DISTANCE = 100;
    int edgeCeil = ceil(edge);

    for(int i = 0; i < edgeCeil+1; i++){
        array[i] = DISTANCE;
    }

    for(int i = edgeCeil+1; i < edgeCeil*2+1; i++){
        array[i] = array[i-1]+DISTANCE;
    }

    for(int i = edgeCeil*2+1; i < edgeCeil*3+1; i++){
        array[i] = array[i-1];
    }

    for(int i = edgeCeil*3+1; i < edgeCeil*4 && i < N; i++){
        array[i] = array[i-1]-DISTANCE;
    }
}

void drawUndirectedGraph(HWND hWnd, HDC hdc, int n, char** nn, int nx[], int ny[], float** A){
    int edgeCeil = ceil(n / 4.0);
    int step = (edgeCeil + 2) * 100;
    printf("X coordinates for undirected graph: ");
    for(int i = 0; i < n; i++){
        nx[i] = nx[i] + step;
        printf("%d ", nx[i]);
    }
    printf("\n");
    //log out matrix of dependencies
    printf("Undirected A matrix:\n");
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(A[j][i] == 1 || A[i][j] == 1){
                printf("1 ");
            } else printf("%.0f ", A[i][j]);
        }
        printf("\n");
    }

    int dx = 16, dy = 16, dtx = 5;
    //switch pen
    HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));
    SelectObject(hdc, KPen);

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(A[i][j] == 1 && abs(i-j) >=2 && (nx[i] == nx[j] || ny[i] == ny[j])){
                const int OFFSET = 30;
                if(nx[i] == nx[j]){
                    MoveToEx(hdc, nx[i], ny[i], NULL);
                    LineTo(hdc, nx[j]+OFFSET, ny[i]-(ny[i]-ny[j])/2);
                    MoveToEx(hdc, nx[j]+OFFSET, ny[i]-(ny[i]-ny[j])/2, NULL);
                    LineTo(hdc, nx[j], ny[j]);
                } else{
                    MoveToEx(hdc, nx[i], ny[i], NULL);
                    LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-OFFSET);
                    MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-OFFSET, NULL);
                    LineTo(hdc, nx[j], ny[j]);
                }
            }
            else if(A[i][j] == 1){
                if(i == j){
                    int dir = (int) ceil((i+1)/(float) edgeCeil);
                    const int ELLIPSE_WIDTH = 40;
                    const int ELLIPSE_HEIGHT = 20;
                    if(dir%2 == 0){
                        if(dir > edgeCeil){
                            Ellipse(hdc, nx[i]-ELLIPSE_WIDTH, ny[i]-ELLIPSE_HEIGHT, nx[i], ny[i]+ELLIPSE_HEIGHT);
                        } else{
                            Ellipse(hdc, nx[i]+ELLIPSE_WIDTH, ny[i]-ELLIPSE_HEIGHT, nx[i], ny[i]+ELLIPSE_HEIGHT);
                        }
                    } else{
                        if(dir >= edgeCeil){
                            Ellipse(hdc, nx[i]-ELLIPSE_HEIGHT, ny[i]+ELLIPSE_WIDTH, nx[i]+ELLIPSE_HEIGHT, ny[i]);
                        } else{
                            Ellipse(hdc, nx[i]-ELLIPSE_HEIGHT, ny[i]-ELLIPSE_WIDTH, nx[i]+ELLIPSE_HEIGHT, ny[i]);
                        }
                    }
                }
                else{
                    MoveToEx(hdc,  nx[i], ny[i], NULL);
                    LineTo(hdc, nx[j], ny[j]);
                }
            }
        }
    }
    HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));

    SelectObject(hdc, BPen);
    for(int i = 0;i < n; i++){
        Ellipse(hdc, nx[i]-dx,ny[i]-dy,nx[i]+dx,ny[i]+dy);
        TextOut(hdc, nx[i]-dtx,ny[i]-dy/2, nn[i],2);
    }
}

void drawGraph(HWND hWnd, HDC hdc){
    const int N = 12;
    int edgeCeil = ceil(N / 4.0);
    int nx[N], ny[N];
    char** nn = createSymbolArray(N);
    fillArrayX(N, nx);
    fillArrayY(N, ny);

    //Logging out info
    printf("Vertex`s name: ");
    for(int i = 0; i < N; i++){
        printf("%s ", nn[i]);
    }
    printf("%\nX coordinates: ");
    for(int i = 0; i < N; i++){
        printf("%d ", nx[i]);
    }
    printf("%\nY coordinates");
    for(int i = 0; i < N; i++){
        printf("%d ", ny[i]);
    }
    printf("\n");
    int dx = 16, dy = 16, dtx = 5;

    float** T = randomizeMatrix(N);
    float** A = multiplyMatrix(0.715, T, N);

    printf("T:\n");
    //logging out random matrix
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.2f ", T[i][j]);
        }
        printf("\n");
    }

    printf("A:\n");
    //logging out matrix of dependencies
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.0f ", A[i][j]);
        }
        printf("\n");
    }
    A[0][9] = 1;
    A[9][0] = 0;
    for(int i = 0; i < N; i++){ // It will be perfect to decompose
        for(int j = 0; j < N; j++){
            if(A[i][j] == 1){
                if(abs(i-j) >=2 && (nx[i] == nx[j] || ny[i] == ny[j])){
                    const int OFFSETX = 30;
                    const int OFFSETY = 30;
                    if(nx[i] == nx[j]){
                        if(i > j){ //to avoid 2 sides arrows
                            MoveToEx(hdc, nx[i], ny[i], NULL);
                            LineTo(hdc, nx[j]+OFFSETX, ny[i]-(ny[i]-ny[j])/2);
                            MoveToEx(hdc, nx[j]+OFFSETX, ny[i]-(ny[i]-ny[j])/2, NULL);
                            LineTo(hdc, nx[j], ny[j]);
                            drawArrow(nx[j]+OFFSETX, ny[i]-(ny[i]-ny[j])/2, nx[j], ny[j], dx, hdc);
                        } else{
                            MoveToEx(hdc, nx[i], ny[i], NULL);
                            LineTo(hdc, nx[j]-OFFSETX, ny[i]-(ny[i]-ny[j])/2);
                            MoveToEx(hdc, nx[j]-OFFSETX, ny[i]-(ny[i]-ny[j])/2, NULL);
                            LineTo(hdc, nx[j], ny[j]);
                            drawArrow(nx[j]-OFFSETX, ny[i]-(ny[i]-ny[j])/2, nx[j], ny[j], dx, hdc);
                        }
                    } else{
                        if(i > j){ //to avoid 2 sides arrows
                            MoveToEx(hdc, nx[i], ny[i], NULL);
                            LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]+OFFSETY);
                            MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]+OFFSETY, NULL);
                            LineTo(hdc, nx[j], ny[j]);
                            drawArrow(nx[j]+(nx[i]-nx[j])/2, ny[i]+OFFSETY, nx[j], ny[j], dx, hdc);

                        } else{
                            MoveToEx(hdc, nx[i], ny[i], NULL);
                            LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-OFFSETY);
                            MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-OFFSETY, NULL);
                            LineTo(hdc, nx[j], ny[j]);
                            drawArrow(nx[j]+(nx[i]-nx[j])/2, ny[i]-OFFSETY, nx[j], ny[j], dx, hdc);
                        }
                    }
                }
                if(i == j){
                    int dir = (int) ceil((i+1)/(float) edgeCeil);
                    const int ARROW_LENGTH = 35;
                    const int ARROW_OFFSET = 50;
                    const int ELLIPSE_WIDTH = 40;
                    const int ELLIPSE_HEIGHT = 20;
                    if(dir%2 == 0){
                        if(dir > edgeCeil){
                            Ellipse(hdc, nx[i]-ELLIPSE_WIDTH, ny[i]-ELLIPSE_HEIGHT, nx[i], ny[i]+ELLIPSE_HEIGHT);
                            drawArrow(nx[i]-ARROW_LENGTH, ny[i]+ARROW_OFFSET, nx[j], ny[j], dx, hdc);
                        } else{
                            Ellipse(hdc, nx[i]+ELLIPSE_WIDTH, ny[i]-ELLIPSE_HEIGHT, nx[i], ny[i]+ELLIPSE_HEIGHT);
                            drawArrow(nx[i]+ARROW_LENGTH, ny[i]-ARROW_OFFSET, nx[j], ny[j], dx, hdc);
                        }
                    } else{
                        if(dir >= edgeCeil){
                            Ellipse(hdc, nx[i]-ELLIPSE_HEIGHT, ny[i]+ELLIPSE_WIDTH, nx[i]+ELLIPSE_HEIGHT, ny[i]);
                            drawArrow(nx[i]+ARROW_OFFSET, ny[i]+ARROW_OFFSET, nx[j], ny[j], dx, hdc);
                        } else{
                            Ellipse(hdc, nx[i]-ELLIPSE_HEIGHT, ny[i]-ELLIPSE_WIDTH, nx[i]+ELLIPSE_HEIGHT, ny[i]);
                            drawArrow(nx[i]-ARROW_OFFSET, ny[i]-ARROW_OFFSET, nx[j], ny[j], dx, hdc);
                        }
                    }

                }
            }
        }
    }

    int summary = 0;//For lines, can be included into the upper for cycle
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(A[i][j] == 1) {
                summary++;
                if (!(abs(i - j) >= 2 && (nx[i] == nx[j] || ny[i] == ny[j])) && i!=j){
                    const int OFFSET = 30;
                    if (i > j && A[j][i] == 1 && A[i][j] == 1) {
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, (nx[i] + nx[j]) / 2 + OFFSET, (ny[i] + ny[j]) / 2);
                        MoveToEx(hdc, (nx[i] + nx[j]) / 2 + OFFSET, (ny[i] + ny[j]) / 2, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow((nx[i] + nx[j]) / 2 + OFFSET, (ny[i] + ny[j]) / 2, nx[j], ny[j], dx, hdc);
                    }
                    else if(j > i && A[i][j] == 1 && A[j][i] == 1) {
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, (nx[i] + nx[j]) / 2 - OFFSET, (ny[i] + ny[j]) / 2);
                        MoveToEx(hdc, (nx[i] + nx[j]) / 2 - OFFSET, (ny[i] + ny[j]) / 2, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow((nx[i] + nx[j]) / 2 - OFFSET, (ny[i] + ny[j]) / 2, nx[j], ny[j], dx, hdc);
                    }
                    else {
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow(nx[i], ny[i], nx[j], ny[j], dx, hdc);
                    }

                }
            }
        }
    }

    printf("Number of all lines: %d\n", summary);

    HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));

    SelectObject(hdc, BPen);
    for(int i = 0;i < N; i++){
        Ellipse(hdc, nx[i]-dx,ny[i]-dy,nx[i]+dx,ny[i]+dy);
        TextOut(hdc, nx[i]-dtx,ny[i]-dy/2, nn[i],2);
    }

    drawUndirectedGraph(hWnd, hdc, N, nn, nx, ny, A);
    //free memory
    for(int i = 0; i < N; i++){
        free(T[i]);
        free(A[i]);
    }
    free(T);
    free(A);
    free(nn);
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

    hWnd = CreateWindow(ProgName, "Lab 3 ASD", WS_OVERLAPPEDWINDOW, 100, 100, 1200, 700, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, (HINSTANCE)NULL);
    ShowWindow(hWnd, nCmdShow);
    while(GetMessage(&lpMsg, hWnd, 0, 0)){
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
    }

    return(lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    PAINTSTRUCT ps;
    switch(message){
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            drawGraph(hWnd, hdc);
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}