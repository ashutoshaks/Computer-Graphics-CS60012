/*
Name: Ashutosh Kumar Singh
Roll No.: 19CS30008
Email: ashu11.03.2002@gmail.com
Assignment 2
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <vector>

using namespace std;

const int SIDE = 40;                         // The side length of the grid layout on which buildings are placed
const float CELL_SIDE = 1.0;                 // The side length of each cell in the imaginary grid layout
const float BUILDING_MAX_BASE_LENGTH = 0.6;  // The maximum base length/width of a building
const float BUILDING_MIN_BASE_LENGTH = 0.3;  // The minimum base length/width of a building

const float TALL_BUILDING_PROBABILITY = 0.4;
const float SHORT_BUILDING_PROBABILITY = 0.5;
const float SENSITIVITY = 0.001;  // The sensitivity of the mouse dragging motion, higher the value, greater the rotation

bool isMouseLeftButtonPressed = false;  // Indicator for whether the left mouse button is currently pressed
int prevMouseX = -1;                    // The previous x-coordinate of the mouse, to calculate the change in the x-coordinate
int prevMouseY = -1;                    // The previous y-coordinate of the mouse, to calculate the change in the y-coordinate

float cameraX = 25.0;  // The x-coordinate of the camera
float cameraY = 22.0;  // The y-coordinate of the camera
float cameraZ = 25.0;  // The z-coordinate of the camera

float targetX = -cameraX;  // The x-coordinate of the direction vector in which the camera is looking
float targetY = -cameraY;  // The y-coordinate of the direction vector in which the camera is looking
float targetZ = -cameraZ;  // The z-coordinate of the direction vector in which the camera is looking

float cameraSpeed = 0.2;  // The speed at which the camera moves

// The struct to store the details of a building
typedef struct {
    float base_x, base_z;        // The x and z coordinates of the center of the base of the building, the y-coordinate is 0
    float width, height, depth;  // The width, height and depth of the building
    float r, g, b;               // The color of the building
} Building;

// The vector to store the details of all the buildings
vector<Building> buildings;

// The struct to store the details of a region in the imaginary grid layout
typedef struct {
    int side_start, side_end;       // The start and end indices of the side of the region
    float height_low, height_high;  // The minimum and maximum height of the buildings in the region
} Region;

/**
 * @brief Checks if a cell at (i, j) is in a given region
 *
 * @param i The x or row index of the cell
 * @param j The y or row index of the cell
 * @param r The region to check if the cell is in
 * @return true If the cell is in the region
 * @return false If the cell is not in the region
 */
bool inRegion(int i, int j, Region r) {
    return i >= r.side_start && i < r.side_end && j >= r.side_start && j < r.side_end;
}

/**
 * @brief Generates a random float between low and high
 *
 * @param low The lower limit of the random float
 * @param high The upper limit of the random float
 * @return float The random float between low and high
 */
float generateRandomFloat(float low, float high) {
    return low + (float)rand() / RAND_MAX * (high - low);
}

/**
 * @brief Generates the buildings in the city.
 * We consider a grid layout of SIDE x SIDE cells, and place buildings in each cell, probabilistically.
 * The width and depth of each building is a random float between BUILDING_MIN_BASE_LENGTH and BUILDING_MAX_BASE_LENGTH.
 * The height of each building is a random float between the height_low and height_high of the region it is in.
 * The color of each building is a random RGB color.
 * We ensure that buildings towards the center are taller.
 *
 */
void generateBuildings() {
    // Define the regions in the grid layout
    Region r1 = {15, 25, 8.0, 10.0}, r2 = {10, 30, 3.0, 4.0}, r3 = {5, 35, 2.0, 3.0}, r4 = {0, 40, 1.0, 2.0};

    // Generate buildings in the cells of the imaginary grid layout
    for (int i = 0; i < SIDE; i++) {
        for (int j = 0; j < SIDE; j++) {
            if ((i + j) % 2 != 0) {  // Skip alternate cells to make the city look more realistic
                continue;
            }

            int ind = i * SIDE + j;
            Building building;
            building.base_x = -SIDE / 2 + (i * CELL_SIDE) + CELL_SIDE / 2.0;
            building.base_z = -SIDE / 2 + (j * CELL_SIDE) + CELL_SIDE / 2.0;
            building.width = generateRandomFloat(BUILDING_MIN_BASE_LENGTH, BUILDING_MAX_BASE_LENGTH);
            building.depth = generateRandomFloat(BUILDING_MIN_BASE_LENGTH, BUILDING_MAX_BASE_LENGTH);

            bool valid = false;  // Whether a building should be placed or not
            if ((float)rand() / RAND_MAX < SHORT_BUILDING_PROBABILITY) {
                valid = true;
            }

            if (inRegion(i, j, r1)) {
                valid = false;
                // We assign a smaller probability to place a tall building in the center region to avoid congestion
                if ((float)rand() / RAND_MAX < TALL_BUILDING_PROBABILITY) {
                    valid = true;
                }
                building.height = generateRandomFloat(r1.height_low, r1.height_high);
            } else if (inRegion(i, j, r2)) {
                building.height = generateRandomFloat(r2.height_low, r2.height_high);
            } else if (inRegion(i, j, r3)) {
                building.height = generateRandomFloat(r3.height_low, r3.height_high);
            } else {
                building.height = generateRandomFloat(r4.height_low, r4.height_high);
            }

            building.r = (float)rand() / RAND_MAX;
            building.g = (float)rand() / RAND_MAX;
            building.b = (float)rand() / RAND_MAX;

            if (valid) {
                buildings.push_back(building);
            }
        }
    }
}

