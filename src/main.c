/**
 * Kompiuterinės grafikos 1 užduotis. 
 * Tiesės brėžimo algoritmas (Bresenham’s line algorithm).
 * 
 * @author Aurelijus Banelis
 */

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define KEY_ESCAPE 27
#define BUTTON_LEFT 0
#define BUTTON_STATE_DOWN 0
#define COMMAND_QUIT 0
#define COMMAND_CLEAR 1
#define COMMAND_BENCHMARK 2
#define CAMERA_WIDTH 2
#define CAMERA_HEIGHT 2
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 32

/*
 * Global variables
 */

// Variables for window and OpenGl position releaton
float pointerX, pointerY = 0;                   // Mouse position in GL
int windowWidth, windowHeight = 500;            // Window size

// Variables for line approximation
enum {clear, first, last} state = clear;        // GUI state
float lineX1, lineX2, lineY1, lineY2 = 0;       // Precise line coordinates
int matrixX1, matrixX2, matrixY1, matrixY2 = 0; // Approximated line coordinates
typedef enum { empty, filled } cell;
cell matrixFloat[MATRIX_WIDTH][MATRIX_HEIGHT];  // Float approximation
cell matrixInteger[MATRIX_WIDTH][MATRIX_HEIGHT];// Integer approximation
char statusText[256];
int statusTextLen = 0;

/*
 * Window and OpenGL coordinates realtion
 */

void saveWindowSize(int width, int height) {
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
    glutPostRedisplay();
}

void to3dCoordinates(int windowX, int windowY, float* x, float* y) {
    *x = windowX / (float) windowWidth * CAMERA_WIDTH;
    *y = windowY / (float) windowHeight * CAMERA_HEIGHT;
}

void toMatrixCoordinates(int windowX, int windowY, int* x, int* y) {
    *x = (int) (windowX / (float) windowWidth * MATRIX_WIDTH);
    *y = (int) (windowY / (float) windowHeight * MATRIX_HEIGHT);
}


/*
 * Approximation
 */

void clearMatrix(cell matrix[MATRIX_WIDTH][MATRIX_HEIGHT]) {
    int x, y;
    for (x = 0; x < MATRIX_WIDTH; x++) {
        for (y = 0; y < MATRIX_HEIGHT; y++) {
            matrix[x][y] = empty;
        }
    }
}

void updateMatrixFloat(cell matrix[MATRIX_WIDTH][MATRIX_HEIGHT],
                       int matrixX1, int matrixY1, int matrixX2, int matrixY2) {
    //Octets:     6  7
    //          5   .__8      
    //          4      1   
    //            3  2   

    // Mirror octets
    int x1, x2, y1, y2;
    if (matrixY1 < matrixY2) {
        x1 = matrixX1;
        x2 = matrixX2;
        y1 = matrixY1;
        y2 = matrixY2;
    } else {
        x1 = matrixX2;
        x2 = matrixX1;
        y1 = matrixY2;
        y2 = matrixY1;
    }

    // Line approximation by octet
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    double error = 0;
    if (deltaX >= 0 && deltaY >= 0 && deltaX >= deltaY) {
        // 1st octet
        double deltaErr = deltaY / (double) deltaX;
        int y = y1;
        int x;
        for (x = x1; x <= x2; x++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                y++;
                error -= 1;
            }
        }
    } else if (deltaX > 0 && deltaY >= 0 && deltaX < deltaY) {
        // 2nd octet
        double deltaErr = deltaX / (double) deltaY;
        int y;
        int x = x1;
        for (y = y1; y <= y2; y++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                x++;
                error -= 1;
            }
        }
    } else if (deltaX <= 0 && deltaY > 0 && -deltaX <= deltaY) {
        // 3rd octet
        double deltaErr = -deltaX / (double) deltaY;
        int y;
        int x = x1;
        for (y = y1; y <= y2; y++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                x--;
                error -= 1;
            }
        }
    } else if (deltaX <= 0 && deltaY >= 0 && -deltaX > deltaY) {
        // 4th octet
        double deltaErr = -deltaY / (double) deltaX;
        int y = y1;
        int x;
        for (x = x1; x >= x2; x--) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                y++;
                error -= 1;
            }
        }
    }
}

