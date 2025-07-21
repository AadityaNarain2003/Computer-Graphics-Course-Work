#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define M_PI 3.14159265358979323846
int sides = 0;

// initial positions of the camera
float cameraX = 2.0;
float cameraY = 2.0;
float cameraZ = 6.0;

// some constants that determine movement
double increase_amount = 1.0;
double angle = 0.01;
float movementSpeed = 0.2;

int condition = 1; // 1 is for prism and 2 is for triangle
typedef struct vertice
{
    float x;
    float y;
    float z;
} Vertice;

// these vectors define the 3 axis of the object around which the object will be build
Vertice x_axis = {1, 0, 0};
Vertice y_axis = {0, 1, 0};
Vertice z_axis = {0, 0, 1}; // now the vertice will be build according to this axis
Vertice origin = {0, 0, 0};
float distance = 2;
float radius = 1;

void resetCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, // Eye position
              0.0, 0.0, 0.0,             // Look-at position
              0.0, 1.0, 0.0);            // Up vector
}

Vertice *vp = NULL;
Vertice *vn = NULL;

Vertice crossProduct(Vertice v1, Vertice v2)
{
    Vertice result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

// Function to normalize a vector
void normalize(Vertice *v)
{
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

void rotateY(Vertice *v, float angle)
{
    float cosA = cos(angle);
    float sinA = sin(angle);
    float x = v->x;
    float z = v->z;
    v->x = x * cosA - z * sinA;
    v->z = x * sinA + z * cosA;
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, // Eye position
              0.0, 0.0, 0.0,             // Look-at position
              0.0, 1.0, 0.0);            // Up vector
}

void generateVertices(int sides)
{

    Vertice u, w;
    if (z_axis.x != 0 || z_axis.y != 0)
    {
        u.x = -z_axis.y;
        u.y = z_axis.x;
        u.z = 0;
    }
    else
    {
        u.x = 1;
        u.y = 0;
        u.z = 0;
    }
    w = crossProduct(z_axis, u);
    normalize(&u);
    normalize(&w);

    vp = (Vertice *)malloc(sizeof(Vertice) * sides);
    vn = (Vertice *)malloc(sizeof(Vertice) * sides);
    if (vp == NULL || vn == NULL)
    {
        // Memory allocation failed
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < sides; i++)
    {
        float theta = 2 * M_PI * i / sides;
        float x = origin.x + distance * z_axis.x + radius * (cos(theta) * u.x + sin(theta) * w.x);
        float y = origin.y + distance * z_axis.y + radius * (cos(theta) * u.y + sin(theta) * w.y);
        float z = origin.z + distance * z_axis.z + radius * (cos(theta) * u.z + sin(theta) * w.z);
        vp[i].x = x;
        vp[i].y = y;
        vp[i].z = z;

        x = origin.x - distance * z_axis.x + radius * (cos(theta) * u.x + sin(theta) * w.x);
        y = origin.y - distance * z_axis.y + radius * (cos(theta) * u.y + sin(theta) * w.y);
        z = origin.z - distance * z_axis.z + radius * (cos(theta) * u.z + sin(theta) * w.z);
        vn[i].x = x;
        vn[i].y = y;
        vn[i].z = z;
    }
}

void drawPolygon(int sides, Vertice *v,int x,int y,int z)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < sides; ++i)
    {
        glColor3f(x, y,z);
        glVertex3f(v[i].x, v[i].y, v[i].z); // Draw the polygon in the x-z plane (y = 0)
    }
    glEnd();
}

void drawQuad(int sides, Vertice *vp, Vertice *vn)
{
    for (int i = 0; i < sides - 1; ++i)
    {
        glBegin(GL_QUADS);
        glColor3f(0.2 * i, 0.03 * i, 0.01 * i);
        glVertex3f(vp[i].x, vp[i].y, vp[i].z);
        glVertex3f(vn[i].x, vn[i].y, vn[i].z);
        glVertex3f(vn[i + 1].x, vn[i + 1].y, vn[i + 1].z);
        glVertex3f(vp[i + 1].x, vp[i + 1].y, vp[i + 1].z);

        glEnd();
    }

    glBegin(GL_QUADS);
    glColor3f(0.02 * sides, 0.05 * sides, 0.04 * sides);
    glVertex3f(vp[0].x, vp[0].y, vp[0].z);
    glVertex3f(vn[0].x, vn[0].y, vn[0].z);
    glVertex3f(vn[sides - 1].x, vn[sides - 1].y, vn[sides - 1].z);
    glVertex3f(vp[sides - 1].x, vp[sides - 1].y, vp[sides - 1].z);
    glEnd();
}

