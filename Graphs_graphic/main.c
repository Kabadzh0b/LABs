#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char ProgName[] = "Ëàáîðàòîðíà ðîáîòà 3";

void arrow(float fi, int px, int py, HDC hdc, int test) {
    int lx, ly, rx, ry;
    lx = px - 15 * cos(fi + 0.3);
    rx = px - 15 * cos(fi - 0.3);
    ly = py - 15 * sin(fi + 0.3);
    ry = py - 15 * sin(fi - 0.3);
    MoveToEx(hdc, px, py, NULL);
    LineTo(hdc, lx, ly);
    MoveToEx(hdc, px, py, NULL);
    LineTo(hdc, rx, ry);
    int Sexing=0;
    Sexing++;
    Sexing++;
    Sexing++;
    Sexing++;
}




void drawArrow(int fromx, int fromy, int tox, int toy, int r, HDC hdc){
    float angleRad = atan2((toy-fromy), (tox-fromx));
    float px = tox - r*cos(angleRad);
    float py = toy - r*sin(angleRad);
    arrow(angleRad, px, py, hdc, );
}

float** randm(int n) {
    srand(2113);
    float** arr = (float**)malloc(n * sizeof(float*));
    for(int i = 0; i < n; i++){
        arr[i] = (float*)malloc(n * sizeof(float));
        for(int j = 0; j < n; j++){
            arr[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0;
        }
    }
    return arr;
}

float** mulmr(float c, float** mat, int n) {
    float** res = (float**)malloc(n * sizeof(float*));
    for(int i = 0; i < n; i++){
        res[i] = (float*)malloc(n * sizeof(float));
        for(int j = 0; j < n; j++){
            res[i][j] = floor(mat[i][j] * c);
        }
    }
    return res;
}

char** symbolArray(int N){
    char** array = malloc(N * sizeof(char*));
    for(int i = 0; i < N; i++) {
        array[i] = malloc(3 * sizeof(char)); // allocate memory for each element
        sprintf(array[i], "%d", i+1); // use sprintf to convert int to string
    }
    return array;
}

void arrayX(int N, int* array){
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

void arrayY(int N, int* array){
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

void drawUnDependenceGraph(HWND hWnd, HDC hdc, int n, char** nn, int nx[], int ny[], float** A){
    int edgeCeil = ceil(n / 4.0);//Number of vertex, that we can draw four time to get squer
    int step = (edgeCeil + 2) * 100;// Distance on which we replace our vertex (in OX)
    printf("X coordinates for undirected graph: ");
    for(int i = 0; i < n; i++){//Change and output coordinates of vertexes for X to draw Undependence graph
        nx[i] = nx[i] + step;
        printf("%d ", nx[i]);
    }
    printf("\n");

    printf("Undirected A matrix:\n");//Output our matrix of dependencies
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(A[j][i] == 1 || A[i][j] == 1){
                printf("1 ");
            } else printf("%.0f ", A[i][j]);
        }
        printf("\n");
    }

    int dx = 16, dy = 16, dtx = 5;
    HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));
    SelectObject(hdc, KPen);

    for(int i = 0; i < n; i++){//For ellipses
        for(int j = 0; j < n; j++){
            if(A[i][j] == 1){
                if(i == j){
                    int dir = (int) ceil((i+1)/(float) edgeCeil);
                    if(dir%2 == 0){
                        if(dir > edgeCeil){
                            Ellipse(hdc, nx[i]-40, ny[i]-20, nx[i], ny[i]+20);
                        } else{
                            Ellipse(hdc, nx[i]+40, ny[i]-20, nx[i], ny[i]+20);
                        }
                    } else{
                        if(dir >= edgeCeil){
                            Ellipse(hdc, nx[i]-20, ny[i]+40, nx[i]+20, ny[i]);
                        } else{
                            Ellipse(hdc, nx[i]-20, ny[i]-40, nx[i]+20, ny[i]);
                        }
                    }
                }
            }
        }
    }

    for(int i = 0; i < n; i++){//For lines when circles are on the same row in X or Y
        for(int j = 0; j < n; j++){
            if(A[i][j] == 1 && abs(i-j) >=2 && abs(i-j) <= edgeCeil && (nx[i] == nx[j] || ny[i] == ny[j])){
                if(nx[i] == nx[j]){
                    MoveToEx(hdc, nx[i], ny[i], NULL);
                    LineTo(hdc, nx[j]+35, ny[i]-(ny[i]-ny[j])/2);
                    MoveToEx(hdc, nx[j]+35, ny[i]-(ny[i]-ny[j])/2, NULL);
                    LineTo(hdc, nx[j], ny[j]);
                } else{
                    MoveToEx(hdc, nx[i], ny[i], NULL);
                    LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-35);
                    MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-35, NULL);
                    LineTo(hdc, nx[j], ny[j]);
                }
            }
        }
    }

    for(int i = 0; i < n; i++){//For lines between vertex
        for(int j = 0; j < n; j++){
            if(A[i][j] == 1){
                if(abs(i-j) >=2 && abs(i-j) <= edgeCeil && (nx[i] == nx[j] || ny[i] == ny[j])){
                } else if(i == j){
                } else{
                    MoveToEx(hdc, nx[i], ny[i], NULL);
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
    const int N = 11;//Number of our vertex
    int edgeCeil = ceil(N / 4.0);//Number of vertex, that we can draw four time to get squer
    int nx[N], ny[N];
    char** nn = symbolArray(N);
    arrayX(N, nx);
    arrayY(N, ny);
    printf("Vertex`s name: ");
    for(int i = 0; i < N; i++){//Output names of our vertexes
        printf("%s ", nn[i]);
    }
    printf("%\nX coordinates: ");
    for(int i = 0; i < N; i++){//Output coordinates of vertexes for X
        printf("%d ", nx[i]);
    }
    printf("%\nY coordinates");
    for(int i = 0; i < N; i++){//Output coordinates of vertexes for Y
        printf("%d ", ny[i]);
    }
    printf("\n");
    int dx = 16, dy = 16, dtx = 5;

    float** T = randm(N);
    float** A = mulmr(0.715, T, N);//Fill our matrix

    printf("T:\n");//Output our start random matrix with value from 0 to 2.0
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.2f ", T[i][j]);
        }
        printf("\n");
    }

    printf("A:\n");//Output our matrix of dependencies
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%.0f ", A[i][j]);
        }
        printf("\n");
    }

    for(int i = 0; i < N; i++){//For ellipses
        for(int j = 0; j < N; j++){
            if(A[i][j] == 1){
                if(i == j){
                    int dir = (int) ceil((i+1)/(float) edgeCeil);
                    const int SDVIG_PO_X = 40;
                    if(dir%2 == 0){
                        if(dir > edgeCeil){
                            Ellipse(hdc, nx[i]-40, ny[i]-20, nx[i], ny[i]+20);
                            drawArrow(nx[i]-35, ny[i]+50, nx[j], ny[j], dx, hdc);
                        } else{
                            Ellipse(hdc, nx[i]+40, ny[i]-20, nx[i], ny[i]+20);
                            drawArrow(nx[i]+35, ny[i]-50, nx[j], ny[j], dx, hdc);
                        }
                    } else{
                        if(dir >= edgeCeil){
                            Ellipse(hdc, nx[i]-20, ny[i]+40, nx[i]+20, ny[i]);
                            drawArrow(nx[i]+70, ny[i]+45, nx[j], ny[j], dx, hdc);
                        } else{
                            Ellipse(hdc, nx[i]-20, ny[i]-40, nx[i]+20, ny[i]);
                            drawArrow(nx[i]-70, ny[i]-45, nx[j], ny[j], dx, hdc);
                        }
                    }
                }
            }
        }
    }

    for(int i = 0; i < N; i++){//For lines when circles are on the same row in X or Y
        for(int j = 0; j < N; j++){
            if(A[i][j] == 1 && abs(i-j) >=2  && (nx[i] == nx[j] || ny[i] == ny[j])){
                if(nx[i] == nx[j]){
                    if(i > j){
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, nx[j]+35, ny[i]-(ny[i]-ny[j])/2);
                        MoveToEx(hdc, nx[j]+35, ny[i]-(ny[i]-ny[j])/2, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow(nx[j]+35, ny[i]-(ny[i]-ny[j])/2, nx[j], ny[j], dx, hdc);
                    } else{
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, nx[j]-35, ny[i]-(ny[i]-ny[j])/2);
                        MoveToEx(hdc, nx[j]-35, ny[i]-(ny[i]-ny[j])/2, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow(nx[j]-35, ny[i]-(ny[i]-ny[j])/2, nx[j], ny[j], dx, hdc);
                    }
                } else{
                    if(i > j){
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]+35);
                        MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]+35, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow(nx[j]+(nx[i]-nx[j])/2, ny[i]+35, nx[j], ny[j], dx, hdc);

                    } else{
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-35);
                        MoveToEx(hdc, nx[j]+(nx[i]-nx[j])/2, ny[i]-35, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow(nx[j]+(nx[i]-nx[j])/2, ny[i]-35, nx[j], ny[j], dx, hdc);
                    }
                }
            }
        }
    }

    int atall = 0;
    for(int i = 0; i < N; i++){//For lines between vertex
        for(int j = 0; j < N; j++){
            if(A[i][j] == 1){
                atall++;
                if(abs(i-j) >=2 && abs(i-j) <= edgeCeil && (nx[i] == nx[j] || ny[i] == ny[j]) && i == j){

                } else if(i == j){

                } else{

                    if(i > j && A[j][i] == 1){
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, (nx[i]+nx[j])/2+20, (ny[i]+ny[j])/2);
                        MoveToEx(hdc, (nx[i]+nx[j])/2+20, (ny[i]+ny[j])/2, NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow((nx[i]+nx[j])/2+20, (ny[i]+ny[j])/2, nx[j], ny[j], dx, hdc);

                    } else{
                        MoveToEx(hdc, nx[i], ny[i], NULL);
                        LineTo(hdc, nx[j], ny[j]);
                        drawArrow(nx[i], ny[i], nx[j], ny[j], dx, hdc);
                    }
                }
            }
        }
    }

    printf("Number of all dependences: %d\n", atall);

    HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
    HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));

    SelectObject(hdc, BPen);
    for(int i = 0;i < N; i++){
        Ellipse(hdc, nx[i]-dx,ny[i]-dy,nx[i]+dx,ny[i]+dy);
        TextOut(hdc, nx[i]-dtx,ny[i]-dy/2, nn[i],2);
    }

    drawUnDependenceGraph(hWnd, hdc, N, nn, nx, ny, A);

    for(int i = 0; i < N; i++){
        free(T[i]);
        free(A[i]);
    }
    free(T);//To avoid problems with dynamic memory we free out matrix in the end of our programme
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

    hWnd = CreateWindow(ProgName, "Ëàáîðàòîðíà ðîáîòà 3. Âèêîíàâ Ä. Ì. Ëåñüêî", WS_OVERLAPPEDWINDOW, 100, 100, 1200, 700, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, (HINSTANCE)NULL);
    ShowWindow(hWnd, nCmdShow);
    while(GetMessage(&lpMsg, hWnd, 0, 0)){
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
    }

    return(lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    PAINTSTRUCT ps;
    switch(messg){
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            drawGraph(hWnd, hdc);
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, messg, wParam, lParam);
    }
    return 0;
}