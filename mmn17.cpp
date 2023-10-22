#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>

// Global variable to control the viewpoint
int currentView = 0;

float cowX = 0.0; // X position of the cow
float cowZ = 0.0; // Z position of the cow
float bodyRotationY = 0.0;
float headRotationY = 0.0;
float headRotationZ = 0.0; 
float maxRotationY = 30;
float maxRotationZ = 10; 
float tailRotationY = 0.0;
float tailRotationZ = 0.0;

int menuID;
int windowID;

int width = 800;  // Window width
int height = 600; // Window height

GLfloat ambientIntensity = 0.3; // Initial ambient intensity

const float pi = 3.14159265358979323846;

// world object
void drawCylinder(float baseRadius, float topRadius, float height, int slices, int stacks) {
    float angleStep = 2.0 * pi / slices;
    float heightStep = height / stacks;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float angle = i * angleStep;
        float x = cos(angle);
        float z = sin(angle);

        for (int j = 0; j <= stacks; ++j) {
            float y = j * heightStep;
            // Set the normal vector for lighting calculations
            glNormal3f(x, 0.0, z);
            // Define vertices for the QUAD_STRIP
            glVertex3f(baseRadius * x, y, baseRadius * z); // Bottom vertex
            glVertex3f(topRadius * x, y + heightStep, topRadius * z); // Top vertex
        }
    }
    glEnd();
}

void drawStone() {
    // Set the material properties
    GLfloat matAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat matDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat matSpecular[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    GLfloat matShininess = 10000.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

    glPushMatrix();
    glColor3f(0.75, 0.75, 0.75); // Gray color for the stone
    glTranslatef(-0.12, -2.2, -0.12); // Position of the stone
    glutSolidSphere(0.20, 40, 40); // Draw the main spherical part
    glPopMatrix();

    // Reset the material properties to default values
    GLfloat defaultMatAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat defaultMatDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat defaultMatSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat defaultMatShininess = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, defaultMatAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultMatDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, defaultMatSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, defaultMatShininess);
}

