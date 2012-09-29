#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

/**
 * Kompiuterinės grafikos 1 užduotis. 
 * Tiesės brėžimo algoritmas (Bresenham’s line algorithm).
 * 
 * @author Aurelijus Banelis
 */

#define ESCAPE 27
#define CAMERA_WIDTH 2
#define CAMERA_HEIGHT 2

/*
 * Global variables
 */

float pointerX = 0;
float pointerY = 0;
int windowWidth = 500;
int windowHeight = 500;

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

/*
 * Render
 */

void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        // Camera as window
//        glLoadIdentity();               
//        glRotatef(180, 1, 0, 0); 
//        glTranslatef(-1, -1, 0);
        
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
        
        // Pointer
        glColor3f(0,1,1);
        glBegin(GL_QUADS);
            float size = 0.05;
            glVertex2f(pointerX-size,pointerY-size);
            glVertex2f(pointerX-size,pointerY+size);
            glVertex2f(pointerX+size,pointerY+size);
            glVertex2f(pointerX+size,pointerY-size);
        glEnd();

        glutSwapBuffers();
}


/*
 * Mouse
 */

void mouseClick(int button, int state, int x, int y) {
    printf("Click %d %d (%dx%d)\n", button, state, x, y);
}

void mouseMove(int x, int y) {
    to3dCoordinates(x, y, &pointerX, &pointerY);
    glutPostRedisplay();
    printf("Move (%dx%d)=(%f %f)\n", x, y, pointerX, pointerY);
}

void mouseDrag(int x, int y) {
    printf("Drag (%dx%d)\n", x, y);
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