/*
Name: Ashutosh Kumar Singh
Roll No.: 19CS30008
Email: ashu11.03.2002@gmail.com
Assignment 3
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const float SENSITIVITY = 0.3;        // The sensitivity of the mouse dragging motion, higher the value, greater the rotation
const float TRANSLATION_DELTA = 2.0;  // The translation delta when the arrow keys, or 'w', 'a', 's', 'd', 'q', 'e' keys are pressed
const float SCALE_DELTA = 0.1;        // The scale delta when the scroll wheel is scrolled up or down

// Structure to store the 3D vertex coordinates
typedef struct {
    float x, y, z;
} Vertex;

// Structure to store the face, which is a list of vertex indices
typedef vector<int> Face;

float rotationX = 0.0;                                                // Rotation about the x-axis, to allow the user to rotate the scene using the mouse
float rotationY = 0.0;                                                // Rotation about the y-axis, to allow the user to rotate the scene using the mouse
float scale = 1.0f;                                                   // Scale factor to allow the user to zoom in and out using the scroll wheel
float translationX = 0.0f, translationY = 0.0f, translationZ = 0.0f;  // Translation along the x, y, and z axes

int isMouseLeftButtonPressed = 0;  // Indicator for whether the left mouse button is currently pressed
int prevMouseX = -1;               // The previous x-coordinate of the mouse, to calculate the change in the x-coordinate
int prevMouseY = -1;               // The previous y-coordinate of the mouse, to calculate the change in the y-coordinate

Vertex eye = {0.0, 0.0, 100.0};  // The position of the eye, or the camera

vector<Vertex> vertices;  // The list of vertices of the mesh
vector<Face> faces;       // The list of faces of the mesh

/**
 * @brief Parses the mesh file and stores the vertices and faces in the respective vectors.
 *
 * @param filename Path to the mesh file
 */
void parseMeshFile(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    string line;
    int numVertices = 0, numFaces = 0;
    // Read the header of the mesh file to get the number of vertices and faces
    while (getline(file, line)) {
        if (line.find("element vertex") == 0) {
            sscanf(line.c_str(), "element vertex %d", &numVertices);
        } else if (line.find("element face") == 0) {
            sscanf(line.c_str(), "element face %d", &numFaces);
        } else if (line == "end_header") {
            break;
        }
    }

    vertices.resize(numVertices);
    faces.resize(numFaces);

    float xmin = 1e9, ymin = 1e9, zmin = 1e9;
    float xmax = -1e9, ymax = -1e9, zmax = -1e9;
    for (int i = 0; i < numVertices; i++) {
        file >> vertices[i].x >> vertices[i].y >> vertices[i].z;
        xmin = min(xmin, vertices[i].x);
        ymin = min(ymin, vertices[i].y);
        zmin = min(zmin, vertices[i].z);
        xmax = max(xmax, vertices[i].x);
        ymax = max(ymax, vertices[i].y);
        zmax = max(zmax, vertices[i].z);
    }

    // Set the eye position to be at the center of the bounding box of the mesh
    eye.x = (xmin + xmax) / 2;
    eye.y = (ymin + ymax) / 2;
    eye.z = zmax + (zmax - zmin);

    for (int i = 0; i < numFaces; i++) {
        int numVerticesInFace;
        file >> numVerticesInFace;
        for (int j = 0; j < numVerticesInFace; j++) {
            int vertexIndex;
            file >> vertexIndex;
            faces[i].push_back(vertexIndex);
        }
    }

    file.close();
}

/**
 * @brief Computes the normal of the face using the cross product of the vectors formed by the vertices of the face.
 *
 * @param f The face for which the normal is to be computed
 * @param normal The array to store the computed normal
 */
void computeNormal(Face f, float* normal) {
    Vertex v1 = vertices[f[0]], v2 = vertices[f[1]], v3 = vertices[f[2]];
    normal[0] = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
    normal[1] = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
    normal[2] = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
}

/**
 * @brief Draws the mesh by iterating over the faces and drawing the vertices of each face.
 * It also computes the normal of each face and sets it for lighting.
 *
 */
void drawMesh() {
    for (int i = 0; i < faces.size(); i++) {
        glBegin(GL_POLYGON);
        float normal[3];
        computeNormal(faces[i], normal);
        glNormal3fv(normal);
        for (int j = 0; j < faces[i].size(); j++) {
            Vertex v = vertices[faces[i][j]];
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
}

/**
 * @brief Initializes the OpenGL settings, such as the background color, depth testing, lighting, and shading.
 *
 */
void initGL() {
    glClearColor(0.0, 0.0, 0.0, 0.0);  // Set the background color to black
    glEnable(GL_DEPTH_TEST);           // Enable depth testing for z-buffering

    glEnable(GL_NORMALIZE);  // Enable automatic normalization of normals

    glEnable(GL_LIGHTING);  // Enable lighting
    glEnable(GL_LIGHT0);    // Turn on a light source

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);  // Enable two-sided lighting

    GLfloat light_pos[] = {eye.x, eye.y, eye.z, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);  // Set the light position at the eye position

    GLfloat ambientColor[] = {0.2, 0.2, 0.2, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);  // Set the ambient light color

    GLfloat diffuseColor[] = {0.8, 0.8, 0.8, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);  // Set the diffuse light color

    GLfloat specularColor[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);  // Set the specular light color

    GLfloat green[] = {0.0, 0.6, 0.0, 1.0};
    float white[] = {0.4, 0.4, 0.4, 1};  // For specular highlights

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);  // Set the ambient and diffuse material color
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);             // Set the specular material color
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);                // Set the shininess of the material

    glShadeModel(GL_SMOOTH);  // Enable smooth shading
}

