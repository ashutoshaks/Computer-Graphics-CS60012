/*
Name: Ashutosh Kumar Singh
Roll No.: 19CS30008
Email: ashu11.03.2002@gmail.com
Assignment 1
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define SPHERE 'a'
#define CUBE 'b'
#define TORUS 'c'
#define CONE 'd'
#define DISC 'e'

#define ABOVE_PLANE 1
#define BELOW_PLANE -1

// Constants for the shape properties
const float ROTATION_DELTA = 5.0;   // The angle by which the shape is rotated each time using the left and right arrow keys
const float SCALE_DELTA = 0.1;      // The factor by which the shape is scaled each time using the up and down arrow keys
const float SCALE_THRESHOLD = 0.1;  // The scale factor at which the shape appears at the other side of the plane
const float SENSITIVITY = 0.4;      // The sensitivity of the mouse dragging motion, higher the value, greater the rotation

const float PLANE_SIDE_LEN = 2.0;      // The side length of the square plane
const float SPHERE_RADIUS = 0.25;      // The initial radius of the sphere
const float CUBE_SIDE_LEN = 0.3;       // The initial side length of the cube
const float TORUS_INNER_RADIUS = 0.1;  // The initial inner radius of the torus
const float TORUS_OUTER_RADIUS = 0.3;  // The initial outer radius of the torus
const float CONE_BASE_RADIUS = 0.3;    // The initial base radius of the cone
const float CONE_HEIGHT = 0.7;         // The initial height of the cone
const float DISC_INNER_RADIUS = 0.0;   // The initial inner radius of the disc
const float DISC_OUTER_RADIUS = 0.3;   // The initial outer radius of the disc
const int SLICES = 60;                 // The number of slices into which the sphere, torus, cone, and disc are subdivided around the z-axis
const int STACKS = 60;                 // The number of stacks into which the sphere, torus, cone, and disc are subdivided along the z-axis

const float EYE_X = 2.0;  // The x-coordinate of the eye position
const float EYE_Y = 1.3;  // The y-coordinate of the eye position
const float EYE_Z = 4.0;  // The z-coordinate of the eye position

// Global variables
char currentShape = SPHERE;     // The current shape to be drawn, initially a sphere
float rotationAngle = 0.0;      // The current rotation of the shape in the horizontal plane (modified using the left and right arrow keys)
float scale = 1.0;              // The current scale of the shape (modified using the up and down arrow keys)
float translationY = 1.0;       // A translation value along the y-axis, so that the shape appears above the plane
int orientation = ABOVE_PLANE;  // Indicator for whether the shape is above or below the plane, 1 for above, -1 for below

float rotationX = 0.0;             // Rotation about the x-axis, to allow the user to rotate the scene using the mouse
float rotationY = 0.0;             // Rotation about the y-axis, to allow the user to rotate the scene using the mouse
int isMouseLeftButtonPressed = 0;  // Indicator for whether the left mouse button is currently pressed

int prevMouseX = -1;  // The previous x-coordinate of the mouse, to calculate the change in the x-coordinate
int prevMouseY = -1;  // The previous y-coordinate of the mouse, to calculate the change in the y-coordinate

void drawHorizontalPlane(float side) {
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.5);
    glVertex3f(-side / 2, 0.0, -side / 2);
    glVertex3f(side / 2, 0.0, -side / 2);
    glVertex3f(side / 2, 0.0, side / 2);
    glVertex3f(-side / 2, 0.0, side / 2);
    glEnd();
    glEnable(GL_LIGHTING);
}

void applyStateTransformations() {
    glRotatef(rotationAngle, 0.0, 1.0, 0.0);
    glScalef(scale, scale, scale);
    glTranslatef(0.0, translationY * orientation, 0.0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == SPHERE || key == CUBE || key == TORUS || key == CONE || key == DISC) {
        currentShape = key;
        glutPostRedisplay();
    }
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            rotationAngle -= ROTATION_DELTA;
            break;
        case GLUT_KEY_RIGHT:
            rotationAngle += ROTATION_DELTA;
            break;
        case GLUT_KEY_UP:
            if (orientation == ABOVE_PLANE) {
                scale += SCALE_DELTA;
            } else {
                scale -= SCALE_DELTA;
                if (scale < SCALE_THRESHOLD) {
                    orientation *= -1;
                    scale = SCALE_THRESHOLD;
                }
            }
            break;
        case GLUT_KEY_DOWN:
            if (orientation == ABOVE_PLANE) {
                scale -= SCALE_DELTA;
                if (scale < SCALE_THRESHOLD) {
                    orientation *= -1;
                    scale = SCALE_THRESHOLD;
                }
            } else {
                scale += SCALE_DELTA;
            }
    }

    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isMouseLeftButtonPressed = 1;
            prevMouseX = x;
            prevMouseY = y;
        } else {
            isMouseLeftButtonPressed = 0;
        }
    }
}

void mouseMotion(int x, int y) {
    if (isMouseLeftButtonPressed) {
        rotationX += (y - prevMouseY) * SENSITIVITY;
        rotationY += (x - prevMouseX) * SENSITIVITY;

        if (rotationX > 90.0) {
            rotationX = 90.0;
        } else if (rotationX < -90.0) {
            rotationX = -90.0;
        }

        prevMouseX = x;
        prevMouseY = y;

        glutPostRedisplay();
    }
}

void drawShape(char currentShape) {
    switch (currentShape) {
        case SPHERE:
            glutSolidSphere(SPHERE_RADIUS, SLICES, STACKS);
            break;
        case CUBE:
            glutSolidCube(CUBE_SIDE_LEN);
            break;
        case TORUS:
            glutSolidTorus(TORUS_INNER_RADIUS, TORUS_OUTER_RADIUS, SLICES, STACKS);
            break;
        case CONE:
            glutSolidCone(CONE_BASE_RADIUS, CONE_HEIGHT, SLICES, STACKS);
            break;
        case DISC:
            gluDisk(gluNewQuadric(), DISC_INNER_RADIUS, DISC_OUTER_RADIUS, SLICES, STACKS);
            break;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(EYE_X, EYE_Y, EYE_Z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glRotatef(rotationY, 0.0, 1.0, 0.0);
    glRotatef(rotationX, 1.0, 0.0, 0.0);

    drawHorizontalPlane(PLANE_SIDE_LEN);

    glPushMatrix();
    applyStateTransformations();
    drawShape(currentShape);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (double)width / (double)height, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void initGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);  // Background color
    glEnable(GL_DEPTH_TEST);           // Required for 3D drawing, not usually for 2D.

    glEnable(GL_LIGHTING);  // Enable lighting.
    glEnable(GL_LIGHT0);    // Turn on a light. By default, shines from direction of viewer.

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    GLfloat ambientColor[] = {0.2, 0.2, 0.2, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);

    GLfloat diffuseColor[] = {0.8, 0.8, 0.8, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);

    GLfloat specularColor[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);

    GLfloat shapeColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, shapeColor);

    glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Plane and Shapes");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    initGL();
    glutMainLoop();
    return 0;
}
