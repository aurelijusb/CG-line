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
enum { empty, filled } matrix[MATRIX_WIDTH][MATRIX_HEIGHT];


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

void clearMatrix() {
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

void updateMatrix() {
    // Clear matrix
    clearMatrix();
    
    //Octets:     6  7
    //          5   .__8      
    //          4      1   
    //            3  2   

    int deltaX = matrixX2 - matrixX1;
    int deltaY = matrixY2 - matrixY1;   
    double error = 0;
    if (deltaX >= 0 && deltaY >= 0 && deltaX >= deltaY) {
        // 1st octet
        double deltaErr = deltaY / (double) deltaX;
        int y = matrixY1;
        int x;
        for (x = matrixX1; x <= matrixX2; x++) {
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
        int x = matrixX1;
        for (y = matrixY1; y <= matrixY2; y++) {
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
        int x = matrixX1;
        for (y = matrixY1; y <= matrixY2; y++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                x--;
                error -= 1;
            }
        }
    } else if (deltaX < 0 && deltaY > 0 && -deltaX > deltaY) {
        // 4th octet
        double deltaErr = -deltaY / (double) deltaX;
        int y = matrixY1;
        int x;
        for (x = matrixX1; x >= matrixX2; x--) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                y++;
                error -= 1;
            }
        }
    } else if (deltaX < 0 && deltaY <= 0 && -deltaX >= -deltaY) {
        // 5th octet
        int x1 = matrixX2;
        int x2 = matrixX1;
        int y1 = matrixY2;
        int y2 = matrixY1;
        // Like 1st octet
        double deltaErr = (y2 - y1) / (double) (x2 - x1);
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
    } else if (deltaX <= 0 && deltaY < 0 && -deltaX < -deltaY) {
        // 6th octet
        int x1 = matrixX2;
        int x2 = matrixX1;
        int y1 = matrixY2;
        int y2 = matrixY1;
        // Like 2st octet
        double deltaErr = (x2 - x1) / (double) (y2 - y1);
        int x = x1;
        int y;
        for (y = y1; y <= y2; y++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                x++;
                error -= 1;
            }
        }
    } else if (deltaX > 0 && deltaY < 0 && deltaX <= -deltaY) {
        // 7th octet
        int x1 = matrixX2;
        int x2 = matrixX1;
        int y1 = matrixY2;
        int y2 = matrixY1;
        // Like 3st octet
        double deltaErr = -(x2 - x1) / (double) (y2 - y1);
        int x = x1;
        int y;
        for (y = y1; y <= y2; y++) {
            matrix[x][y] = filled;
            error = error + deltaErr;
            if (error >= 0.5) {
                x--;
                error -= 1;
            }
        }
    } else if (deltaX > 0 && deltaY < 0 && deltaX > -deltaY) {
        // 8th octet
        int x1 = matrixX2;
        int x2 = matrixX1;
        int y1 = matrixY2;
        int y2 = matrixY1;
        // Like 4st octet
        double deltaErr = -(y2 - y1) / (double) (x2 - x1);
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

void renderMatrix() {
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
    glColor3f(0.5, 0, 0.5);
    renderMatrix();
    
    // Real line
    glColor3f(0,1,1);
    renderRealLine();

    // Pointer
    glColor3f(0,1,1);
    renderPointer();
    
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
                clearMatrix();
                state = first;
            break;
            case first:
                to3dCoordinates(x, y, &lx2, &ly2);
                toMatrixCoordinates(x, y, &matrixX2, &matrixY2);
                updateMatrix();
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
    updateMatrix();

    glutPostRedisplay();
    
//    printf("Move (%dx%d)=(%f %f)\n", x, y, pointerX, pointerY);
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
    clearMatrix();
    
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