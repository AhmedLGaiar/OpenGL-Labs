
#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int MAX_DEPTH = 5;

int Init()
{
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black background
    glDisable(GL_DEPTH_TEST); // Not needed for 2D

    return 0;
}

void SetColorByDepth(int depth)
{
    float t = static_cast<float>(depth) / MAX_DEPTH;

    // HSV to RGB approximation for colorful gradient
    float r = fabs(sin(3.14f * t));
    float g = fabs(sin(3.14f * (t + 0.33f)));
    float b = fabs(sin(3.14f * (t + 0.66f)));

    glColor3f(r, g, b);
}


void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int depth)
{
    SetColorByDepth(depth);

    glBegin(GL_TRIANGLES); // Filled triangle for color to show
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void Sierpinski(float x1, float y1, float x2, float y2, float x3, float y3, int depth)
{
    if (depth == 0)
    {
        DrawTriangle(x1, y1, x2, y2, x3, y3, depth);
    }
    else
    {
        float mx1 = (x1 + x2) / 2;
        float my1 = (y1 + y2) / 2;
        float mx2 = (x2 + x3) / 2;
        float my2 = (y2 + y3) / 2;
        float mx3 = (x1 + x3) / 2;
        float my3 = (y1 + y3) / 2;

        Sierpinski(x1, y1, mx1, my1, mx3, my3, depth - 1);
        Sierpinski(mx1, my1, x2, y2, mx2, my2, depth - 1);
        Sierpinski(mx3, my3, mx2, my2, x3, y3, depth - 1);
    }
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::Window window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Colorful Sierpinski Triangle", sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);

    if (Init()) return 1;

    // Setup 2D projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        glClear(GL_COLOR_BUFFER_BIT);

        float x1 = -0.8f, y1 = -0.7f;
        float x2 = 0.8f, y2 = -0.7f;
        float x3 = 0.0f, y3 = 0.9f;

        Sierpinski(x1, y1, x2, y2, x3, y3, MAX_DEPTH);

        window.display();
    }

    return 0;
}
