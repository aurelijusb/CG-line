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


/*
 * Render
 */


void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        
        glutSwapBuffers();
}


/*
 * Mouse
 */

void mouseClick(int button, int state, int x, int y) {
    printf("Click %d %d (%dx%d)\n", button, state, x, y);
}

void mouseMove(int x, int y) {
    printf("Move (%dx%d)\n", x, y);
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
    
    // Camera
    gluLookAt ( 1, 1, 0,    //   y|/      y| /  /
                1, 1, 1,    // ---/---x    |/  V
                0,-1, 0 );  //  z/|       z/-----x
    
}

int main(int argc, char **argv) {

    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
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