void drawPyramind(int sides, Vertice *vp, Vertice temp)
{
    for (int i = 0; i < sides - 1; ++i)
    {
        glBegin(GL_POLYGON);
        glColor3f(0.2 * i, 0.03 * i, 0.01 * i);
        glVertex3f(vp[i].x, vp[i].y, vp[i].z);
        glVertex3f(temp.x, temp.y, temp.z);
        glVertex3f(vp[i + 1].x, vp[i + 1].y, vp[i + 1].z);

        glEnd();
    }

    glBegin(GL_POLYGON);
    glColor3f(0.02 * sides, 0.05 * sides, 0.04 * sides);
    glVertex3f(vp[0].x, vp[0].y, vp[0].z);
    glVertex3f(temp.x, temp.y, temp.z);
    glVertex3f(vp[sides-1].x, vp[sides-1].y, vp[sides-1].z);
    glEnd();
}

void display()
{
    printf("here\n");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    resetCamera();

    glColor3f(1.0, 1.0, 1.0);

    // int sides = 40; // Change the number of sides here
    if (condition == 1)
    {
        generateVertices(sides);
        drawQuad(sides, vp, vn);
        drawPolygon(sides, vp,1,0,0);
        drawPolygon(sides, vn,0,0,1);
    }
    else
    {
        generateVertices(sides);
        drawPolygon(sides, vp,3,2,1);
        Vertice temp;
        temp.x = origin.x - distance * z_axis.x;
        temp.y = origin.z - distance * z_axis.y;
        temp.z = origin.z - distance * z_axis.z;
        drawPyramind(sides, vp, temp);
    }

    // Draw X-axis
    glColor3f(1.0, 0.0, 0.0); // Red
    glBegin(GL_LINES);
    glVertex3f(-10.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
    glEnd();

    // Draw Y-axis
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_LINES);
    glVertex3f(0.0, -10.0, 0.0);
    glVertex3f(0.0, 10.0, 0.0);
    glEnd();

    // Draw Z-axis
    glColor3f(0.0, 0.0, 1.0); // Blue
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -10.0);
    glVertex3f(0.0, 0.0, 10.0);
    glEnd();

    glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    {
        double distance = sqrt(cameraX * cameraX + cameraY * cameraY + cameraZ * cameraZ);

        double unit_x = cameraX / distance;
        double unit_y = cameraY / distance;
        double unit_z = cameraZ / distance;

        double increased_x = unit_x * increase_amount;
        double increased_y = unit_y * increase_amount;
        double increased_z = unit_z * increase_amount;

        cameraX -= increased_x;
        cameraY -= increased_y;
        cameraZ -= increased_z;
        break;
    }
    case 's':
    {
        double distance = sqrt(cameraX * cameraX + cameraY * cameraY + cameraZ * cameraZ);

        double unit_x = cameraX / distance;
        double unit_y = cameraY / distance;
        double unit_z = cameraZ / distance;

        double increased_x = unit_x * increase_amount;
        double increased_y = unit_y * increase_amount;
        double increased_z = unit_z * increase_amount;

        cameraX += increased_x;
        cameraY += increased_y;
        cameraZ += increased_z;
        break;
    }
    case 'a':
    {
        double radius = sqrt(cameraX * cameraX + cameraY * cameraY + cameraZ * cameraZ);
        double s = sin(angle);
        double c = cos(angle);

        // Convert back to Cartesian coordinates
        double x = c * cameraX + s * cameraZ;
        double y = cameraY;
        double z = -s * cameraX + c * cameraZ;

        cameraX = x;
        cameraY = y;
        cameraZ = z;
        break;
    }
    case 'd':
    {
        double radius = sqrt(cameraX * cameraX + cameraY * cameraY + cameraZ * cameraZ);
        double s = sin(-angle);
        double c = cos(-angle);

        // Convert back to Cartesian coordinates
        double x = c * cameraX + s * cameraZ;
        double y = cameraY;
        double z = -s * cameraX + c * cameraZ;

        cameraX = x;
        cameraY = y;
        cameraZ = z;
        break;
    }
    case 'q':
    {
        // first lets find the axis
        double radius = sqrt(cameraZ * cameraZ + cameraX * cameraX);
        double axis_x = cameraZ / radius;
        double axis_y = 0;
        double axis_z = -cameraX / radius;

        double sinAngle = sin(angle);
        double cosAngle = cos(angle);

        double x = (cosAngle + (1 - cosAngle) * axis_x * axis_x) * cameraX +
                   ((1 - cosAngle) * axis_x * axis_y - sinAngle * axis_z) * cameraY +
                   ((1 - cosAngle) * axis_x * axis_z + sinAngle * axis_y) * cameraZ;

        double y = ((1 - cosAngle) * axis_y * axis_x + sinAngle * axis_z) * cameraX +
                   (cosAngle + (1 - cosAngle) * axis_y * axis_y) * cameraY +
                   ((1 - cosAngle) * axis_y * axis_z - sinAngle * axis_x) * cameraZ;

        double z = ((1 - cosAngle) * axis_z * axis_x - sinAngle * axis_y) * cameraX +
                   ((1 - cosAngle) * axis_z * axis_y + sinAngle * axis_x) * cameraY +
                   (cosAngle + (1 - cosAngle) * axis_z * axis_z) * cameraZ;

        cameraX = x;
        cameraY = y;
        cameraZ = z;
        break;
    }
    case 'e':
    {
        // first lets find the axis
        double radius = sqrt(cameraZ * cameraZ + cameraX * cameraX);
        double axis_x = cameraZ / radius;
        double axis_y = 0;
        double axis_z = -cameraX / radius;

        double sinAngle = sin(-angle);
        double cosAngle = cos(-angle);

        double x = (cosAngle + (1 - cosAngle) * axis_x * axis_x) * cameraX +
                   ((1 - cosAngle) * axis_x * axis_y - sinAngle * axis_z) * cameraY +
                   ((1 - cosAngle) * axis_x * axis_z + sinAngle * axis_y) * cameraZ;

        double y = ((1 - cosAngle) * axis_y * axis_x + sinAngle * axis_z) * cameraX +
                   (cosAngle + (1 - cosAngle) * axis_y * axis_y) * cameraY +
                   ((1 - cosAngle) * axis_y * axis_z - sinAngle * axis_x) * cameraZ;

        double z = ((1 - cosAngle) * axis_z * axis_x - sinAngle * axis_y) * cameraX +
                   ((1 - cosAngle) * axis_z * axis_y + sinAngle * axis_x) * cameraY +
                   (cosAngle + (1 - cosAngle) * axis_z * axis_z) * cameraZ;

        cameraX = x;
        cameraY = y;
        cameraZ = z;
        break;
    }
    }
    switch (key)
    {
    case 'i':
    {
        float shiftDistance = 1.0;

        float cameraDirectionX = -cameraX;
        float cameraDirectionY = -cameraY;
        float cameraDirectionZ = -cameraZ;

        float length = sqrt(cameraDirectionX * cameraDirectionX +
                            cameraDirectionY * cameraDirectionY +
                            cameraDirectionZ * cameraDirectionZ);

        cameraDirectionX /= length;
        cameraDirectionY /= length;
        cameraDirectionZ /= length;

        float upX = 0.0;
        float upY = 1.0;
        float upZ = 0.0;

        float leftX = upY * cameraDirectionZ - upZ * cameraDirectionY;
        float leftY = upZ * cameraDirectionX - upX * cameraDirectionZ;
        float leftZ = upX * cameraDirectionY - upY * cameraDirectionX;

        float upDirectionX = cameraDirectionY * leftZ - cameraDirectionZ * leftY;
        float upDirectionY = cameraDirectionZ * leftX - cameraDirectionX * leftZ;
        float upDirectionZ = cameraDirectionX * leftY - cameraDirectionY * leftX;

        length = sqrt(upDirectionX * upDirectionX +
                      upDirectionY * upDirectionY +
                      upDirectionZ * upDirectionZ);

        upDirectionX /= length;
        upDirectionY /= length;
        upDirectionZ /= length;

        float shiftX = shiftDistance * upDirectionX;
        float shiftY = shiftDistance * upDirectionY;
        float shiftZ = shiftDistance * upDirectionZ;

        origin.x += shiftX;
        origin.y += shiftY;
        origin.z += shiftZ;
        break;
    }
    case 'k':
    {
        float shiftDistance = 1.0;

        float cameraDirectionX = -cameraX;
        float cameraDirectionY = -cameraY;
        float cameraDirectionZ = -cameraZ;

        float length = sqrt(cameraDirectionX * cameraDirectionX +
                            cameraDirectionY * cameraDirectionY +
                            cameraDirectionZ * cameraDirectionZ);

        cameraDirectionX /= length;
        cameraDirectionY /= length;
        cameraDirectionZ /= length;

        float upX = 0.0;
        float upY = 1.0;
        float upZ = 0.0;

        float leftX = upY * cameraDirectionZ - upZ * cameraDirectionY;
        float leftY = upZ * cameraDirectionX - upX * cameraDirectionZ;
        float leftZ = upX * cameraDirectionY - upY * cameraDirectionX;

        float upDirectionX = cameraDirectionY * leftZ - cameraDirectionZ * leftY;
        float upDirectionY = cameraDirectionZ * leftX - cameraDirectionX * leftZ;
        float upDirectionZ = cameraDirectionX * leftY - cameraDirectionY * leftX;

        length = sqrt(upDirectionX * upDirectionX +
                      upDirectionY * upDirectionY +
                      upDirectionZ * upDirectionZ);

        upDirectionX /= length;
        upDirectionY /= length;
        upDirectionZ /= length;

        float shiftX = shiftDistance * upDirectionX;
        float shiftY = shiftDistance * upDirectionY;
        float shiftZ = shiftDistance * upDirectionZ;

        origin.x -= shiftX;
        origin.y -= shiftY;
        origin.z -= shiftZ;
        break;
    }
    case 'j':
    {
        float cameraDirectionX = -cameraX;
        float cameraDirectionY = -cameraY;
        float cameraDirectionZ = -cameraZ;

        float length = sqrt(cameraDirectionX * cameraDirectionX +
                            cameraDirectionY * cameraDirectionY +
                            cameraDirectionZ * cameraDirectionZ);

        cameraDirectionX /= length;
        cameraDirectionY /= length;
        cameraDirectionZ /= length;

        float upX = 0.0;
        float upY = 1.0;
        float upZ = 0.0;

        float leftX = upY * cameraDirectionZ - upZ * cameraDirectionY;
        float leftY = upZ * cameraDirectionX - upX * cameraDirectionZ;
        float leftZ = upX * cameraDirectionY - upY * cameraDirectionX;

        float shiftX = 1 * leftX;
        float shiftY = 1 * leftY;
        float shiftZ = 1 * leftZ;

        origin.x += shiftX;
        origin.y += shiftY;
        origin.z += shiftZ;
        break;
    }
    case 'l':
    {
        float cameraDirectionX = -cameraX;
        float cameraDirectionY = -cameraY;
        float cameraDirectionZ = -cameraZ;

        float length = sqrt(cameraDirectionX * cameraDirectionX +
                            cameraDirectionY * cameraDirectionY +
                            cameraDirectionZ * cameraDirectionZ);

        cameraDirectionX /= length;
        cameraDirectionY /= length;
        cameraDirectionZ /= length;

        float upX = 0.0;
        float upY = 1.0;
        float upZ = 0.0;

        float leftX = upY * cameraDirectionZ - upZ * cameraDirectionY;
        float leftY = upZ * cameraDirectionX - upX * cameraDirectionZ;
        float leftZ = upX * cameraDirectionY - upY * cameraDirectionX;

        float shiftX = 1 * leftX;
        float shiftY = 1 * leftY;
        float shiftZ = 1 * leftZ;

        origin.x -= shiftX;
        origin.y -= shiftY;
        origin.z -= shiftZ;
        break;
    }
    case 'u':
    {
        float shiftDistance = 1.0;

        float cameraDirectionX = -cameraX;
        float cameraDirectionY = -cameraY;
        float cameraDirectionZ = -cameraZ;

        float length = sqrt(cameraDirectionX * cameraDirectionX +
                            cameraDirectionY * cameraDirectionY +
                            cameraDirectionZ * cameraDirectionZ);

        cameraDirectionX /= length;
        cameraDirectionY /= length;
        cameraDirectionZ /= length;

        float shiftX = shiftDistance * cameraDirectionX;
        float shiftY = shiftDistance * cameraDirectionY;
        float shiftZ = shiftDistance * cameraDirectionZ;

        origin.x += shiftX;
        origin.y += shiftY;
        origin.z += shiftZ;
        break;
    }
    case 'o':
    {
        float shiftDistance = 1.0;

        float cameraDirectionX = -cameraX;
        float cameraDirectionY = -cameraY;
        float cameraDirectionZ = -cameraZ;

        float length = sqrt(cameraDirectionX * cameraDirectionX +
                            cameraDirectionY * cameraDirectionY +
                            cameraDirectionZ * cameraDirectionZ);

        cameraDirectionX /= length;
        cameraDirectionY /= length;
        cameraDirectionZ /= length;

        float shiftX = shiftDistance * cameraDirectionX;
        float shiftY = shiftDistance * cameraDirectionY;
        float shiftZ = shiftDistance * cameraDirectionZ;

        origin.x -= shiftX;
        origin.y -= shiftY;
        origin.z -= shiftZ;
        break;
    }
    }

    switch (key)
    {
    case 'r':
    {
        rotateY(&x_axis, angle);
        rotateY(&z_axis, angle);
    }
    }

    switch (key)
    {
    case 't':
    {
        if (condition == 1)
        {
            condition = 2;
        }
        else
        {
            condition = 1;
        }
    }
    }
    // printf("%f %f %f\n", x_offset, y_offset, z_offset);
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    sides = atoi(argv[1]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(800, 300);
    glutCreateWindow("Polygon of n Sides");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    free(vp); // Free allocated memory
    free(vn);
    return 0;
}