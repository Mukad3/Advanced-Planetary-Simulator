 
#include <GLUT/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
 
#define PI 3.14159265
#define WINDOW_WIDTH 2000.0
#define WINDOW_HEIGHT 1000.0
#define G 0.005
#define SUN_MASS 30
#define DISTANCE_KOEF 1.01
#define TRACE_SIZE 1500
#define SPEED_KOEF 1.5
#define U 0.1
 
using namespace std;
 
struct point {
    float x, y;
};
 
struct planet {
    float radius, mass, distance,
          x, y, z,
          red, green, blue;
    struct point trace[TRACE_SIZE];
    int trace_count;
   
} sun, mercury, venus, earth, mars, jupiter;
 
struct planet planets[5];
 
struct camera {
    float eyeX, eyeY, eyeZ,  // поцизия камеры
          centerX, centerY, centerZ,  // куда смотрит
          upX, upY, upZ,  // вектор макушки
          alpha, beta; // счетчик для вращения
} camera;
 
 
void changeCoordinats(planet *p)
{
    float distance = sqrt(p->x * p->x + p->y * p->y);
   
    float f = G * U * p->mass * sun.mass / (distance * distance);
    p->x /= 1 + f;
    p->y /= 1 + f;
   
    for (int i = 0; i < 5; i++) {
        float d = sqrt((p->x - planets[i].x) * (p->x - planets[i].x) +
                       (p->y - planets[i].y) * (p->y - planets[i].y));        
        if (d == 0)
            continue;
        if (d <= p->radius + planets[i].radius)
            continue;
 
        float g = G * planets[i].mass / (d * d);
 
        if (distance < sqrt(planets[i].x * planets[i].x + planets[i].y * planets[i].y)) {
            p->x *= 1 + g;
            p->y *= 1 + g;
        } else {
            p->x /= 1 + g;
            p->y /= 1 + g;
        }
    }
 
    p->x -= p->y / SPEED_KOEF / distance;
    p->y += p->x / SPEED_KOEF / distance;
}
 
void drawOrbit(float r)
{    
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 50; i++) {
        float a = (float)i / 50.0f * PI * 2;
        glVertex2f(cos(a) * r, sin(a) * r);
    }
    glEnd();
}
 
void drawPlanet(planet p)
{
    glEnable(GL_COLOR_MATERIAL);
    glColor3d(p.red, p.green, p.blue);
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    glutSolidSphere(p.radius, 40, 40);
    glPopMatrix();
}
 
void drawTrace(planet *p)
{
    if (p->trace_count < TRACE_SIZE) {
        p->trace[p->trace_count].x = p->x;
        p->trace[p->trace_count].y = p->y;
        p->trace_count++;
    }
    glPointSize(2);
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_POINTS);
    glColor3d(p->red, p->green, p->blue);
    for (int i = 0; i < p->trace_count; i++) {
        glVertex3d(p->trace[i].x, p->trace[i].y, 0);
    }
    glEnd();
}
 
void Draw()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
 
    gluLookAt(camera.eyeX, camera.eyeY, camera.eyeZ,
              camera.centerX, camera.centerY, camera.centerZ,
              camera.upX, camera.upY, camera.upZ);
 
    drawPlanet(sun);
 
    glEnable(GL_LIGHTING);
 
    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light0_direction[] = {0.0, 0.0, 0.0, 1.0};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
 
    for (int i = 0; i < 5; i++) {
        drawOrbit(planets[i].distance);
        changeCoordinats(&planets[i]);
        drawPlanet(planets[i]);
        drawTrace(&planets[i]);
    }
 
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
 
    glutSwapBuffers();
}
 
void Initialize()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
 
    camera = {0, 0, 90,
              0, 0, 0,
              0.0, 1.0, 0.0,
              PI / 2.0, PI / 2.0};
 
    sun = {5, 60, 0,
           0, 0, 0,
           1, 0.7, 0};
   
    mercury = {1, 1.675, 10,
               10, 0, 0,
               1, 0, 0};
 
    venus = {2, 4.25, 16,
             16, 0, 0,
             0, 0, 1};
 
    earth = {2, 8.8, 23,
             23, 0, 0,
             0, 1, 0};
   
    mars = {1.7, 15, 30,
            30, 0, 0,
            1, 0, 0};
 
    jupiter = {3, 25.3, 39,
               39, 0, 0,
               1, 1, 0};
   
    planets[0] = mercury;
    planets[1] = venus;
    planets[2] = earth;
    planets[3] = mars;
    planets[4] = jupiter;
 
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < TRACE_SIZE; j++) {
            planets[i].trace[j] = {0, 0};
        }
        planets[i].trace_count = 0;
    }
}
 
void Keyboard(unsigned char key, int x, int y)
{
    float radius = sqrt(camera.eyeX * camera.eyeX +
                        camera.eyeY * camera.eyeY +
                        camera.eyeZ * camera.eyeZ);
    switch (key) {
    case 'w':
        camera.centerY -= 1;
        break;
    case 's':
        camera.centerY += 1;
        break;
    case 'a':
        camera.centerX += 1;
        break;
    case 'd':
        camera.centerX -= 1;
        break;
    case 'q':
        camera.eyeX *= DISTANCE_KOEF;
        camera.eyeY *= DISTANCE_KOEF;
        camera.eyeZ *= DISTANCE_KOEF;
        break;
    case 'e':
        camera.eyeX /= DISTANCE_KOEF;
        camera.eyeY /= DISTANCE_KOEF;
        camera.eyeZ /= DISTANCE_KOEF;
        break;
    case 't':
        camera.eyeY = radius * cos(camera.beta);
        camera.eyeZ = radius * sin(camera.beta);
        camera.beta += 0.02;
        break;
    case 'g':
        camera.eyeY = radius * cos(camera.beta);
        camera.eyeZ = radius * sin(camera.beta);
        camera.beta -= 0.02;
        break;
    case 'f':
        camera.eyeX = radius * cos(camera.alpha);
        camera.eyeZ = radius * sin(camera.alpha);
        camera.alpha -= 0.02;
        break;
    case 'h':
        camera.eyeX = radius * cos(camera.alpha);
        camera.eyeZ = radius * sin(camera.alpha);
        camera.alpha += 0.02;
        break;
    }
}
 
void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = 1.0 * w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45, ratio, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}
 
void Timer(int value)
{
    Draw();
    glutTimerFunc(20, Timer, 0);
}
 
int main(int argc, char** argv)
{  
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Star System");
    glutDisplayFunc(Draw);
    glutReshapeFunc(changeSize);
    glutTimerFunc(25, Timer, 0);
    glutKeyboardFunc(Keyboard);
    Initialize();
    glutMainLoop();
    return 0;
}