void drawTree() {
    glPushMatrix();
    // Draw tree trunk
    glColor3f(0.5, 0.3, 0.1);
    glPushMatrix();
    glTranslatef(-1.0, -0.45, 0.0);
    glScalef(0.1, 1.8, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    // Draw tree leaves (a cone)
    glColor3f(0.0, 0.5, 0.0);
    glPushMatrix();
    glTranslatef(-1.0, 0.5, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glutSolidCone(0.6, 1.5, 50, 50);
    glPopMatrix();
    glPopMatrix();
}

void drawChairLegs(float x, float y) {
    glPushMatrix();
    glTranslatef(x*0.25, -0.5, y*0.25);
    glScalef(0.05, 0.4, 0.05);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawChair() {
    // Draw chair seat
    glColor3f(0.6, 0.4, 0.2);
    glPushMatrix();
    glTranslatef(0,- 0.25, 0);
    glScalef(0.7, 0.1, 0.7);
    glutSolidCube(1.0);
    glPopMatrix();
    // Draw chair backrest
    glPushMatrix();
    glTranslatef(0, 0 + 0.05, 0 - 0.35);
    glScalef(0.7, 0.76, 0.05);
    glutSolidCube(1.0);
    glPopMatrix();
    // Draw chair legs
    drawChairLegs(1, 1);
    drawChairLegs(-1, 1);
    drawChairLegs(1, -1);
    drawChairLegs(-1, -1);
}

void drawTire() {
    glPushMatrix();
    // Draw the outer rim (torus shape)
    glColor3f(0.2, 0.2, 0.2); // Gray color for the rim
    glTranslatef(0.0, 0.0, 2); // Position the rim
    glutSolidTorus(0.08, 0.25, 20, 20); // Draw the rim
    // Draw the inner rim (smaller torus shape)
    glColor3f(0.6, 0.6, 0.6); // Gray color for the rim
    glTranslatef(0.0, 0.0, 0); // Position the rim
    glutSolidTorus(0.04, 0.15, 20, 20); // Draw the rim
    // Draw the tire body (cylinder shape)
    glColor3f(0.1, 0.1, 0.1); // Gray color for the tire body
    glTranslatef(0.0, 0.0, 0); // Position the tire body
    glRotatef(-90.0, 1.0, 0.0, 0.0); // Rotate the tire body to be vertical
    drawCylinder(0.08, 0.08, 0.2, 20, 20); // Draw the light fixture
    glPopMatrix();
}

// draw all the objects in the world
void drawMultipleObjects() {
    struct ObjectPosition {
        float tx, tz; // Tree position
        float sx, sz; // Stone position
        float cx, cz; // Chair position
        float txr, tzr; // Tire position
    };

    ObjectPosition positions[] = {
        { -5.0, -5.0, -3.0, -3.0, -2.0, -2.0, -1.0, -1.0 },
        { 10.0, -5.0, 0.5, -3.0, 1.0, -2.0, 2.0, -1.0 },
        { 5.0, -5.0, 3.5, -3.0, 1.9, -2.0, 1.25, -1.0 },
        { -3.0, -3.0, -2.0, -1.0, -1.5, -1.0, -1.0, 0.0 },
        { 3.0, -3.0, 3.5, -1.0, 1.75, -0.5, 2.5, 0.0 },
        { -5.0, 0.0, -3.0, 2.0, -4.0, 4.0, -2.0, 6.0 },
        { -1.5, -1.5, -1.0, -0.5, -1.0, 0.0, 0.0, 0.5 },
        { -3.0, 2.0, -2.0, 4.0, -2.75, 4.5, -2.0, 6.0 },
        { -1.0, 2.0, 0.0, 4.0, 0.25, 4.5, 1.0, 6.0 },
        { 3.0, 2.0, 2.5, 4.0, 1.75, 4.5, 2.5, 6.0 },
        { -4.0, 0.0, -2.5, 2.0, -3.0, 4.0, -1.5, 6.0 },
        { -2.0, -2.0, -0.5, -1.0, -1.0, 1.0, 0.0, 3.0 },
        { 6.0, -8.0, 3.5, -6.0, 2.5, -4.0, 7.0, -2.0 },
        { 4.0, 0.0, 5.0, 2.0, 6.0, 4.0, 3.0, -1.0 },
        { -6.0, -3.0, -4.5, -2.0, -5.0, 0.0, -3.0, 1.0 },
        { -2.0, -6.0, -1.0, -4.0, 0.0, -3.0, 2.0, -2.0 },
    };

    int numObjects = sizeof(positions) / sizeof(ObjectPosition);

    // draw each object in each position
    for (int i = 0; i < numObjects; ++i) {
        glPushMatrix();
        glTranslatef(positions[i].tx, 0.0, positions[i].tz); // Tree position
        drawTree();
        glTranslatef(positions[i].sx, 0.0, positions[i].sz); // Stone position
        drawStone();
        glTranslatef(positions[i].cx, 0.0, positions[i].cz); // Chair position
        // rotate the chairs
        glTranslatef(0, -1.25, 0);
        if (i % 3 == 0) {
            glRotatef(120.0, 0.0, 1.0, 0.0);
        }
        if (i % 3 == 1) {
            glRotatef(240.0, 0.0, 1.0, 0.0);
        }
        drawChair();
        glTranslatef(positions[i].txr, 0.0, positions[i].tzr); // Tire position
        // rotate half of the tires
        if (i % 2 == 0){
            glRotatef(90.0, 1.0, 0.0, 0.0);
            
        }
        if (i % 2 == 1) {
            glTranslatef(0, -0.6, 0);
        }
        drawTire();
        glPopMatrix();
    }
}

void drawGround() {
    // Set green color for the grass
    glColor3f(0.0, 0.8, 0.0);

    // Draw the grass in a non-uniform shape
    glBegin(GL_QUADS);
    float grassHeight = 2.0;
    float yOffset = -10.0;
    float zOffset = -10.0;
    int grassRange = 200; // Adjust the range to cover a larger area
    for (int i = -grassRange; i < grassRange; ++i) {
        for (int j = -grassRange; j < grassRange; ++j) {
            glVertex3f(i - 1.5, yOffset, j - 0.5 - zOffset - grassHeight); // Adjust Z coordinate
            glVertex3f(i + 5.5, yOffset, j - 0.5 - zOffset - grassHeight);
            glVertex3f(i + 5.5, yOffset, j + 0.5 - zOffset - grassHeight);
            glVertex3f(i - 0.5, yOffset, j + 0.0 - zOffset - grassHeight);
        }
    }
    glEnd();
}

// draw head and face
void drawEyes(float  x) {
    glColor3f(0.36, 0.25, 0.20); // set color for the eye
    glPushMatrix();
    glTranslatef(0.6, 0.0, x * 0.25); // Position of the right eye
    glutSolidSphere(0.05, 20, 20); // Draw the eye
    glPopMatrix();
}

void drawEars(float  x, float y) {
    glColor3f(0.8, 0.6, 0.4); // set color for the ear
    glPushMatrix();
    glTranslatef(0.45, 0.10, x*0.25); // Position of the ear
    glRotatef(-x*180.0, 1.0, 0.50, y*2.0); // Rotate the ear to point outward
    glutSolidCone(0.06, 0.3, 20, 20); // Draw the ear
    glPopMatrix();
}

void drawNose() {
    glColor3f(0.6, 0.3, 0.3); //Set color for the nose
    glPushMatrix();
    glTranslatef(0.65, -0.1, -0.0); // Position of the nose
    glRotatef(90.0, 0.0, 1.0, -0.2);
    glutSolidCone(0.1, 0.1, 20, 20); // Draw the nose
    glPopMatrix();
}

void drawLegs(float  x, float y) {
    glColor3f(0.6, 0.6, 0.6); // Set color for the legs
    glPushMatrix();
    glTranslatef(x*0.35, -0.5, y*0.2); // Translate to leg position
    glScalef(1.0, 7.0, 2.0); // Scale the length of the leg
    glutSolidCube(0.1); // Draw a cube representing the leg
    glPopMatrix();
}

// draw cow's body
void drawTail() {
    glColor3f(0.0, 0.0, 0.0); // Set color for the tail
    glPushMatrix(); // Save the current transformation state
    glTranslatef(-0.5, -0.0, 0.0); // Translate to tail position
    glRotatef(tailRotationY, 0.0, 1.0, 0.0); // Apply Y-axis rotation
    glRotatef(tailRotationZ, 0.0, 0.0, 1.0); // Apply Z-axis rotation
    glScalef(-5, 0.5, 0.2); // Scale the tail

    // Bottom part of the tail
    glBegin(GL_QUADS);

    glVertex3f(-0.1, -0.1, -0.1);
    glVertex3f(0.1, -0.1, -0.1);
    glVertex3f(0.08, 0.1, 0.2);
    glVertex3f(-0.08, 0.1, 0.2);
    glEnd();

    glutSolidCube(0.175);

    // Top part of the tail
    glBegin(GL_QUADS);
    glVertex3f(-0.08, 0.1, 0.2);
    glVertex3f(0.08, 0.1, 0.2);
    glVertex3f(0.05, 0.2, 0.4);
    glVertex3f(-0.05, 0.2, 0.4);
    glEnd();

    // Additional segments for volume
    glBegin(GL_QUADS);
    glVertex3f(-0.08, 0.1, 0.2);
    glVertex3f(0.08, 0.1, 0.2);
    glVertex3f(0.0, 0.3, 0.5);
    glVertex3f(0.0, 0.3, 0.5);

    glVertex3f(-0.05, 0.2, 0.4);
    glVertex3f(0.05, 0.2, 0.4);
    glVertex3f(0.0, 0.4, 0.6);
    glVertex3f(0.0, 0.4, 0.6);
    glEnd();

    glPopMatrix(); // Restore the previous transformation state
}

void drawSpot(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(0.13, 30, 30);
    glPopMatrix();
}

void drawCowHead() {
    // head
    glColor3f(0.9, 0.9, 0.9);
    glPushMatrix();
    glTranslatef(0.4, 0.0, 0.0);
    glutSolidSphere(0.3, 50, 50);
    glPopMatrix();
    // eyes
    drawEyes(1.0); // left eye
    drawEyes(-1.0); // right eye
    // ears
    drawEars(1.0, 1.0); // left
    drawEars(-1.0, 0.25); // right
    // nose
    drawNose();
}

void drawCow() {
    glPushMatrix();
    glTranslatef(cowX, 0.0, cowZ); // Position the cow above the ground
    glRotatef(bodyRotationY, 0.0, 1.0, 0.0); // Apply Y-axis rotation
    // body
    glColor3f(0.8, 0.8, 0.8);
    glutSolidSphere(0.5, 50, 50);
    // Draw black spots on the body
    glColor3f(0.0, 0.0, 0.0);
    drawSpot(0.2, 0.2, 0.3);
    drawSpot(-0.3, 0.2, -0.2);
    drawSpot(-0.3, 0.2, -0.2);
    drawSpot(0.0, -0.2, 0.4);
    drawSpot(-0.4, 0.2, 0.0);
    drawSpot(0.15, -0.2, -0.1);
    drawSpot(0.35, -0.2, -0.2);
    drawSpot(-0.2, -0.2, -0.1);
    drawSpot(-0.02, -0.2, -0.01);
    // head
    glPushMatrix();
    glTranslatef(0.0, 0.5, 0.0); // Position of the head
    glRotatef(headRotationY, 0.0, 1.0, 0.0); // Apply Y-axis rotation
    glRotatef(headRotationZ, 0.0, 0.0, 1.0); // Apply Z-axis rotation
    drawCowHead(); // Draw the head
    glPopMatrix();
    // legs
    drawLegs(-1.0, 1.0);
    drawLegs(1.0, 1.0);
    drawLegs(-1.0, -1.0);
    drawLegs(1.0, -1.0);
    // tail
    drawTail();
    glPopMatrix();
}

// Create lights
void init() {
    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    // Enable lighting
    glEnable(GL_LIGHTING);
    // Enable the first light source (GL_LIGHT0)
    glEnable(GL_LIGHT0);
    // Set the position of the light source
    GLfloat lightPos[] = { -10.0, -4, -7.5, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    // Define ambient, diffuse, and specular properties for the light
    GLfloat ambient[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0 };
    GLfloat diffuse[] = { 0.015, 0.015, 0.015, 1.0 };
    GLfloat specular[] = { 0.9, 0.9, 0.9, 1.0 };
    // Set light properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    // Enable automatic color tracking between lighting and material properties
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    // Set the shading model to smooth (Gouraud shading)
    glShadeModel(GL_SMOOTH);

    // Enable the second light source (GL_LIGHT1)
    glEnable(GL_LIGHT1);
    // Set the position of the second light source
    GLfloat lightPos1[] = { 50.2, -20.5, 1.2, 1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    // Define ambient properties only for the environmental light (no position)
    GLfloat ambient1[] = { 0.1, 0.1, 0.1, 1.0 }; // Adjust ambient values as needed
    GLfloat diffuse1[] = { 0.0, 0.0, 0.0, 1.0 }; // No diffuse component for an environmental light
    GLfloat specular1[] = { 0.0, 0.0, 0.0, 1.0 }; // No specular component for an environmental light
    // Set light properties for the second light source
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    // Set the clear color to blue (R, G, B, A)
    glClearColor(0.0, 0.0, 1.0, 1.0);
}

// Keyboard function to switch between views
void keyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
    case '2': // First person view
        currentView = 2;
        break;
    case '3': // Far above view
        currentView = 3;
        break;
    case '1': // Default view
        currentView = 0;
        break;
    case 'd': // Move cow to the left
        cowX += 0.05 * sin(bodyRotationY * (pi / 180.0));
        cowZ += 0.05 * cos(bodyRotationY * (pi / 180.0));
        break;
    case 'a': // Move cow to the right
        cowX -= 0.05 * sin(bodyRotationY * (pi / 180.0));
        cowZ -= 0.05 * cos(bodyRotationY * (pi / 180.0));
        break;
    case 'w': // Move cow to the left
        cowX += 0.05 * cos(bodyRotationY * (pi / 180.0));
        cowZ -= 0.05 * sin(bodyRotationY * (pi / 180.0));
        break;
    case 's': // Move cow to the right
        cowX -= 0.05 * cos(bodyRotationY * (pi / 180.0));
        cowZ += 0.05 * sin(bodyRotationY * (pi / 180.0));
        break;
    case 'q': // Body rotation to the left
        bodyRotationY += 5.0;
        break;
    case 'e': // Body rotation to the right
        bodyRotationY -= 5.0;
        break;
    case 'h': // Head rotation to the right
        if (headRotationY >= -maxRotationY){
            headRotationY -= 5.0;
        }
        break;
    case 'f': // Head rotation to the left
        if (headRotationY <= maxRotationY) {
            headRotationY += 5.0;
        }
        break;
    case 't': // Head rotation upwards
        if (headRotationZ <= maxRotationZ) {
            headRotationZ += 5.0;
        }
        break;
    case 'g': // Head rotation downwards
        if (headRotationZ >= -maxRotationZ) {
            headRotationZ -= 5.0;
        }
        break;
    case 'j': // Tail rotation to the left
        if (tailRotationY >= -maxRotationY) {
            tailRotationY -= 5.0;
        }
        break;
    case 'l': // Tail rotation to the right
        if (tailRotationY <= maxRotationY) {
            tailRotationY += 5.0;
        }
        break;
    case 'k': // Tail rotation upwards
        if (tailRotationZ <= maxRotationZ) {
            tailRotationZ += 5.0;
        }
        break;
    case 'i': // Tail rotation downwards
        if (tailRotationZ >= -maxRotationZ) {
            tailRotationZ -= 5.0;
        }
        break;
    }
    glutPostRedisplay();
}

// Set the camera viewpoint based on the current view
void viewPoint() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    // Set the camera transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentView == 2) {
        // First Person View
        float cowY = 0.25;
        float cowDirX = cowX - std::sin(bodyRotationY * pi / 180.0f);
        float cowDirZ = cowZ - std::cos(bodyRotationY * pi / 180.0f);
        gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
        glRotatef(90, 0, 1, 0);
        glTranslatef(-1.0, 0, 0);
        gluLookAt(cowX, cowY, cowZ,cowDirX, cowY, cowDirZ, 0.0f, 1.0f, 0.0f);
    }
    else if (currentView == 3) {
        // Far Isometric View
        gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
        gluLookAt(cowX  -6.0, 6.0, cowZ -24.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
    else {
        // Default Viewpoint
        gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
        gluLookAt( -2.0, 1.0, -4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
}

// menu function
void menuCallback(int value) {
    if (value == 1) {
        // Help option
        printf("Cow Drawing OpenGL Program Help:\n");
        printf("================================\n");
        printf("Use the following keys to control the cow:\n");
        printf("w: Move cow up\n");
        printf("s: Move cow down\n");
        printf("a: Move cow left\n");
        printf("d: Move cow right\n");
        printf("q: Move cow diagonally (up and left)\n");
        printf("e: Move cow diagonally (up and right)\n");
        printf("t: Rotate cow's head up\n");
        printf("g: Rotate cow's head down\n");
        printf("f: Rotate cow's head left\n");
        printf("h: Rotate cow's head right\n");
        printf("i: Rotate cow's tail up\n");
        printf("k: Rotate cow's tail down\n");
        printf("j: Rotate cow's tail left\n");
        printf("l: Rotate cow's tail right\n");
        printf("Press '1' for the default view.\n");
        printf("Press '2' for the first person view.\n");
        printf("Press '3' for a far view from above.\n");
        printf("Right-click the window to access the context menu for more options.\n");
        printf("\n");
    }
    else if (value == 2) {
        // Increase Ambient Light option
        ambientIntensity += 0.1; // Increase ambient intensity
        if (ambientIntensity > 1.0) {
            ambientIntensity = 1.0;
        }
        GLfloat ambient[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glutPostRedisplay();
    }
    else if (value == 3) {
        // Decrease Ambient Light option
        ambientIntensity -= 0.1; // Decrease ambient intensity
        if (ambientIntensity < 0.0) {
            ambientIntensity = 0.0; // Cap at 0.0
        }
        GLfloat ambient[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glutPostRedisplay();
    }
    else if (value == 4) {
        // Quit option
        glutDestroyWindow(windowID);
        exit(0);
    }
}

void createMenu() {
    menuID = glutCreateMenu(menuCallback); 
    // Create the submenu for ambient light adjustment
    int ambientSubMenuID = glutCreateMenu(menuCallback);
    glutAddMenuEntry("Increase Ambient Light", 2);
    glutAddMenuEntry("Decrease Ambient Light", 3);
    glutSetMenu(menuID);
    glutAddSubMenu("Adjust Ambient Light", ambientSubMenuID);
    glutAddMenuEntry("Help", 1);
    glutAddMenuEntry("Quit", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void display() {
    glClearColor(0.05, 0.05, 0.05, 1.0); // Light Black
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // set the viewpoint
    viewPoint();
    // draw the cow and the world
    drawCow();
    drawMultipleObjects();
    drawGround();
    glFlush();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);   
    // Set display mode (double buffering, RGB color, depth buffer)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Set window size and create the window
    glutInitWindowSize(width, height);
    windowID  = glutCreateWindow("OpenGL Cow on Grass");
    // Create and attach the menu
    createMenu();
    glEnable(GL_DEPTH_TEST);
    // Call the init function to set up lighting
    init();
    // Set the display and keyboard callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardFunc); 
    // Start the main loop
    glutMainLoop();
    return 0;
}