void updateMatrixInteger(cell matrix[MATRIX_WIDTH][MATRIX_HEIGHT], int matrixX1,
                         int matrixY1, int matrixX2, int matrixY2) {
    //Octets:     6  7
    //          5   .__8      
    //          4      1   
    //            3  2   

    // Mirror octets
    int x1, x2, y1, y2;
    if (matrixY1 < matrixY2) {
        x1 = matrixX1;
        x2 = matrixX2;
        y1 = matrixY1;
        y2 = matrixY2;
    } else {
        x1 = matrixX2;
        x2 = matrixX1;
        y1 = matrixY2;
        y2 = matrixY1;
    }
    
    // Line approximation by octet
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx >= 0 && dy >= 0 && dx >= dy) {
        // 1st octet
        int D = 2*dy - dx;
        matrix[x1][y1] = filled;
        int y = y1;

        int x;
        for (x = x1+1; x <= x2; x++) {
            if (D > 0) {
                y = y + 1;
                matrix[x][y] = filled;
                D = D + (2*dy - 2*dx);
            } else {
                matrix[x][y] = filled;
                D = D + (2*dy);
            }
        }
    } else if (dx > 0 && dy >= 0 && dx < dy) {
        // 2nd octet
        int D = 2*dx - dy;
        matrix[x1][y1] = filled;
        int x = x1;

        int y;
        for (y = y1+1; y <= y2; y++) {
            if (D > 0) {
                x = x + 1;
                matrix[x][y] = filled;
                D = D + (2*dx - 2*dy);
            } else {
                matrix[x][y] = filled;
                D = D + (2*dx);
            }
        }
    } else if (dx <= 0 && dy > 0 && -dx <= dy) {
        // 3rd octet
        int D = -2*dx - dy;
        matrix[x1][y1] = filled;
        int x = x1;

        int y;
        for (y = y1+1; y <= y2; y++) {
            if (D > 0) {
                x = x - 1;
                matrix[x][y] = filled;
                D = D - 2*dx - 2*dy;
            } else {
                matrix[x][y] = filled;
                D = D - 2*dx;
            }
        }
    } else if (dx <= 0 && dy >= 0 && -dx > dy) {
        // 4th octet
        int D = 2*dy + dx;
        matrix[x2][y2] = filled;
        int y = y2;

        int x;
        for (x = x2+1; x <= x1; x++) {
            if (D > 0) {
                y = y - 1;
                matrix[x][y] = filled;
                D = D + (2*dy + 2*dx);
            } else {
                matrix[x][y] = filled;
                D = D + 2*dy;
            }
        }
    }
}

void renderBox(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
        glVertex2f(x, y);               //  0------3
        glVertex2f(x, y+height);        //  :      :
        glVertex2f(x+width, y+height);  //  :      :
        glVertex2f(x+width,y);          //  1------2
    glEnd();    
}

void renderMatrix(cell matrix[MATRIX_WIDTH][MATRIX_HEIGHT]) {
    float stepWidth = CAMERA_WIDTH / (float) MATRIX_WIDTH;
    float stepHeight = CAMERA_HEIGHT / (float) MATRIX_HEIGHT;
    int x, y;
    for (x = 0; x < MATRIX_WIDTH; x++) {
        for (y = 0; y < MATRIX_HEIGHT; y++) {
            if (matrix[x][y] == filled) {
                renderBox(x*stepWidth, y*stepWidth, stepWidth, stepHeight);
            }
        }
    }
}


/*
 * Pointer and real line
 */

void renderPointer() {
    glBegin(GL_QUADS);
        const float size = 0.05;
        glVertex2f(pointerX,pointerY-size);
        glVertex2f(pointerX-size,pointerY);
        glVertex2f(pointerX,pointerY+size);
        glVertex2f(pointerX+size,pointerY);
    glEnd();
}

void renderRealLine() {
    if (state != clear) {
        if (state == first) {
            lineX2 = pointerX;
            lineY2 = pointerY;
        }
        glColor3f(1,1,0);
        glBegin(GL_LINES);
            glVertex2f(lineX1, lineY1);
            glVertex2f(lineX2, lineY2);
        glEnd();
    }
}

void onMouseClick(int button, int buttonState, int x, int y) {
    if (buttonState == BUTTON_STATE_DOWN) {
        switch (state) {
            case clear:
            case last:
                // Starting line
                to3dCoordinates(x, y, &lineX1, &lineY1);
                toMatrixCoordinates(x, y, &matrixX1, &matrixY1);
                clearMatrix(matrixFloat);
                clearMatrix(matrixInteger);
                state = first;
            break;
            case first:
                // Finishing line
                to3dCoordinates(x, y, &lineX2, &lineY2);
                state = last;
            break;
        }
    }
    statusTextLen = 0;
    glutPostRedisplay();
}