/**
 * @brief Callback function for the keyboard, which is called when a key is pressed.
 * The function changes the camera position based on the key pressed.
 * If 'F' or 'f' is pressed, the camera moves forward in the direction it is looking.
 * If 'B' or 'b' is pressed, the camera moves backward in the direction it is looking.
 *
 * @param key The key pressed
 * @param x The x-coordinate of the mouse when the key was pressed
 * @param y The y-coordinate of the mouse when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'F':
        case 'f':
            cameraX += targetX * cameraSpeed;
            cameraY += targetY * cameraSpeed;
            cameraZ += targetZ * cameraSpeed;
            break;
        case 'B':
        case 'b':
            cameraX -= targetX * cameraSpeed;
            cameraY -= targetY * cameraSpeed;
            cameraZ -= targetZ * cameraSpeed;
            break;
    }
    glutPostRedisplay();
}

/**
 * @brief Callback function for the mouse button, which is called when a mouse button is pressed or released.
 *
 * @param button The mouse button pressed or released
 * @param state The state of the button, either GLUT_DOWN or GLUT_UP
 * @param x The x-coordinate of the mouse when the button was pressed or released
 * @param y The y-coordinate of the mouse when the button was pressed or released
 */
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // If the left mouse button is pressed, set the indicator to 1 and store the current mouse coordinates
            isMouseLeftButtonPressed = true;
            prevMouseX = x;
            prevMouseY = y;
        } else {
            isMouseLeftButtonPressed = false;
        }
    }
}

/**
 * @brief Callback function for the mouse motion, which is called when the mouse is dragged.
 * The function changes the direction the camera is looking in, based on the change in the x and y coordinates of the mouse.
 *
 * @param x The x-coordinate of the mouse when it was dragged
 * @param y The y-coordinate of the mouse when it was dragged
 */
void mouseMotion(int x, int y) {
    if (isMouseLeftButtonPressed) {
        float dx = x - prevMouseX;
        float dy = y - prevMouseY;

        // Modify the direction vector in which the camera is looking based on the change in the x and y coordinates of the mouse
        float theta = -SENSITIVITY * dx;
        targetX = targetX * cos(theta) - targetZ * sin(theta);
        targetZ = targetX * sin(theta) + targetZ * cos(theta);

        float theta1 = acos(targetY / sqrt(targetX * targetX + targetY * targetY + targetZ * targetZ));
        float theta2 = theta1 - SENSITIVITY * dy;
        targetY = targetY * (cos(theta2) / cos(theta1));
        float k = sqrt((1 - targetY * targetY) / (targetX * targetX + targetZ * targetZ));
        targetX *= k;
        targetZ *= k;

        prevMouseX = x;
        prevMouseY = y;

        glutPostRedisplay();
    }
}

/**
 * @brief Initializes the OpenGL settings, such as the background color, depth testing, lighting, and shading.
 *
 */
void initGL() {
    glClearColor(0.0, 0.0, 0.0, 1.0);  // Set the background color to black
    glEnable(GL_DEPTH_TEST);           // Enable depth testing for z-buffering

    glEnable(GL_LIGHTING);  // Enable lighting
    glEnable(GL_LIGHT0);    // Turn on a light, by default, it shines from the direction of the viewer

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);  // Enable two-sided lighting

    GLfloat ambientColor[] = {0.2, 0.2, 0.2, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);  // Set the ambient light color

    GLfloat diffuseColor[] = {0.8, 0.8, 0.8, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);  // Set the diffuse light color

    GLfloat specularColor[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);  // Set the specular light color

    glShadeModel(GL_SMOOTH);  // Enable smooth shading
}

/**
 * @brief Draws a building on the xz-plane with the given colour, width, height, and depth.
 *
 * @param b
 */
void drawBuilding(Building b) {
    GLfloat shapeColor[] = {b.r, b.g, b.b, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, shapeColor);
    glPushMatrix();
    glTranslatef(b.base_x, b.height / 2, b.base_z);
    glScalef(b.width, b.height, b.depth);
    glutSolidCube(1.0);
    glPopMatrix();
}

/**
 * @brief Normalizes the given vector to convert it to a unit vector.
 *
 * @param x The x-coordinate of the vector
 * @param y The y-coordinate of the vector
 * @param z The z-coordinate of the vector
 */
void normalize(float* x, float* y, float* z) {
    float length = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    (*x) /= length;
    (*y) /= length;
    (*z) /= length;
}

/**
 * @brief Callback function for the display, which is called whenever the window is redrawn.
 *
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the color and depth buffers
    glLoadIdentity();
    normalize(&targetX, &targetY, &targetZ);
    gluLookAt(cameraX, cameraY, cameraZ,
              cameraX + targetX, cameraY + targetY, cameraZ + targetZ,
              0.0, 1.0, 0.0);  // Set the eye position and look-at point

    // Draw the buildings
    for (int i = 0; i < (int)buildings.size(); i++) {
        drawBuilding(buildings[i]);
    }

    glutSwapBuffers();  // Swap the front and back buffers to display the scene
}

/**
 * @brief Callback function for the reshape, which is called whenever the window is resized.
 *
 * @param width The new width of the window
 * @param height The new height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, (double)width / (double)height, 0.1, 100.0);  // Set the perspective projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("City View from Helicopter");

    srand(0);
    generateBuildings();

    glutDisplayFunc(display);     // Call display() when the window needs to be redrawn
    glutReshapeFunc(reshape);     // Call reshape() when the window is resized
    glutKeyboardFunc(keyboard);   // Call keyboard() when a key is pressed
    glutMouseFunc(mouseButton);   // Call mouseButton() when a mouse button is pressed or released
    glutMotionFunc(mouseMotion);  // Call mouseMotion() when the mouse is dragged

    initGL();
    glutMainLoop();
    return 0;
}