/**
 * @brief Callback function for the keyboard, which is called when a key is pressed.
 * The function changes the translation of the shape along the x, y, and z axes, based on the key pressed.
 * 'w' moves the shape up, 's' moves the shape down, 'a' moves the shape left, 'd' moves the shape right,
 * 'q' moves the shape closer, and 'e' moves the shape farther.
 *
 * @param key The key pressed
 * @param x The x-coordinate of the mouse when the key was pressed
 * @param y The y-coordinate of the mouse when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            translationY += TRANSLATION_DELTA;
            break;
        case 's':
            translationY -= TRANSLATION_DELTA;
            break;
        case 'a':
            translationX -= TRANSLATION_DELTA;
            break;
        case 'd':
            translationX += TRANSLATION_DELTA;
            break;
        case 'q':
            translationZ += TRANSLATION_DELTA;
            break;
        case 'e':
            translationZ -= TRANSLATION_DELTA;
            break;
    }
    glutPostRedisplay();
}

/**
 * @brief Callback function for the special keys, which is called when a special key is pressed.
 * The function changes the translation of the shape along the x and y axes, based on the special key pressed.
 * The up arrow key moves the shape up, the down arrow key moves the shape down,
 * the left arrow key moves the shape left, and the right arrow key moves the shape right.
 *
 * @param key The special key pressed
 * @param x The x-coordinate of the mouse when the special key was pressed
 * @param y The y-coordinate of the mouse when the special key was pressed
 */
void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            translationY += TRANSLATION_DELTA;
            break;
        case GLUT_KEY_DOWN:
            translationY -= TRANSLATION_DELTA;
            break;
        case GLUT_KEY_LEFT:
            translationX -= TRANSLATION_DELTA;
            break;
        case GLUT_KEY_RIGHT:
            translationX += TRANSLATION_DELTA;
            break;
    }
    glutPostRedisplay();
}

/**
 * @brief Callback function for the mouse button, which is called when a mouse button is pressed or released,
 * or the scroll wheel is scrolled.
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
            isMouseLeftButtonPressed = 1;
            prevMouseX = x;
            prevMouseY = y;
        } else {
            isMouseLeftButtonPressed = 0;
        }
    } else if (button == 3 || button == 4) {
        if (state == GLUT_UP) {  // Ignore the state when the scroll wheel is released
            return;
        }
        if (button == 3) {  // Scroll wheel scrolled up
            scale += SCALE_DELTA;
        } else {  // Scroll wheel scrolled down
            scale -= SCALE_DELTA;
        }
        glutPostRedisplay();
    }
}

/**
 * @brief Callback function for the mouse motion, which is called when the mouse is dragged.
 * The function changes the rotation of the scene based on the change in the x and y coordinates of the mouse.
 *
 * @param x The x-coordinate of the mouse when it was dragged
 * @param y The y-coordinate of the mouse when it was dragged
 */
void mouseMotion(int x, int y) {
    if (isMouseLeftButtonPressed) {
        rotationX += (y - prevMouseY) * SENSITIVITY;  // Change in the y-coordinate of the mouse is used to rotate about the x-axis
        rotationY += (x - prevMouseX) * SENSITIVITY;  // Change in the x-coordinate of the mouse is used to rotate about the y-axis

        // Limit the rotation about the x-axis to -90 to 90 degrees
        // Otherwise, the scene will be flipped upside down

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

/**
 * @brief Callback function for the display, which is called whenever the window is redrawn.
 *
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the color and depth buffers
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // Set the eye position and look-at point

    glTranslatef(translationX, translationY, translationZ);  // Translate the shape
    glRotatef(rotationY, 0.0, 1.0, 0.0);                     // Rotate the shape about the y-axis
    glRotatef(rotationX, 1.0, 0.0, 0.0);                     // Rotate the shape about the x-axis
    glScalef(scale, scale, scale);                           // Scale the shape

    drawMesh();

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
    gluPerspective(70.0, (double)width / (double)height, 1.0, 700.0);  // Set the perspective projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <mesh_file>" << endl;
        return 1;
    }

    cout << "Press 'w' to move up, 's' to move down, 'a' to move left, 'd' to move right, 'q' to move closer, 'e' to move farther" << endl;
    cout << "Or you can also use the arrow keys to move up, down, left, and right" << endl;
    cout << "Scroll the mouse wheel up to zoom in and down to zoom out" << endl;
    cout << "Click and drag the left mouse button to rotate the scene" << endl;

    parseMeshFile(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Visualise and Interact with 3D Meshes");

    glutDisplayFunc(display);      // Call display() when the window needs to be redrawn
    glutReshapeFunc(reshape);      // Call reshape() when the window is resized
    glutKeyboardFunc(keyboard);    // Call keyboard() when a key is pressed
    glutSpecialFunc(specialKeys);  // Call specialKeys() when a special key is pressed
    glutMouseFunc(mouseButton);    // Call mouseButton() when a mouse button is pressed or released
    glutMotionFunc(mouseMotion);   // Call mouseMotion() when the mouse is dragged

    initGL();
    glutMainLoop();
    return 0;
}
