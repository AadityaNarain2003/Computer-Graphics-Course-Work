#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846
int sides=0;

typedef struct vertice
{
    float x;
    float y;
} Vertice;

Vertice *v = NULL;

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    gluLookAt(2.0, 2.0, 6.0,  // Eye position
              0.0, 0.0, 0.0,  // Look-at position
              0.0, 1.0, 0.0); // Up vector
}

void generateVertices(int sides)
{
    v = (Vertice *)malloc(sizeof(Vertice) * sides);
    if (v == NULL)
    {
        // Memory allocation failed
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < sides; ++i)
    {
        double angle = 2 * M_PI * i / sides;
        double x = cos(angle);
        double y = sin(angle);
        v[i].x = x;
        v[i].y = y;
    }
}

void drawPolygon(int sides, float z)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < sides; ++i)
    {
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(v[i].x, v[i].y, z); // Draw the polygon in the x-z plane (y = 0)
    }
    glEnd();
}

void drawQuad(int sides, float z1, float z2)
{
    for (int i = 0; i < sides - 1; ++i)
    {
        glBegin(GL_QUADS);
        glColor3f(0.02*i, 0.0, 0.01*i);
        glVertex3f(v[i].x, v[i].y, z1);
        glVertex3f(v[i + 1].x, v[i + 1].y, z1);
        glVertex3f(v[i + 1].x, v[i + 1].y, z2);
        glVertex3f(v[i].x, v[i].y, z2);
        
        glEnd();
    }

    glBegin(GL_QUADS);
    glColor3f(0.02*sides, 0.05*sides,0.04* sides);
    glVertex3f(v[0].x, v[0].y, z1);
    glVertex3f(v[sides-1].x, v[sides-1].y, z1);
    glVertex3f(v[sides-1].x, v[sides-1].y, z2);
    glVertex3f(v[0].x, v[0].y, z2);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glLoadIdentity();

    // glTranslatef(0.0, -1.0, -8.0); // Translate to the appropriate position

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

    glColor3f(1.0, 1.0, 1.0);

    //int sides = 40; // Change the number of sides here

    generateVertices(sides);
   
    drawQuad(sides, 3.0, -3.0);
     drawPolygon(sides, 3.0);
    drawPolygon(sides, -3.0);

    glFlush();
}

int main(int argc, char **argv)
{
    sides=atoi(argv[1]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(300, 300);
    glutCreateWindow("Polygon of n Sides");

    init();

    glutDisplayFunc(display);

    glutMainLoop();

    free(v); // Free allocated memory

    return 0;
}
