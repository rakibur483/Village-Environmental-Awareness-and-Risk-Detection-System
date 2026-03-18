#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>

// ================= GLOBAL VARIABLES =================
float waterLevel = 40;
bool isDay = true;
bool rainOn = false;

struct Mosquito {
    float x, y;
    float dx, dy;
};

std::vector<Mosquito> mosquitoes;

struct RainDrop {
    float x, y;
};

std::vector<RainDrop> rain;

// ================= DDA LINE =================
void drawLineDDA(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++)
    {
        glVertex2i(round(x), round(y));
        x += xInc;
        y += yInc;
    }
    glEnd();
}

// ================= BRESENHAM CIRCLE =================
void drawCircle(int xc, int yc, int r)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    glBegin(GL_POINTS);
    while (x <= y)
    {
        glVertex2i(xc + x, yc + y);
        glVertex2i(xc - x, yc + y);
        glVertex2i(xc + x, yc - y);
        glVertex2i(xc - x, yc - y);
        glVertex2i(xc + y, yc + x);
        glVertex2i(xc - y, yc + x);
        glVertex2i(xc + y, yc - x);
        glVertex2i(xc - y, yc - x);

        if (d < 0)
            d += 4 * x + 6;
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
    glEnd();
}

// ================= TEXT =================
void drawText(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    while (*text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}

// ================= INITIALIZATION =================
void initMosquitoes(int count)
{
    mosquitoes.clear();
    for (int i = 0; i < count; i++)
    {
        Mosquito m;
        m.x = rand() % 800;
        m.y = 200 + rand() % 300;
        m.dx = (rand() % 3 + 1) * 0.5;
        m.dy = (rand() % 3 + 1) * 0.5;
        mosquitoes.push_back(m);
    }
}

void initRain()
{
    rain.clear();
    for (int i = 0; i < 100; i++)
    {
        RainDrop r;
        r.x = rand() % 800;
        r.y = rand() % 600;
        rain.push_back(r);
    }
}

// ================= SCENE OBJECTS =================
void drawGround()
{
    drawLineDDA(0, 100, 800, 100);
}

void drawHouse()
{
    // Base
    drawLineDDA(300, 100, 500, 100);
    drawLineDDA(300, 100, 300, 300);
    drawLineDDA(500, 100, 500, 300);
    drawLineDDA(300, 300, 500, 300);

    // Roof
    drawLineDDA(300, 300, 400, 400);
    drawLineDDA(400, 400, 500, 300);

    // Door
    drawLineDDA(375, 100, 375, 200);
    drawLineDDA(425, 100, 425, 200);
    drawLineDDA(375, 200, 425, 200);

    // Window
    drawLineDDA(320, 220, 350, 220);
    drawLineDDA(320, 250, 350, 250);
    drawLineDDA(320, 220, 320, 250);
    drawLineDDA(350, 220, 350, 250);
}

void drawTree(int x)
{
    // trunk
    drawLineDDA(x, 100, x, 200);

    // leaves
    drawLineDDA(x - 30, 200, x, 260);
    drawLineDDA(x + 30, 200, x, 260);
}

void drawSun()
{
    if (isDay)
    {
        drawCircle(650, 500, 40);
        drawCircle(650, 500, 50);
    }
}

void drawPond()
{
    int radius = 60 + waterLevel;
    drawCircle(600, 150, radius);
    drawCircle(600, 150, radius - 10);
    drawCircle(600, 150, radius - 20);
}

void drawMosquito(float x, float y)
{
    drawLineDDA(x - 5, y - 5, x + 5, y + 5);
    drawLineDDA(x - 5, y + 5, x + 5, y - 5);
}

void drawRain()
{
    if (!rainOn) return;

    for (auto &r : rain)
        drawLineDDA(r.x, r.y, r.x, r.y - 10);
}

// ================= DISPLAY =================
void display()
{
    if (isDay)
        glClearColor(0.5, 0.8, 1, 1);
    else
        glClearColor(0.05, 0.05, 0.2, 1);

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    drawGround();
    drawHouse();
    drawTree(150);
    drawTree(220);
    drawSun();
    drawPond();

    for (auto &m : mosquitoes)
        drawMosquito(m.x, m.y);

    drawRain();

    char waterText[50];
    sprintf(waterText, "Water Level: %.0f", waterLevel);
    drawText(20, 550, waterText);

    if (waterLevel > 80)
    {
        glColor3f(1, 0, 0);
        drawText(20, 520, "DENGUE RISK: HIGH");
    }

    glFlush();
}

// ================= UPDATE =================
void update(int value)
{
    for (auto &m : mosquitoes)
    {
        m.x += m.dx;
        m.y += m.dy;

        if (m.x < 0 || m.x > 800) m.dx *= -1;
        if (m.y < 100 || m.y > 600) m.dy *= -1;
    }

    if (rainOn)
    {
        for (auto &r : rain)
        {
            r.y -= 5;
            if (r.y < 0)
                r.y = 600;

            waterLevel += 0.05;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ================= KEYBOARD =================
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
        waterLevel += 5;
        break;
    case 's':
        if (waterLevel > 10)
            waterLevel -= 5;
        break;
    case 'd':
        isDay = true;
        break;
    case 'n':
        isDay = false;
        break;
    case 'r':
        rainOn = !rainOn;
        break;
    }

    if (waterLevel > 60)
        initMosquitoes(10);
    else
        initMosquitoes(3);

    glutPostRedisplay();
}

// ================= INIT =================
void init()
{
    gluOrtho2D(0, 800, 0, 600);
    initMosquitoes(3);
    initRain();
}

// ================= MAIN =================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Advanced Sustainable Village Simulation");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