void onMouseMove(int x, int y) {
    if (state == first) {
        // Updating approximation
        to3dCoordinates(x, y, &lineX2, &lineY2);
        toMatrixCoordinates(x, y, &matrixX2, &matrixY2);
        clearMatrix(matrixInteger);
        clearMatrix(matrixFloat);
        updateMatrixFloat(matrixFloat, matrixX1, matrixY1, matrixX2, matrixY2);
        updateMatrixInteger(matrixInteger, matrixX1, matrixY1, matrixX2,
                            matrixY2);
    }

    // Updating precise line
    to3dCoordinates(x, y, &pointerX, &pointerY);    
    glutPostRedisplay();
}


/*
 * Benchmarking
 */

void benchmarkAlgorythms() {
    const int repeat = 2000;
    int n = 10000;
    short rands[n][4];
    int i, r = 0;
    for (i = 0; i < n; i++) {
        rands[i][0] = rand() % MATRIX_WIDTH;
        rands[i][1] = rand() % MATRIX_HEIGHT;
        rands[i][2] = rand() % MATRIX_WIDTH;
        rands[i][3] = rand() % MATRIX_HEIGHT;
    }
    
    clock_t start = clock();    
    for (r = 0; r < repeat; r++) {
        for (i = 0; i < n; i++) {
            updateMatrixFloat(matrixFloat, rands[i][0], rands[i][1],
                                           rands[i][2], rands[i][3]);
        }
    }

    clock_t finish1 = clock();    
    for (r = 0; r < repeat; r++) {
        for (i = 0; i < n; i++) {
            updateMatrixInteger(matrixFloat, rands[i][0], rands[i][1],
                                           rands[i][2], rands[i][3]);
        }
    }
    clock_t finish2 = clock();
    
    double time1 = (finish1-start) / (double) CLOCKS_PER_SEC;
    double time2 = (finish2-finish1) / (double) CLOCKS_PER_SEC;
    
    statusTextLen = sprintf(statusText,
                    "(santykis: %f) Slankaus: %f Sveiko: %f s. kartu %d",
                    time2 / time1, time1, time2, repeat * n);
    printf("%s\n", statusText);
    glutPostRedisplay();
}

void renderString(float x, float y, char *string, int len) {
    glRasterPos2f(x, y);
    int i;
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}

void onMenu(int command) {
    if (command == COMMAND_CLEAR) {
        state = clear;
        clearMatrix(matrixFloat);
        clearMatrix(matrixInteger);
        statusTextLen = 0;
        glutPostRedisplay();
    } else if (command == COMMAND_BENCHMARK) {
        benchmarkAlgorythms();
    } else if (command == COMMAND_QUIT) {
        exit(0); // glutLeaveMainLoop();
    }
}

/*
 * Main rendering and routines
 */

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrix
        glColor4f(0.3, 1, 0.3, 0.5);
        renderMatrix(matrixFloat);
        glColor4f(0, 0, 1, 0.5);
        renderMatrix(matrixInteger);

        // Real line
        glColor3f(1,0,0);
        renderRealLine();

        // Pointer
        glColor4f(1,1,0, 0.3);
        renderPointer();
    
        // Status text
        if (statusTextLen > 0) {
            glColor3f(1,1,1);
            renderString(0, 0.1, statusText, statusTextLen);
        }
        
    glutSwapBuffers();
}

void init() {   
    // Mouse
    glutMouseFunc(onMouseClick);
    glutPassiveMotionFunc(onMouseMove);
        
    // Window and colors
    glutReshapeFunc(saveWindowSize);
    clearMatrix(matrixFloat);
    clearMatrix(matrixInteger);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Menu
    glutCreateMenu(onMenu);
    glutAddMenuEntry("Atstatyti", COMMAND_CLEAR);
    glutAddMenuEntry("Palyginti algoritmus", COMMAND_BENCHMARK);
    glutAddMenuEntry("Baigti", COMMAND_QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Camera
    gluLookAt ( 1, 1, 0,    //   y|/      y| /  /
                1, 1, 1,    // ---/---x    |/  V
                0,-1, 0 );  //  z/|       z/-----x
}

int main(int argc, char **argv) {
    // Init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA);
    glutInitWindowPosition(1000,500);
    glutInitWindowSize(320,320);
    glutCreateWindow("U1");
    init();

    // Register callbacks
    glutDisplayFunc(renderScene);

    // Enter GLUT event processing cycle
    glutMainLoop();

    return 1;
}