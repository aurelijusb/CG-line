#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

/**
 * Kompiuterinės grafikos 1 užduotis. 
 * Tiesės brėžimo algoritmas (Bresenham’s line algorithm).
 * 
 * @author Aurelijus Banelis
 */

#define ESCAPE 27
#define BUTTON_LEFT 0
#define BUTTON_STATE_DOWN 0
#define CAMERA_WIDTH 2
#define CAMERA_HEIGHT 2

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 32


/*
 * Global variables
 */

float pointerX = 0;
float pointerY = 0;
int windowWidth = 500;
int windowHeight = 500;

enum {clear, first, last} state = clear;
float lx1, lx2, ly1, ly2 = 0;       // Line coordinates in 2D space
int matrixX1, matrixX2, matrixY1, matrixY2 = 0;         // Line coordinates in matrix
typedef enum { empty, filled } cell;
cell matrixFloat[MATRIX_WIDTH][MATRIX_HEIGHT];
cell matrixInteger[MATRIX_WIDTH][MATRIX_HEIGHT];


/*
 * Transformations
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

double max(int value1, int value2) {
    if (value1 > value2) {
        return value1;
    } else {
        return value2;
    }
}

double min(int value1, int value2) {
    if (value1 < value2) {
        return value1;
    } else {
        return value2;
    }
}

void updateMatrixFloat(cell matrix[MATRIX_WIDTH][MATRIX_HEIGHT]) {
    // Clear matrix
    clearMatrix(matrix);

    //Octets:     6  7
    //          5   .__8      
    //          4      1   
    //            3  2   

    // Changing octets
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

    // Line by octet
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    double error = 0;
    if (deltaX >= 0 && deltaY >= 0 && deltaX >= deltaY) {
        printf("1\n");
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
        printf("2\n");
        // 2nd octet
        double deltaErr = deltaX / (double) deltaY;
        int y;
        int x = min(x1, x2);
        for (y = y1; y <= y2; y++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                x++;
                error -= 1;
            }
        }
    } else if (deltaX <= 0 && deltaY > 0 && -deltaX <= deltaY) {
        printf("3\n");
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
        printf("4\n");
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

void updateMatrixInteger(cell matrix[MATRIX_WIDTH][MATRIX_HEIGHT]) {
    // Clear matrix
    clearMatrix(matrix);
    
    // Changing octets
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
    
    //Octets:     6  7
    //          5   .__8      
    //          4      1   
    //            3  2   
    
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


/*
 * Render
 */

void renderPointer() {
    glBegin(GL_QUADS);
        const float size = 0.05;
        glVertex2f(pointerX-size,pointerY-size);
        glVertex2f(pointerX-size,pointerY+size);
        glVertex2f(pointerX+size,pointerY+size);
        glVertex2f(pointerX+size,pointerY-size);
    glEnd();
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

void renderRealLine() {
    if (state != clear) {
        if (state == first) {
            lx2 = pointerX;
            ly2 = pointerY;
        }
        glColor3f(1,1,0);
        glBegin(GL_LINES);
            glVertex2f(lx1, ly1);
            glVertex2f(lx2, ly2);
        glEnd();
    }
}

void renderDebug() {
    // X
    glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);
        glVertex3f(1.0,0.0,0.0);
        glVertex3f(0.0,-0.3,0.0);
        glVertex3f(0.0,0.3,0.0);
    glEnd();

    // y
    glColor3f(0,1,0);
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0,1.0,0.0);
        glVertex3f(-0.3,0.0,0.0);
        glVertex3f(0.3,0.0,0.0);
    glEnd();

    // z
    glColor3f(0,0,1);
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0,0.0,1.0);
        glVertex3f(-0.3,0.0,0.0);
        glVertex3f(0.3,0.0,0.0);
    glEnd();    
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderDebug();
    
    // Matrix
    glColor4f(1, 0, 1, 0.5);
    renderMatrix(matrixFloat);
    glColor4f(0, 1, 1, 0.5);
    renderMatrix(matrixInteger);
    
    // Real line
    glColor3f(0,1,1);
    renderRealLine();

    // Pointer
    glColor3f(0,1,1);
//    renderPointer();
    
    glutSwapBuffers();
}


/*
 * Mouse
 */

void mouseClick(int button, int buttonState, int x, int y) {
    if (buttonState == BUTTON_STATE_DOWN) {
        switch (state) {
            case clear:
            case last:
                to3dCoordinates(x, y, &lx1, &ly1);
                toMatrixCoordinates(x, y, &matrixX1, &matrixY1);
                clearMatrix(matrixFloat);
                clearMatrix(matrixInteger);
                state = first;
            break;
            case first:
                //FIXME:
                to3dCoordinates(x, y, &lx2, &ly2);
                toMatrixCoordinates(x, y, &matrixX2, &matrixY2);
//                updateMatrix(matrix);
                state = last;
            break;
        }
    }
    glutPostRedisplay();
//    printf("Click %d %d (%dx%d) State = %d\n", button, state, x, y, state);
}

void mouseMove(int x, int y) {
    to3dCoordinates(x, y, &pointerX, &pointerY);
    
    //FIXME: after
    to3dCoordinates(x, y, &lx2, &ly2);
    toMatrixCoordinates(x, y, &matrixX2, &matrixY2);
    updateMatrixFloat(matrixFloat);
    updateMatrixInteger(matrixInteger);
    glutPostRedisplay();
}

void mouseDrag(int x, int y) {    
//    printf("Drag (%dx%d)\n", x, y);
}


/*
 * Kayboard
 */

void keyPressed(unsigned char key, int x, int y) {
    printf("Key %c (%dx%d)\n", key, x, y);
    switch(key) {
        case ESCAPE:
        exit(0); //glutLeaveMainLoop();
        break;
    }
    glutPostRedisplay();
}


/*
 * Menu
 */

void menu1(int value) {
    printf("Testas %d\n", value);
}

/*
 * Main
 */

void init() {
    // Menu
    glutCreateMenu(menu1);
    glutAddMenuEntry("Vienas", 1);
    glutAddMenuEntry("Du", 2);
    glutAddMenuEntry("Trys", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Mouse
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMove);
    glutMotionFunc(mouseDrag);
    
    // Keyboard
    glutKeyboardFunc(keyPressed);
    
    // Window
    glutReshapeFunc(saveWindowSize);
    clearMatrix(matrixFloat);
    clearMatrix(matrixInteger);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Camera
    gluLookAt ( 1, 1, 0,    //   y|/      y| /  /
                1, 1, 1,    // ---/---x    |/  V
                0,-1, 0 );  //  z/|       z/-----x
}

int main(int argc, char **argv) {

    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA);
    glutInitWindowPosition(1000,500);
    glutInitWindowSize(320,320);
    glutCreateWindow("U1");
    init();

    // register callbacks
    glutDisplayFunc(renderScene);

    // enter GLUT event processing cycle
    glutMainLoop();

    return 1;
}